/*
 *
 */

#ifndef HPSDR_DEFINITIONS_H_
#define HPSDR_DEFINITIONS_H_

// Unfortunately, the code number of the gear
// differs in old and new protocol

#define DEVICE_METIS            0
#define DEVICE_HERMES           1
#define DEVICE_GRIFFIN          2
#define DEVICE_ANGELIA          4
#define DEVICE_ORION            5
#define DEVICE_HERMES_LITE      6
#define DEVICE_HERMES_LITE2     1006
#define DEVICE_ORION2           10
#define DEVICE_C25              100

#define NEW_DEVICE_ATLAS        0
#define NEW_DEVICE_HERMES       1
#define NEW_DEVICE_HERMES2      2
#define NEW_DEVICE_ANGELIA      3
#define NEW_DEVICE_ORION        4
#define NEW_DEVICE_ORION2       5
#define NEW_DEVICE_HERMES_LITE  6
#define NEW_DEVICE_HERMES_LITE2 1006

// port definitions from host
#define GENERAL_REGISTERS_FROM_HOST_PORT 1024
#define PROGRAMMING_FROM_HOST_PORT 1024
#define RECEIVER_SPECIFIC_REGISTERS_FROM_HOST_PORT 1025
#define TRANSMITTER_SPECIFIC_REGISTERS_FROM_HOST_PORT 1026
#define HIGH_PRIORITY_FROM_HOST_PORT 1027
#define AUDIO_FROM_HOST_PORT 1028
#define TX_IQ_FROM_HOST_PORT 1029

// port definitions to host
#define COMMAND_RESPONCE_TO_HOST_PORT 1024
#define HIGH_PRIORITY_TO_HOST_PORT 1025
#define MIC_LINE_TO_HOST_PORT 1026
#define WIDE_BAND_TO_HOST_PORT 1027
#define RX_IQ_TO_HOST_PORT_0 1035
#define RX_IQ_TO_HOST_PORT_1 1036
#define RX_IQ_TO_HOST_PORT_2 1037
#define RX_IQ_TO_HOST_PORT_3 1038
#define RX_IQ_TO_HOST_PORT_4 1039
#define RX_IQ_TO_HOST_PORT_5 1040
#define RX_IQ_TO_HOST_PORT_6 1041
#define RX_IQ_TO_HOST_PORT_7 1042


#endif /* HPSDR_DEFINITIONS_H_ */
