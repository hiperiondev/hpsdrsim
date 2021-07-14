/*
 *
 */

#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include "debug.h"
#include "definitions.h"

// new protocol

void np_discovery(uint8_t *buffer, int new_protocol_running, int NEWDEVICE) {
    // prepare response
    memset(buffer, 0, 60);
    buffer[4] = 0x02 + new_protocol_running;
    buffer[5] = 0xAA;
    buffer[6] = 0xBB;
    buffer[7] = 0xCC;
    buffer[8] = 0xDD;
    buffer[9] = 0xEE;
    buffer[10] = 0xFF;
    buffer[11] = NEWDEVICE;
    buffer[12] = 38;
    buffer[13] = 19;
    buffer[20] = 2;
    buffer[21] = 1;
    buffer[22] = 3;
    // HERMES_LITE2 is a HermesLite with a new software version
    if (NEWDEVICE == NEW_DEVICE_HERMES_LITE2) {
        buffer[11] = NEW_DEVICE_HERMES_LITE;
    }
}

void np_erase_packet(uint8_t *buffer, int active_thread, int NEWDEVICE) {
    memset(buffer, 0, 60);
    buffer[4] = 0x02 + active_thread;
    buffer[5] = 0xAA;
    buffer[6] = 0xBB;
    buffer[7] = 0xCC;
    buffer[8] = 0xDD;
    buffer[9] = 0xEE;
    buffer[10] = 0xFF;
    buffer[11] = NEWDEVICE;
    buffer[12] = 38;
    buffer[13] = 103;
    buffer[20] = 2;
    buffer[21] = 1;
    buffer[22] = 3;
}

void np_program(uint8_t *buffer, unsigned long checksum, int NEWDEVICE) {
    memset(buffer + 4, 0, 56); // keep seq. no
    buffer[4] = 0x04;
    buffer[5] = 0xAA;
    buffer[6] = 0xBB;
    buffer[7] = 0xCC;
    buffer[8] = 0xDD;
    buffer[9] = 0xEE;
    buffer[10] = 0xFF;
    buffer[11] = 103;
    buffer[12] = NEWDEVICE;
    buffer[13] = (checksum >> 8) & 0xFF;
    buffer[14] = (checksum) & 0xFF;
}

void np_set_ip(uint8_t *buffer, int active_thread, int NEWDEVICE) {
    memset(buffer, 0, 60);
    buffer[4] = 0x02 + active_thread;
    buffer[5] = 0xAA;
    buffer[6] = 0xBB;
    buffer[7] = 0xCC;
    buffer[8] = 0xDD;
    buffer[9] = 0xEE;
    buffer[10] = 0xFF;
    buffer[11] = NEWDEVICE;
    buffer[12] = 38;
    buffer[13] = 103;
    buffer[20] = 2;
    buffer[21] = 1;
    buffer[22] = 3;
}

///////////////

// old_protocol


void op_erase_packet(uint8_t *buffer) {
    sleep(1);
    memset(buffer, 0, 60);
    buffer[0] = 0xEF;
    buffer[1] = 0xFE;
    buffer[2] = 0x03;
    buffer[3] = 0xAA;
    buffer[4] = 0xBB;
    buffer[5] = 0xCC;
    buffer[6] = 0xDD;
    buffer[7] = 0xEE;
    buffer[8] = 0xFF;
}

void op_program(uint8_t *buffer) {
    usleep(1000);
    memset(buffer, 0, 60);
    buffer[0] = 0xEF;
    buffer[1] = 0xFE;
    buffer[2] = 0x04;
    buffer[3] = 0xAA;
    buffer[4] = 0xBB;
    buffer[5] = 0xCC;
    buffer[6] = 0xDD;
    buffer[7] = 0xEE;
    buffer[8] = 0xFF;
}

void op_set_ip(uint8_t *buffer) {
    buffer[2] = 0x02;
    memset(buffer + 9, 0, 54);
}

////////////////

// ep2 functions
void ep2_adc1preamp(uint8_t *frame) {
}

void ep2_adc2preamp(uint8_t *frame) {
}

void ep2_adc3preamp(uint8_t *frame) {
}

void ep2_adc4preamp(uint8_t *frame) {
}

void ep2_alex6mlna(uint8_t *frame) {
}

void ep2_alexbyphpfs(uint8_t *frame) {
}

void ep2_alexhpf(uint8_t *frame) {
}

void ep2_alexlpf(uint8_t *frame) {
}

void ep2_alexmanhpflpf(uint8_t *frame) {
}

void ep2_alextrdis(uint8_t *frame) {
}

void ep2_c25extboarddata(uint8_t *frame) {
}

void ep2_commonmercuryfreq(uint8_t *frame) {
}

void ep2_cwdelay(uint8_t *frame) {
}

void ep2_cwhang(uint8_t *frame) {
}

void ep2_cwint(uint8_t *frame) {
}

void ep2_cw_mode(uint8_t *frame) {
}

void ep2_cwrev(uint8_t *frame) {
}

void ep2_cwspacing(uint8_t *frame) {
}

void ep2_cwspeed(uint8_t *frame) {
}

void ep2_cwweight(uint8_t *frame) {
}

void ep2_hermesconfig(uint8_t *frame) {
}

void ep2_linegain(uint8_t *frame) {
}

void ep2_mercuryattontx0(uint8_t *frame) {
}

void ep2_mercuryattontx1(uint8_t *frame) {
}

void ep2_metisdb9(uint8_t *frame) {
}

void ep2_micbias(uint8_t *frame) {
}

void ep2_micptt(uint8_t *frame) {
}

void ep2_micsrc(uint8_t *frame) {
}

void ep2_opencollector(uint8_t *frame) {
}

void ep2_penelopeselect(uint8_t *frame) {
}

void ep2_pm_config(uint8_t *frame) {
}

void ep2_ptt(uint8_t *frame) {
}

void ep2_puresignal(uint8_t *frame) {
}

void ep2_receivers(uint8_t *frame) {
}

void ep2_ref10mhz(uint8_t *frame) {
}

void ep2_rx1adc(uint8_t *frame) {
}

void ep2_rx1attenable(uint8_t *frame) {
}

void ep2_rx1att(uint8_t *frame) {
}

void ep2_rx1hlattgain(uint8_t *frame) {
}

void ep2_rx2adc(uint8_t *frame) {
}

void ep2_rx2att(uint8_t *frame) {
}

void ep2_rx3adc(uint8_t *frame) {
}

void ep2_rx4adc(uint8_t *frame) {
}

void ep2_rx5adc(uint8_t *frame) {
}

void ep2_rx6adc(uint8_t *frame) {
}

void ep2_rx7adc(uint8_t *frame) {
}

void ep2_rxfreq1(uint8_t *frame) {
}

void ep2_rxfreq2(uint8_t *frame) {
}

void ep2_rxfreq3(uint8_t *frame) {
}

void ep2_rxfreq4(uint8_t *frame) {
}

void ep2_rxfreq5(uint8_t *frame) {
}

void ep2_rxfreq6(uint8_t *frame) {
}

void ep2_rxfreq7(uint8_t *frame) {
}

void ep2_samplerate(uint8_t *frame) {
}

void ep2_sidetonefreq(uint8_t *frame) {
}

void ep2_sidetonevolume(uint8_t *frame) {
}

void ep2_src122mhz(uint8_t *frame) {
}

void ep2_timestampmic(uint8_t *frame) {
}

void ep2_tipring(uint8_t *frame) {
}

void ep2_txatt(uint8_t *frame) {
}

void ep2_txclasse(uint8_t *frame) {
}

void ep2_txdrive(uint8_t *frame) {
}

void ep2_txfreq(uint8_t *frame) {
}

void ep2_vnamode(uint8_t *frame) {
}

////////////////

// ep6 functions


////////////////
