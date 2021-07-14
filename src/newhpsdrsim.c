#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <math.h>

#define EXTERN extern
#include "hpsdrsim.h"
#include "debug.h"
#include "definitions.h"

#define NUMRECEIVERS 4

// These variables represent the state of the machine

// data from general packet
static int ddc_port = 0;
static int duc_port = 0;
static int hp_port = 0;
static int shp_port = 0;
static int audio_port = 0;
static int duc0_port = 0;
static int ddc0_port = 0;
static int mic_port = 0;
static int wide_port = 0;
static int wide_enable = 0;
static int wide_len = 0;
static int wide_size = 0;
static int wide_rate = 0;
static int wide_ppf = 0;
static int port_mm = 0;
static int port_smm = 0;
static int pwm_min = 0;
static int pwm_max = 0;
static int bits = 0;
static int hwtim = 0;
static int pa_enable = 0;
static int alex0_enable = 0;
static int alex1_enable = 0;
static int iqform = 0;

// data from rx specific packet
static int adc = 0;
static int adcdither[8];
static int adcrandom[8];
static int ddcenable[NUMRECEIVERS];
static int adcmap[NUMRECEIVERS];
static int rxrate[NUMRECEIVERS];
static int syncddc[NUMRECEIVERS];

// data from tx specific packet
static int dac = 0;
static int cwmode = 0;
static int sidelevel = 0;
static int sidefreq = 0;
static int speed = 0;
static int weight = 0;
static int hang = 0;
static int delay = 0;
static int txrate = 0;
static int ducbits = 0;
static int orion = 0;
static int gain = 0;
static int txatt = 0;

// stat from high-priority packet
static int run = 0;
static int ptt = 0;
static int cwx = 0;
static int dot = 0;
static int dash = 0;
static unsigned long rxfreq[NUMRECEIVERS];
static unsigned long txfreq = 0;
static int txdrive = 0;
static int w1400 = 0;  // Xvtr and Audio enable
static int ocout = 0;
static int db9 = 0;
static int mercury_atts = 0;
static int alex0[32];
static int alex1[32];
static int stepatt0 = 0;
static int stepatt1 = 0;

// floating point representation of TX-Drive and ADC0-Attenuator
static double rxatt0_dbl = 1.0;
static double rxatt1_dbl = 1.0;
static double txatt_dbl = 1.0;
static double txdrv_dbl = 0.0;

// End of state variables

static int txptr = 10000;

static pthread_t ddc_specific_thread_id;
static pthread_t duc_specific_thread_id;
static pthread_t rx_thread_id[NUMRECEIVERS];
static pthread_t tx_thread_id;
static pthread_t mic_thread_id;
static pthread_t audio_thread_id;
static pthread_t highprio_thread_id = 0;
static pthread_t send_highprio_thread_id;

void* ddc_specific_thread(void*);
void* duc_specific_thread(void*);
void* highprio_thread(void*);
void* send_highprio_thread(void*);
void* rx_thread(void*);
void* tx_thread(void*);
void* mic_thread(void*);
void* audio_thread(void*);

static double txlevel;

int new_protocol_running() {
    if (run)
        return 1;
    else
        return 0;
}

void new_protocol_general_packet(unsigned char *buffer) {
    dbg_printf(1, "-- new protocol packet received\n");
    static unsigned long seqnum = 0;
    unsigned long seqold;
    int rc;

    seqold = seqnum;
    seqnum = (buffer[0] >> 24) + (buffer[1] << 16) + (buffer[2] << 8) + buffer[3];
    if (seqnum != 0 && seqnum != seqold + 1) {
        dbg_printf(1, "GP: SEQ ERROR, old=%lu new=%lu\n", seqold, seqnum);
    }

    rc = (buffer[5] << 8) + buffer[6];
    if (rc == 0)
        rc = 1025;
    if (rc != ddc_port || !run) {
        ddc_port = rc;
        dbg_printf(1, "GP: RX specific rcv        port is  %4d\n", rc);
    }
    rc = (buffer[7] << 8) + buffer[8];
    if (rc == 0)
        rc = 1026;
    if (rc != duc_port || !run) {
        duc_port = rc;
        dbg_printf(1, "GP: TX specific rcv        port is  %4d\n", rc);
    }
    rc = (buffer[9] << 8) + buffer[10];
    if (rc == 0)
        rc = 1027;
    if (rc != hp_port || !run) {
        hp_port = rc;
        dbg_printf(1, "GP: HighPrio Port rcv      port is  %4d\n", rc);
    }
    rc = (buffer[11] << 8) + buffer[12];
    if (rc == 0)
        rc = 1025;
    if (rc != shp_port || !run) {
        shp_port = rc;
        dbg_printf(1, "GP: HighPrio Port snd      port is  %4d\n", rc);
    }
    rc = (buffer[13] << 8) + buffer[14];
    if (rc == 0)
        rc = 1028;
    if (rc != audio_port || !run) {
        audio_port = rc;
        dbg_printf(1, "GP: Audio rcv              port is  %4d\n", rc);
    }
    rc = (buffer[15] << 8) + buffer[16];
    if (rc == 0)
        rc = 1029;
    if (rc != duc0_port || !run) {
        duc0_port = rc;
        dbg_printf(1, "GP: TX data rcv base       port is  %4d\n", rc);
    }
    rc = (buffer[17] << 8) + buffer[18];
    if (rc == 0)
        rc = 1035;
    if (rc != ddc0_port || !run) {
        ddc0_port = rc;
        dbg_printf(1, "GP: RX data snd base       port is  %4d\n", rc);
    }
    rc = (buffer[19] << 8) + buffer[20];
    if (rc == 0)
        rc = 1026;
    if (rc != mic_port || !run) {
        mic_port = rc;
        dbg_printf(1, "GP: Microphone data snd    port is  %4d\n", rc);
    }
    rc = (buffer[21] << 8) + buffer[22];
    if (rc == 0)
        rc = 1027;
    if (rc != wide_port || !run) {
        wide_port = rc;
        dbg_printf(1, "GP: Wideband data snd      port is  %4d\n", rc);
    }
    rc = buffer[23];
    if (rc != wide_enable || !run) {
        wide_enable = rc;
        dbg_printf(1, "GP: Wideband Enable Flag is %d\n", rc);
    }
    rc = (buffer[24] << 8) + buffer[25];
    if (rc == 0)
        rc = 512;
    if (rc != wide_len || !run) {
        wide_len = rc;
        dbg_printf(1, "GP: WideBand Length is %d\n", rc);
    }
    rc = buffer[26];
    if (rc == 0)
        rc = 16;
    if (rc != wide_size || !run) {
        wide_size = rc;
        dbg_printf(1, "GP: Wideband sample size is %d\n", rc);
    }
    rc = buffer[27];
    if (rc != wide_rate || !run) {
        wide_rate = rc;
        dbg_printf(1, "GP: Wideband sample rate is %d\n", rc);
    }
    rc = buffer[28];
    if (rc != wide_ppf || !run) {
        wide_ppf = rc;
        dbg_printf(1, "GP: Wideband PPF is %d\n", rc);
    }
    rc = (buffer[29] << 8) + buffer[30];
    if (rc != port_mm || !run) {
        port_mm = rc;
        dbg_printf(1, "MemMapped Registers rcv port is %d\n", rc);
    }
    rc = (buffer[31] << 8) + buffer[32];
    if (rc != port_smm || !run) {
        port_smm = rc;
        dbg_printf(1, "MemMapped Registers snd port is %d\n", rc);
    }
    rc = (buffer[33] << 8) + buffer[34];
    if (rc != pwm_min || !run) {
        pwm_min = rc;
        dbg_printf(1, "GP: PWM Min value is %d\n", rc);
    }
    rc = (buffer[35] << 8) + buffer[36];
    if (rc != pwm_max || !run) {
        pwm_max = rc;
        dbg_printf(1, "GP: PWM Max value is %d\n", rc);
    }
    rc = buffer[37];
    if (rc != bits || !run) {
        bits = rc;
        dbg_printf(1, "GP: ModeBits=x%02x\n", rc);
    }
    rc = buffer[38];
    if (rc != hwtim || !run) {
        hwtim = rc;
        dbg_printf(1, "GP: Hardware Watchdog enabled=%d\n", rc);
    }

    iqform = buffer[39];
    if (iqform == 0)
        iqform = 3;
    if (iqform != 3)
        dbg_printf(1, "GP: Wrong IQ Format requested: %d\n", iqform);

    rc = (buffer[58] & 0x01);
    if (rc != pa_enable || !run) {
        pa_enable = rc;
        dbg_printf(1, "GP: PA enabled=%d\n", rc);
    }

    rc = buffer[59] & 0x01;
    if (rc != alex0_enable || !run) {
        alex0_enable = rc;
        dbg_printf(1, "GP: ALEX0 register enable=%d\n", rc);
    }
    rc = (buffer[59] & 0x02) >> 1;
    if (rc != alex1_enable || !run) {
        alex1_enable = rc;
        dbg_printf(1, "GP: ALEX1 register enable=%d\n", rc);
    }

    // Start HighPrio thread if we arrive here for the first time
    // The HighPrio thread keeps running all the time.
    if (!highprio_thread_id) {
        if (pthread_create(&highprio_thread_id, NULL, highprio_thread, NULL) < 0) {
            dbg_printf(1, "***** ERROR: Create HighPrio thread");
        }
        pthread_detach(highprio_thread_id);

        // init state arrays to zero for the first time
        memset(adcdither, -1, 8 * sizeof(int));
        memset(adcrandom, -1, 8 * sizeof(int));
        memset(ddcenable, -1, NUMRECEIVERS * sizeof(int));
        memset(adcmap, -1, NUMRECEIVERS * sizeof(int));
        memset(syncddc, -1, NUMRECEIVERS * sizeof(int));

        memset(rxfreq, -1, NUMRECEIVERS * sizeof(unsigned long));
        memset(alex0, 0, 32 * sizeof(int));
        memset(alex1, 0, 32 * sizeof(int));
    }
}

void* ddc_specific_thread(void *data) {
    dbg_printf(1, "-- Start ddc_specific_thread port: %d\n", ddc_port);
    int sock;
    struct sockaddr_in addr;
    socklen_t lenaddr = sizeof(addr);
    unsigned long seqnum, seqold;
    struct timeval tv;
    unsigned char buffer[2000];
    int yes = 1;
    int rc;
    int i, j;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        dbg_printf(1, "***** ERROR: RX specific: socket");
        return NULL;
    }

    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void*) &yes, sizeof(yes));
    setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, (void*) &yes, sizeof(yes));
    tv.tv_sec = 0;
    tv.tv_usec = 10000;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (void*) &tv, sizeof(tv));

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(ddc_port);

    if (bind(sock, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
        dbg_printf(1, "***** ERROR: RX specific: bind");
        close(sock);
        return NULL;
    }

    seqnum = 0;
    while (run) {
        rc = recvfrom(sock, buffer, 1444, 0, (struct sockaddr*) &addr, &lenaddr);
        if (rc < 0 && errno != EAGAIN) {
            dbg_printf(1, "***** ERROR: DDC specific thread: recvmsg");
            break;
        }
        if (rc < 0)
            continue;
        if (rc != 1444) {
            dbg_printf(1, "RXspec: Received DDC specific packet with incorrect length");
            break;
        }
        seqold = seqnum;
        seqnum = (buffer[0] >> 24) + (buffer[1] << 16) + (buffer[2] << 8) + buffer[3];
        if (seqnum != 0 && seqnum != seqold + 1) {
            dbg_printf(1, "RXspec: SEQ ERROR, old=%lu new=%lu\n", seqold, seqnum);
        }
        if (adc != buffer[4]) {
            adc = buffer[4];
            dbg_printf(1, "RX: Number of ADCs: %d\n", adc);
        }
        for (i = 0; i < adc; i++) {
            rc = (buffer[5] >> i) & 0x01;
            if (rc != adcdither[i]) {
                adcdither[i] = rc;
                dbg_printf(1, "RX: ADC%d dither=%d\n", i, rc);
            }
        }
        for (i = 0; i < adc; i++) {
            rc = (buffer[6] >> i) & 0x01;
            if (rc != adcrandom[i]) {
                adcrandom[i] = rc;
                dbg_printf(1, "RX: ADC%d random=%d\n", i, rc);
            }
        }

        for (i = 0; i < NUMRECEIVERS; i++) {
            int modified = 0;

            rc = buffer[17 + 6 * i];
            if (rc != adcmap[i]) {
                modified = 1;
                adcmap[i] = rc;
            }

            rc = (buffer[18 + 6 * i] << 8) + buffer[19 + 6 * i];
            if (rc != rxrate[i]) {
                modified = 1;
                rxrate[i] = rc;
                modified = 1;
            }

            if (syncddc[i] != buffer[1363 + i]) {
                syncddc[i] = buffer[1363 + i];
                modified = 1;
            }
            rc = (buffer[7 + (i / 8)] >> (i % 8)) & 0x01;
            if (rc != ddcenable[i]) {
                modified = 1;
                ddcenable[i] = rc;
            }
            if (modified) {
                dbg_printf(1, "RX: DDC%d Enable=%d ADC%d Rate=%d SyncMap=%02x\n", i, ddcenable[i], adcmap[i], rxrate[i], syncddc[i]);
                rc = 0;
                for (j = 0; j < 8; j++) {
                    rc += (syncddc[i] >> i) & 0x01;
                }
                if (rc > 1) {
                    dbg_printf(1, "WARNING:\n");
                    dbg_printf(1, "WARNING:\n");
                    dbg_printf(1, "WARNING:\n");
                    dbg_printf(1, "WARNING: more than two DDC sync'ed\n");
                    dbg_printf(1, "WARNING: this system is not prepared to handle this case\n");
                    dbg_printf(1, "WARNING: so are most of SDRs around!\n");
                    dbg_printf(1, "WARNING:\n");
                    dbg_printf(1, "WARNING:\n");
                    dbg_printf(1, "WARNING:\n");
                }
            }
        }
    }
    close(sock);
    return NULL;
}

void* duc_specific_thread(void *data) {
    dbg_printf(1, "-- Start duc_specific_thread port: %d\n", duc_port);
    int sock;
    struct sockaddr_in addr;
    socklen_t lenaddr = sizeof(addr);
    unsigned long seqnum, seqold;
    struct timeval tv;
    unsigned char buffer[100];
    int yes = 1;
    int rc;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        dbg_printf(1, "***** ERROR: TX specific: socket");
        return NULL;
    }

    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void*) &yes, sizeof(yes));
    setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, (void*) &yes, sizeof(yes));
    tv.tv_sec = 0;
    tv.tv_usec = 10000;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (void*) &tv, sizeof(tv));

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(duc_port);

    if (bind(sock, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
        dbg_printf(1, "***** ERROR: TXspec: bind");
        close(sock);
        return NULL;
    }

    seqnum = 0;
    while (run) {
        rc = recvfrom(sock, buffer, 60, 0, (struct sockaddr*) &addr, &lenaddr);
        if (rc < 0 && errno != EAGAIN) {
            dbg_printf(1, "***** ERROR: TXspec: recvmsg");
            break;
        }
        if (rc < 0)
            continue;
        if (rc != 60) {
            dbg_printf(1, "TX: wrong length\n");
            break;
        }
        seqold = seqnum;
        seqnum = (buffer[0] >> 24) + (buffer[1] << 16) + (buffer[2] << 8) + buffer[3];
        if (seqnum != 0 && seqnum != seqold + 1) {
            dbg_printf(1, "TX: SEQ ERROR, old=%lu new=%lu\n", seqold, seqnum);
        }
        if (dac != buffer[4]) {
            dac = buffer[4];
            dbg_printf(1, "TX: Number of DACs: %d\n", dac);
        }
        if (cwmode != buffer[5]) {
            cwmode = buffer[5];
            dbg_printf(1, "TX: CW mode bits = %x\n", cwmode);
        }
        if (sidelevel != buffer[6]) {
            sidelevel = buffer[6];
            dbg_printf(1, "TX: CW side tone level: %d\n", sidelevel);
        }
        rc = (buffer[7] << 8) + buffer[8];
        if (rc != sidefreq) {
            sidefreq = rc;
            dbg_printf(1, "TX: CW sidetone freq: %d\n", sidefreq);
        }
        if (speed != buffer[9]) {
            speed = buffer[9];
            dbg_printf(1, "TX: CW keyer speed: %d wpm\n", speed);
        }
        if (weight != buffer[10]) {
            weight = buffer[10];
            dbg_printf(1, "TX: CW weight: %d\n", weight);
        }
        rc = (buffer[11] << 8) + buffer[12];
        if (hang != rc) {
            hang = rc;
            dbg_printf(1, "TX: CW hang time: %d msec\n", hang);
        }
        if (delay != buffer[13]) {
            delay = buffer[13];
            dbg_printf(1, "TX: RF delay: %d msec\n", delay);
        }
        rc = (buffer[14] << 8) + buffer[15];
        if (txrate != rc) {
            txrate = rc;
            dbg_printf(1, "TX: DUC sample rate: %d\n", rc);
        }
        if (ducbits != buffer[16]) {
            ducbits = buffer[16];
            dbg_printf(1, "TX: DUC sample width: %d bits\n", ducbits);
        }
        if (orion != buffer[50]) {
            orion = buffer[50];
            dbg_printf(1, "TX: ORION bits (mic etc): %x\n", orion);
        }
        if (gain != buffer[51]) {
            gain = buffer[51];
            dbg_printf(1, "TX: LineIn Gain (dB): %f\n", 12.0 - 1.5 * gain);
        }
        if (txatt != buffer[59]) {
            txatt = buffer[59];
            txatt_dbl = pow(10.0, -0.05 * txatt);
            dbg_printf(1, "TX: ATT DUC0/ADC0: %d\n", txatt);
        }
    }
    close(sock);
    return NULL;
}

void* highprio_thread(void *data) {
    dbg_printf(1, "-- Start highprio_thread port: %d\n", hp_port);
    int sock;
    struct sockaddr_in addr;
    socklen_t lenaddr = sizeof(addr);
    unsigned long seqnum, seqold;
    unsigned char buffer[2000];
    struct timeval tv;
    int yes = 1;
    int rc;
    unsigned long freq;
    int i;

    seqnum = 0;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        dbg_printf(1, "***** ERROR: HP: socket");
        return NULL;
    }

    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void*) &yes, sizeof(yes));
    setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, (void*) &yes, sizeof(yes));
    tv.tv_sec = 0;
    tv.tv_usec = 10000;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (void*) &tv, sizeof(tv));

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(hp_port);

    if (bind(sock, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
        dbg_printf(1, "***** ERROR: HP: bind");
        close(sock);
        return NULL;
    }

    while (1) {
        rc = recvfrom(sock, buffer, 1444, 0, (struct sockaddr*) &addr, &lenaddr);
        if (rc < 0 && errno != EAGAIN) {
            dbg_printf(1, "***** ERROR: HighPrio thread: recvmsg");
            break;
        }
        if (rc < 0)
            continue;
        if (rc != 1444) {
            dbg_printf(1, "Received HighPrio packet with incorrect length");
            break;
        }
        seqold = seqnum;
        seqnum = (buffer[0] >> 24) + (buffer[1] << 16) + (buffer[2] << 8) + buffer[3];
        if (seqnum != 0 && seqnum != seqold + 1) {
            dbg_printf(1, "HP: SEQ ERROR, old=%lu new=%lu\n", seqold, seqnum);
        }
        rc = (buffer[4] >> 0) & 0x01;
        if (rc != run) {
            run = rc;
            dbg_printf(1, "HP: Run=%d\n", rc);
            // if run=0, wait for threads to complete, otherwise spawn them off
            if (run) {
                if (pthread_create(&ddc_specific_thread_id, NULL, ddc_specific_thread, NULL) < 0) {
                    dbg_printf(1, "***** ERROR: Create DDC specific thread");
                }
                if (pthread_create(&duc_specific_thread_id, NULL, duc_specific_thread, NULL) < 0) {
                    dbg_printf(1, "***** ERROR: Create DUC specific thread");
                }
                for (i = 0; i < NUMRECEIVERS; i++) {
                    if (pthread_create(&rx_thread_id[i], NULL, rx_thread, (void*) (uintptr_t) i) < 0) {
                        dbg_printf(1, "***** ERROR: Create RX thread");
                    }
                }
                if (pthread_create(&tx_thread_id, NULL, tx_thread, NULL) < 0) {
                    dbg_printf(1, "***** ERROR: Create TX thread");
                }
                if (pthread_create(&send_highprio_thread_id, NULL, send_highprio_thread, NULL) < 0) {
                    dbg_printf(1, "***** ERROR: Create SendHighPrio thread");
                }
                if (pthread_create(&mic_thread_id, NULL, mic_thread, NULL) < 0) {
                    dbg_printf(1, "***** ERROR: Create Mic thread");
                }
                if (pthread_create(&audio_thread_id, NULL, audio_thread, NULL) < 0) {
                    dbg_printf(1, "***** ERROR: Create Audio thread");
                }
            } else {
                pthread_join(ddc_specific_thread_id, NULL);
                pthread_join(duc_specific_thread_id, NULL);
                for (i = 0; i < NUMRECEIVERS; i++) {
                    pthread_join(rx_thread_id[i], NULL);
                }
                pthread_join(send_highprio_thread_id, NULL);
                pthread_join(tx_thread_id, NULL);
                pthread_join(mic_thread_id, NULL);
                pthread_join(audio_thread_id, NULL);
                highprio_thread_id = 0;
                dbg_printf(1, "HP thread terminated.\n");
                close(sock);
                return NULL;
            }
        }
        rc = (buffer[4] >> 1) & 0x01;
        if (rc != ptt) {
            ptt = rc;
            dbg_printf(1, "HP: PTT=%d\n", rc);
            if (ptt == 0) {
                memset(isample, 0, sizeof(float) * NEWRTXLEN);
                memset(qsample, 0, sizeof(float) * NEWRTXLEN);
            }
        }
        rc = (buffer[5] >> 0) & 0x01;
        if (rc != cwx) {
            cwx = rc;
            dbg_printf(1, "HP: CWX=%d\n", rc);
        }
        rc = (buffer[5] >> 1) & 0x01;
        if (rc != dot) {
            dot = rc;
            dbg_printf(1, "HP: DOT=%d\n", rc);
        }
        rc = (buffer[5] >> 2) & 0x01;
        if (rc != dash) {
            dash = rc;
            dbg_printf(1, "HP: DASH=%d\n", rc);
        }
        for (i = 0; i < NUMRECEIVERS; i++) {
            freq = (buffer[9 + 4 * i] << 24) + (buffer[10 + 4 * i] << 16) + (buffer[11 + 4 * i] << 8) + buffer[12 + 4 * i];
            if (bits & 0x08) {
                freq = round(122880000.0 * (double) freq / 4294967296.0);
            }
            if (freq != rxfreq[i]) {
                rxfreq[i] = freq;
                dbg_printf(1, "HP: DDC%d freq: %lu\n", i, freq);
            }
        }
        freq = (buffer[329] << 24) + (buffer[330] << 16) + (buffer[331] << 8) + buffer[332];
        if (bits & 0x08) {
            freq = round(122880000.0 * (double) freq / 4294967296.0);
        }
        if (freq != txfreq) {
            txfreq = freq;
            dbg_printf(1, "HP: DUC freq: %lu\n", freq);
        }
        rc = buffer[345];
        if (rc != txdrive) {
            txdrive = rc;
            txdrv_dbl = (double) txdrive * 0.003921568627;
            dbg_printf(1, "HP: TX drive= %d (%f)\n", txdrive, txdrv_dbl);
        }
        rc = buffer[1400];
        if (rc != w1400) {
            w1400 = rc;
            dbg_printf(1, "HP: Xvtr/Audio enable=%x\n", rc);
        }
        rc = buffer[1401];
        if (rc != ocout) {
            ocout = rc;
            dbg_printf(1, "HP: OC outputs=%x\n", rc);
        }
        rc = buffer[1402];
        if (rc != db9) {
            db9 = rc;
            dbg_printf(1, "HP: Outputs DB9=%x\n", rc);
        }
        rc = buffer[1403];
        if (rc != mercury_atts) {
            mercury_atts = rc;
            dbg_printf(1, "HP: MercuryAtts=%x\n", rc);
        }
        // Store Alex0 and Alex1 bits in separate ints
        freq = (buffer[1428] << 24) + (buffer[1429] << 16) + (buffer[1430] << 8) + buffer[1431];
        for (i = 0; i < 32; i++) {
            rc = (freq >> i) & 0x01;
            if (rc != alex1[i]) {
                alex1[i] = rc;
                dbg_printf(1, "HP: ALEX1 bit%d set to %d\n", i, rc);
            }
        }
        freq = (buffer[1432] << 24) + (buffer[1433] << 16) + (buffer[1434] << 8) + buffer[1435];
        for (i = 0; i < 32; i++) {
            rc = (freq >> i) & 0x01;
            if (rc != alex0[i]) {
                alex0[i] = rc;
                dbg_printf(1, "HP: ALEX0 bit%d set to %d\n", i, rc);
            }
        }
        rc = buffer[1442];
        if (rc != stepatt1) {
            stepatt1 = rc;
            rxatt1_dbl = pow(10.0, -0.05 * stepatt1);
            dbg_printf(1, "HP: StepAtt1 = %d\n", rc);
        }
        rc = buffer[1443];
        if (rc != stepatt0) {
            stepatt0 = rc;
            dbg_printf(1, "HP: StepAtt0 = %d\n", stepatt0);
        }
        // rxatt0 depends both on ALEX att and Step Att, so re-calc. it each time
        if (NEWDEVICE == NEW_DEVICE_ORION2) {
            // There is no step attenuator on ANAN7000
            rxatt0_dbl = pow(10.0, -0.05 * stepatt0);
        } else {
            rxatt0_dbl = pow(10.0, -0.05 * (stepatt0 + 10 * alex0[14] + 20 * alex0[13]));
        }
    }
    close(sock);
    return NULL;
}

void* rx_thread(void *data) {
    int sock;
    struct sockaddr_in addr;
    // One instance of this thread is started for each DDC
    unsigned long seqnum;
    unsigned char buffer[1444];
    int yes = 1;
    int i;
    long wait;
    double i0sample, q0sample;
    double i1sample, q1sample;
    double irsample, qrsample;
    double fac;
    int sample;
    unsigned char *p;
    int noisept, tonept;
    int myddc;
    int sync, size;
    int myadc, syncadc;
    int rxptr;
    int divptr;
    int decimation;
    unsigned int seed;

    struct timespec delay;

    syncadc = 0;

    myddc = (int) (uintptr_t) data;
    if (myddc < 0 || myddc >= NUMRECEIVERS)
        return NULL;

    dbg_printf(1, "-- Start rx_thread port: %d\n", ddc0_port + myddc);

    seqnum = 0;
    // unique seed value for random number generator
    seed = ((uintptr_t) &seed) & 0xffffff;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        dbg_printf(1, "***** ERROR: RXthread: socket");
        return NULL;
    }

    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void*) &yes, sizeof(yes));
    setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, (void*) &yes, sizeof(yes));

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(ddc0_port + myddc);

    if (bind(sock, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
        dbg_printf(1, "***** ERROR: RXthread: bind");
        close(sock);
        return NULL;
    }

    tonept = noisept = 0;
    clock_gettime(CLOCK_MONOTONIC, &delay);
    dbg_printf(1, "RX thread %d, enabled=%d\n", myddc, ddcenable[myddc]);
    rxptr = txptr - 4096;
    if (rxptr < 0)
        rxptr += NEWRTXLEN;
    divptr = 0;
    while (run) {
        if (ddcenable[myddc] <= 0 || rxrate[myddc] == 0 || rxfreq[myddc] == 0) {
            usleep(5000);
            clock_gettime(CLOCK_MONOTONIC, &delay);
            rxptr = txptr - 4096;
            if (rxptr < 0)
                rxptr += NEWRTXLEN;
            continue;
        }
        decimation = 1536 / rxrate[myddc];
        myadc = adcmap[myddc];
        // for simplicity, we only allow for a single "synchronized" DDC,
        // this well covers the PURESIGNAL and DIVERSITY cases
        sync = 0;
        i = syncddc[myddc];
        while (i) {
            sync++;
            i = i >> 1;
        }
        // sync == 0 means no synchronizatsion
        // sync == 1,2,3  means synchronization with DDC0,1,2
        // Usually we send 238 samples per buffer, but with synchronization
        // we send 119 sample *pairs*.
        if (sync) {
            size = 119;
            wait = 119000000L / rxrate[myddc]; // time for these samples in nano-secs
            syncadc = adcmap[sync - 1];
        } else {
            size = 238;
            wait = 238000000L / rxrate[myddc]; // time for these samples in nano-secs
        }

        // ADC0 RX: noise + 800Hz signal at -73 dBm
        // ADC0 TX: noise + distorted TX signal
        // ADC1 RX: noise
        // ADC1 TX: HERMES only: original TX signal
        // ADC2   : original TX signal
        p = buffer;
        *p++ = (seqnum >> 24) & 0xFF;
        *p++ = (seqnum >> 16) & 0xFF;
        *p++ = (seqnum >> 8) & 0xFF;
        *p++ = (seqnum >> 0) & 0xFF;
        seqnum += 1;
        // do not use time stamps
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        // 24 bits per sample *ALWAYS*
        *p++ = 0;
        *p++ = 24;
        *p++ = 0;
        *p++ = sync ? 2 * size : size;  // should be 238 in either case
        for (i = 0; i < size; i++) {
            // produce noise depending on the ADC
            i1sample = i0sample = noiseItab[noisept];
            q1sample = q0sample = noiseQtab[noisept++];
            if (noisept == LENNOISE)
                noisept = rand_r(&seed) / NOISEDIV;
            // PS: produce sample PAIRS,
            // a) distorted TX data (with Drive and Attenuation)
            // b) original TX data (normalized)
            //
            // DIV: produce sample PAIRS,
            // a) add man-made-noise on I-sample of RX channel
            // b) add man-made-noise on Q-sample of "synced" channel
            if (sync && (rxrate[myadc] == 192) && ptt && (syncadc == adc)) {
                irsample = isample[rxptr];
                qrsample = qsample[rxptr++];
                if (rxptr >= NEWRTXLEN)
                    rxptr = 0;
                fac = txatt_dbl * txdrv_dbl * (IM3a + IM3b * (irsample * irsample + qrsample * qrsample) * txdrv_dbl * txdrv_dbl);
                if (myadc == 0) {
                    i0sample += irsample * fac;
                    q0sample += qrsample * fac;
                }
                i1sample = irsample * 0.2899;
                q1sample = qrsample * 0.2899;
            } else if (myadc == 0) {
                i0sample += toneItab[tonept] * 0.0002239 * rxatt0_dbl;
                q0sample += toneQtab[tonept] * 0.0002239 * rxatt0_dbl;
                tonept += decimation;
                if (tonept >= LENTONE)
                    tonept = 0;
            }
            if (diversity && !sync && myadc == 0) {
                i0sample += 0.0001 * rxatt0_dbl * divtab[divptr];
                divptr += decimation;
                if (divptr >= LENDIV)
                    divptr = 0;
            }
            if (diversity && !sync && myadc == 1) {
                q0sample += 0.0002 * rxatt1_dbl * divtab[divptr];
                divptr += decimation;
                if (divptr >= LENDIV)
                    divptr = 0;
            }
            if (diversity && sync && !ptt) {
                if (myadc == 0)
                    i0sample += 0.0001 * rxatt0_dbl * divtab[divptr];
                if (syncadc == 1)
                    q1sample += 0.0002 * rxatt1_dbl * divtab[divptr];
                divptr += decimation;
                if (divptr >= LENDIV)
                    divptr = 0;
            }
            if (sync) {
                sample = i0sample * 8388607.0;
                *p++ = (sample >> 16) & 0xFF;
                *p++ = (sample >> 8) & 0xFF;
                *p++ = (sample >> 0) & 0xFF;
                sample = q0sample * 8388607.0;
                *p++ = (sample >> 16) & 0xFF;
                *p++ = (sample >> 8) & 0xFF;
                *p++ = (sample >> 0) & 0xFF;
                sample = i1sample * 8388607.0;
                *p++ = (sample >> 16) & 0xFF;
                *p++ = (sample >> 8) & 0xFF;
                *p++ = (sample >> 0) & 0xFF;
                sample = q1sample * 8388607.0;
                *p++ = (sample >> 16) & 0xFF;
                *p++ = (sample >> 8) & 0xFF;
                *p++ = (sample >> 0) & 0xFF;
            } else {
                sample = i0sample * 8388607.0;
                *p++ = (sample >> 16) & 0xFF;
                *p++ = (sample >> 8) & 0xFF;
                *p++ = (sample >> 0) & 0xFF;
                sample = q0sample * 8388607.0;
                *p++ = (sample >> 16) & 0xFF;
                *p++ = (sample >> 8) & 0xFF;
                *p++ = (sample >> 0) & 0xFF;
            }
        }
        delay.tv_nsec += wait;
        while (delay.tv_nsec >= 1000000000) {
            delay.tv_nsec -= 1000000000;
            delay.tv_sec++;
        }

        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &delay, NULL);

        if (sendto(sock, buffer, 1444, 0, (struct sockaddr*) &addr_new, sizeof(addr_new)) < 0) {
            dbg_printf(1, "***** ERROR: RX thread sendto");
            break;
        }
    }
    close(sock);
    return NULL;
}

// This thread receives data (TX samples) from the PC
void* tx_thread(void *data) {
    dbg_printf(1, "-- Start tx_thread port: %d\n", duc0_port);
    int sock;
    struct sockaddr_in addr;
    socklen_t lenaddr = sizeof(addr);
    unsigned long seqnum, seqold;
    unsigned char buffer[1444];
    int yes = 1;
    int rc;
    int i;
    unsigned char *p;
    int sample;
    double di, dq;
    double sum;
    struct timeval tv;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        dbg_printf(1, "***** ERROR: TX: socket");
        return NULL;
    }

    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void*) &yes, sizeof(yes));
    setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, (void*) &yes, sizeof(yes));
    tv.tv_sec = 0;
    tv.tv_usec = 10000;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (void*) &tv, sizeof(tv));

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(duc0_port);

    if (bind(sock, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
        dbg_printf(1, "***** ERROR: TX: bind");
        close(sock);
        return NULL;
    }

    seqnum = 0;
    while (run) {
        rc = recvfrom(sock, buffer, 1444, 0, (struct sockaddr*) &addr, &lenaddr);
        if (rc < 0 && errno != EAGAIN) {
            dbg_printf(1, "***** ERROR: TX thread: recvmsg");
            break;
        }
        if (rc < 0)
            continue;
        if (rc != 1444) {
            dbg_printf(1, "Received TX packet with incorrect length");
            break;
        }
        seqold = seqnum;
        seqnum = (buffer[0] << 24) + (buffer[1] << 16) + (buffer[2] << 8) + buffer[3];
        if (seqnum != 0 && seqnum != seqold + 1) {
            dbg_printf(1, "TXthread: SEQ ERROR, old=%lu new=%lu\n", seqold, seqnum);
        }
        p = buffer + 4;
        sum = 0.0;
        for (i = 0; i < 240; i++) {
            // process 240 TX iq samples
            sample = (int) ((signed char) (*p++)) << 16;
            sample |= (int) ((((unsigned char) (*p++)) << 8) & 0xFF00);
            sample |= (int) ((unsigned char) (*p++) & 0xFF);
            di = (double) sample / 8388608.0;
            sample = (int) ((signed char) (*p++)) << 16;
            sample |= (int) ((((unsigned char) (*p++)) << 8) & 0xFF00);
            sample |= (int) ((unsigned char) (*p++) & 0xFF);
            dq = (double) sample / 8388608.0;

            // I don't know why (perhaps the CFFIR in the SDR program)
            // but somehow I must multiply the samples to get the correct
            // strength
            di *= 1.118;
            dq *= 1.118;

            // put TX samples into ring buffer
            isample[txptr] = di;
            qsample[txptr++] = dq;
            if (txptr >= NEWRTXLEN)
                txptr = 0;

            // accumulate TX power
            sum += (di * di + dq * dq);
            data_print("TX", di, dq);
        }
        txlevel = sum * txdrv_dbl * txdrv_dbl * 0.0041667;
    }
    close(sock);
    return NULL;
}

void* send_highprio_thread(void *data) {
    dbg_printf(1, "-- Start send_highprio_thread port: %d\n", shp_port);
    int sock;
    struct sockaddr_in addr;
    unsigned long seqnum;
    unsigned char buffer[60];
    int yes = 1;
    int rc;
    unsigned char *p;

    seqnum = 0;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        dbg_printf(1, "***** ERROR: SendHighPrio thread: socket");
        return NULL;
    }

    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void*) &yes, sizeof(yes));
    setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, (void*) &yes, sizeof(yes));

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(shp_port);

    if (bind(sock, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
        dbg_printf(1, "***** ERROR: SendHighPrio thread: bind");
        close(sock);
        return NULL;
    }

    seqnum = 0;
    while (1) {
        if (!run) {
            close(sock);
            break;
        }
        // prepare buffer
        memset(buffer, 0, 60);
        p = buffer;
        *p++ = (seqnum >> 24) & 0xFF;
        *p++ = (seqnum >> 16) & 0xFF;
        *p++ = (seqnum >> 8) & 0xFF;
        *p++ = (seqnum >> 0) & 0xFF;
        *p++ = 0;  // no PTT and CW attached
        *p++ = 0;  // no ADC overload
        *p++ = 0;
        *p++ = txdrive;

        p += 6;

        rc = (int) ((4095.0 / c1) * sqrt(100.0 * txlevel * c2));
        *p++ = (rc >> 8) & 0xFF;
        *p++ = (rc) & 0xFF;

        buffer[49] = 63;  // about 13 volts supply

        if (sendto(sock, buffer, 60, 0, (struct sockaddr*) &addr_new, sizeof(addr_new)) < 0) {
            dbg_printf(1, "***** ERROR: HP send thread sendto");
            break;
        }
        seqnum++;
        usleep(50000);  // wait 50 msec then send again
    }
    close(sock);
    return NULL;
}

// This thread receives the audio samples and plays them
void* audio_thread(void *data) {
    dbg_printf(1, "-- Start audio_thread port: %d\n", audio_port);
    int sock;
    struct sockaddr_in addr;
    socklen_t lenaddr = sizeof(addr);
    unsigned long seqnum, seqold;
    unsigned char buffer[260];
    int yes = 1;
    int rc;
    int i;
    unsigned char *p;
    int16_t lsample, rsample;
    struct timeval tv;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        dbg_printf(1, "***** ERROR: Audio: socket");
        return NULL;
    }

    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void*) &yes, sizeof(yes));
    setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, (void*) &yes, sizeof(yes));
    tv.tv_sec = 0;
    tv.tv_usec = 10000;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (void*) &tv, sizeof(tv));

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(audio_port);

    if (bind(sock, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
        dbg_printf(1, "***** ERROR: Audio: bind");
        close(sock);
        return NULL;
    }

    seqnum = 0;
    while (run) {
        rc = recvfrom(sock, buffer, 260, 0, (struct sockaddr*) &addr, &lenaddr);
        if (rc < 0 && errno != EAGAIN) {
            dbg_printf(1, "***** ERROR: Audio thread: recvmsg");
            break;
        }
        if (rc < 0)
            continue;
        if (rc != 260) {
            dbg_printf(1, "Received Audio packet with incorrect length");
            break;
        }
        seqold = seqnum;
        seqnum = (buffer[0] << 24) + (buffer[1] << 16) + (buffer[2] << 8) + buffer[3];
        if (seqnum != 0 && seqnum != seqold + 1) {
            dbg_printf(1, "Audio thread: SEQ ERROR, old=%lu new=%lu\n", seqold, seqnum);
        }
        p = buffer + 4;
        for (i = 0; i < 64; i++) {
            lsample = ((signed char) *p++) << 8;
            lsample |= (*p++ & 0xff);
            rsample = ((signed char) *p++) << 8;
            rsample |= (*p++ & 0xff);
        }
    }
    close(sock);
    return NULL;
}

// The microphone thread just sends silence
void* mic_thread(void *data) {
    dbg_printf(1, "-- Start mic_thread port: %d\n", mic_port);
    int sock;
    struct sockaddr_in addr;
    unsigned long seqnum;
    unsigned char buffer[132];
    unsigned char *p;
    int yes = 1;
    struct timespec delay;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        dbg_printf(1, "***** ERROR: Mic thread: socket");
        return NULL;
    }

    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void*) &yes, sizeof(yes));
    setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, (void*) &yes, sizeof(yes));

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(mic_port);

    if (bind(sock, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
        dbg_printf(1, "***** ERROR: Mic thread: bind");
        close(sock);
        return NULL;
    }

    seqnum = 0;
    memset(buffer, 0, 132);
    clock_gettime(CLOCK_MONOTONIC, &delay);
    while (run) {
        // update seq number
        p = buffer;
        *p++ = (seqnum >> 24) & 0xFF;
        *p++ = (seqnum >> 16) & 0xFF;
        *p++ = (seqnum >> 8) & 0xFF;
        *p++ = (seqnum >> 0) & 0xFF;
        seqnum++;
        // 64 samples with 48000 kHz, makes 1333333 nsec
        delay.tv_nsec += 1333333;
        while (delay.tv_nsec >= 1000000000) {
            delay.tv_nsec -= 1000000000;
            delay.tv_sec++;
        }

        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &delay, NULL);

        if (sendto(sock, buffer, 132, 0, (struct sockaddr*) &addr_new, sizeof(addr_new)) < 0) {
            dbg_printf(1, "***** ERROR: Mic thread sendto");
            break;
        }
    }
    close(sock);
    return NULL;
}
