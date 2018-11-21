#ifndef XPACKET_H_
#define XPACKET_H_

#include <stdint.h>

#define	PACKET_MARK  0x4B4C

typedef struct {
	uint16_t	mark;
	uint8_t		type;
	uint16_t	data_len;
} __attribute__((packed)) Packet_Header;

typedef struct {
	Packet_Header 	header;
	uint8_t 	crc;
	char		*data;
} __attribute__((packed)) Data_Packet;

typedef struct {
	uint32_t	uptime;
	uint16_t	battery_voltage;
} __attribute__((packed)) Status_Packet;

enum packet_type{
	packet_type_status = 0,
	packet_type_nack,
	packet_type_ack,
	packet_type_lidar_data
};

uint8_t calc_crc8(uint8_t* data, uint16_t len);



#endif
