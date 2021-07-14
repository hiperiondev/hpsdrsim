#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

// new protocol
void np_discovery(uint8_t *buffer, int new_protocol_running, int NEWDEVICE);
void np_erase_packet(uint8_t* buffer, int active_thread, int NEWDEVICE);
void np_program(uint8_t *buffer, unsigned long checksum, int NEWDEVICE);
void np_set_ip(uint8_t *buffer, int active_thread, int NEWDEVICE);

///////////////

// old protocol
void op_program(uint8_t *buffer);
void op_erase_packet(uint8_t *buffer);
void op_set_ip(uint8_t *buffer);

///////////////

// ep2 functions
void ep2_adc1preamp(uint8_t *frame, int* reg, int val, char* str);
void ep2_adc2preamp(uint8_t *frame, int* reg, int val, char* str);
void ep2_adc3preamp(uint8_t *frame, int* reg, int val, char* str);
void ep2_adc4preamp(uint8_t *frame, int* reg, int val, char* str);
void ep2_alex6mlna(uint8_t *frame, int* reg, int val, char* str);
void ep2_alexbyphpfs(uint8_t *frame, int* reg, int val, char* str);
void ep2_alexhpf(uint8_t *frame, int* reg, int val, char* str);
void ep2_alexlpf(uint8_t *frame, int* reg, int val, char* str);
void ep2_alexmanhpflpf(uint8_t *frame, int* reg, int val, char* str);
void ep2_alextrdis(uint8_t *frame, int* reg, int val, char* str);
void ep2_c25extboarddata(uint8_t *frame, int* reg, int val, char* str);
void ep2_commonmercuryfreq(uint8_t *frame, int* reg, int val, char* str);
void ep2_cwdelay(uint8_t *frame, int* reg, int val, char* str);
void ep2_cwhang(uint8_t *frame, int* reg, int val, char* str);
void ep2_cwint(uint8_t *frame, int* reg, int val, char* str);
void ep2_cw_mode(uint8_t *frame, int* reg, int val, char* str);
void ep2_cwrev(uint8_t *frame, int* reg, int val, char* str);
void ep2_cwspacing(uint8_t *frame, int* reg, int val, char* str);
void ep2_cwspeed(uint8_t *frame, int* reg, int val, char* str);
void ep2_cwweight(uint8_t *frame, int* reg, int val, char* str);
void ep2_hermesconfig(uint8_t *frame, int* reg, int val, char* str);
void ep2_linegain(uint8_t *frame, int* reg, int val, char* str);
void ep2_mercuryattontx0(uint8_t *frame, int* reg, int val, char* str);
void ep2_mercuryattontx1(uint8_t *frame, int* reg, int val, char* str);
void ep2_metisdb9(uint8_t *frame, int* reg, int val, char* str);
void ep2_micbias(uint8_t *frame, int* reg, int val, char* str);
void ep2_micptt(uint8_t *frame, int* reg, int val, char* str);
void ep2_micsrc(uint8_t *frame, int* reg, int val, char* str);
void ep2_opencollector(uint8_t *frame, int* reg, int val, char* str);
void ep2_penelopeselect(uint8_t *frame, int* reg, int val, char* str);
void ep2_pm_config(uint8_t *frame, int* reg, int val, char* str);
void ep2_ptt(uint8_t *frame, int* reg, int val, char* str);
void ep2_puresignal(uint8_t *frame, int* reg, int val, char* str);
void ep2_receivers(uint8_t *frame, int* reg, int val, char* str);
void ep2_ref10mhz(uint8_t *frame, int* reg, int val, char* str);
void ep2_rx1adc(uint8_t *frame, int* reg, int val, char* str);
void ep2_rx1attenable(uint8_t *frame, int* reg, int val, char* str);
void ep2_rx1att(uint8_t *frame, int* reg, int val, char* str);
void ep2_rx1hlattgain(uint8_t *frame, int* reg, int val, char* str);
void ep2_rx2adc(uint8_t *frame, int* reg, int val, char* str);
void ep2_rx2att(uint8_t *frame, int* reg, int val, char* str);
void ep2_rx3adc(uint8_t *frame, int* reg, int val, char* str);
void ep2_rx4adc(uint8_t *frame, int* reg, int val, char* str);
void ep2_rx5adc(uint8_t *frame, int* reg, int val, char* str);
void ep2_rx6adc(uint8_t *frame, int* reg, int val, char* str);
void ep2_rx7adc(uint8_t *frame, int* reg, int val, char* str);
void ep2_rxfreq1(uint8_t *frame, long* reg, int val, char* str);
void ep2_rxfreq2(uint8_t *frame, long* reg, int val, char* str);
void ep2_rxfreq3(uint8_t *frame, long* reg, int val, char* str);
void ep2_rxfreq4(uint8_t *frame, long* reg, int val, char* str);
void ep2_rxfreq5(uint8_t *frame, long* reg, int val, char* str);
void ep2_rxfreq6(uint8_t *frame, long* reg, int val, char* str);
void ep2_rxfreq7(uint8_t *frame, long* reg, int val, char* str);
void ep2_samplerate(uint8_t *frame, int* reg, int val, char* str);
void ep2_sidetonefreq(uint8_t *frame, int* reg, int val, char* str);
void ep2_sidetonevolume(uint8_t *frame, int* reg, int val, char* str);
void ep2_src122mhz(uint8_t *frame, int* reg, int val, char* str);
void ep2_timestampmic(uint8_t *frame, int* reg, int val, char* str);
void ep2_tipring(uint8_t *frame, int* reg, int val, char* str);
void ep2_txatt(uint8_t *frame, int* reg, int val, char* str);
void ep2_txclasse(uint8_t *frame, int* reg, int val, char* str);
void ep2_txdrive(uint8_t *frame, int* reg, int val, char* str);
void ep2_txfreq(uint8_t *frame, long* reg, int val, char* str);
void ep2_vnamode(uint8_t *frame, int* reg, int val, char* str);

////////////////

// ep6 functions



////////////////

#endif
