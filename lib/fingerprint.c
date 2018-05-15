/*
 * fingerprint.c
 *
 *  Created on: 09.05.2018
 *      Author: Michael Sauer
 */
#include "fingerprint.h"
#include "delay.h"
#include "uart2.h"

#define GET_CMD_PACKET(...) \
  uint8_t data[] = {__VA_ARGS__}; \
  Fingerprint_Get_Cmd_Packet(&currPacket, FINGERPRINT_COMMANDPACKET, sizeof(data), data); \
  Fingerprint_writeStructuredPacket(&currPacket); \
  if (Fingerprint_getStructuredPacket(&currPacket, DEFAULTTIMEOUT) != FINGERPRINT_OK) return FINGERPRINT_PACKETRECIEVEERR; \
  if (currPacket.type != FINGERPRINT_ACKPACKET) return FINGERPRINT_PACKETRECIEVEERR;

#define SEND_CMD_PACKET(...) GET_CMD_PACKET(__VA_ARGS__); return currPacket.data[0];

#define SERIAL_WRITE(...) USART1_Put(__VA_ARGS__); //USART2_Put(__VA_ARGS__);
#define SERIAL_WRITE_U16(v) SERIAL_WRITE((uint8_t)(v>>8)); SERIAL_WRITE((uint8_t)(v & 0xFF));

uint32_t thePassword;
uint32_t theAddress;
uint8_t recvPacket[20];
Fingerprint_Packet currPacket;

void Fingerprint_Get_Cmd_Packet(Fingerprint_Packet *packet, uint8_t type, uint16_t length, uint8_t * data);

void Fingerprint_Init(uint32_t password)
{
	/* USART1 Init */
	USART1_Init(57600);

	thePassword = password;
	theAddress = 0xFFFFFFFF;
}

void Fingerprint_writeStructuredPacket(Fingerprint_Packet *packet)
{
	SERIAL_WRITE(packet->address[0]); // first byte goes into nowhere?
	SERIAL_WRITE_U16(packet->start_code);
	SERIAL_WRITE(packet->address[0]);
	SERIAL_WRITE(packet->address[1]);
	SERIAL_WRITE(packet->address[2]);
	SERIAL_WRITE(packet->address[3]);
	SERIAL_WRITE(packet->type);

	uint16_t wire_length = packet->length + 2;
	SERIAL_WRITE_U16(wire_length);

	uint16_t sum = ((wire_length)>>8) + ((wire_length)&0xFF) + packet->type;
	for (uint8_t i=0; i< packet->length; i++) {
		SERIAL_WRITE(packet->data[i]);
		sum += packet->data[i];
	}

	SERIAL_WRITE_U16(sum);
	return;
}

uint8_t Fingerprint_getStructuredPacket(Fingerprint_Packet * packet, uint32_t timeout) {
  uint8_t byte;
  uint32_t idx=0, timer=0;

  while(true) {
	  while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET) {
		  //DelayMs(1);
		  timer++;
		  if( timer >= timeout) {
#ifdef FINGERPRINT_DEBUG
			  printf("Timed out\r\n");
#endif
			  return FINGERPRINT_TIMEOUT;
		  }
	  }
	  byte = (uint8_t) USART_ReceiveData(USART1);
    switch (idx) {
      case 0:
        if (byte != (FINGERPRINT_STARTCODE >> 8))
        	continue;
        packet->start_code = (uint16_t)byte << 8;
        break;
      case 1:
        packet->start_code |= byte;
        if (packet->start_code != FINGERPRINT_STARTCODE)
        	return FINGERPRINT_BADPACKET;
        break;
      case 2:
      case 3:
      case 4:
      case 5:
        packet->address[idx-2] = byte;
        break;
      case 6:
    	  packet->type = byte;
		break;
      case 7:
    	  packet->length = (uint16_t)byte << 8;
    	  break;
      case 8:
    	  packet->length |= byte;
    	  break;
      default:
    	  packet->data[idx-9] = byte;
    	  if((idx-8) == packet->length)
    		  return FINGERPRINT_OK;
    	  break;
    }
    idx++;
  }
  // Shouldn't get here so...
  return FINGERPRINT_BADPACKET;
}

void Fingerprint_Get_Cmd_Packet(Fingerprint_Packet *packet, uint8_t type, uint16_t length, uint8_t * data)
{
	packet->start_code = FINGERPRINT_STARTCODE;
	packet->type = type;
	packet->length = length;
	packet->address[0] = 0xFF; packet->address[1] = 0xFF;
	packet->address[2] = 0xFF; packet->address[3] = 0xFF;
    if(length<64)
      memcpy(packet->data, data, length);
    else
      memcpy(packet->data, data, 64);
    return;
}

uint8_t Fingerprint_CheckPassword(void)
{
	GET_CMD_PACKET(FINGERPRINT_VERIFYPASSWORD,
				  (uint8_t)(thePassword >> 24), (uint8_t)(thePassword >> 16),
				  (uint8_t)(thePassword >> 8), (uint8_t)(thePassword & 0xFF));
	if (currPacket.data[0] == FINGERPRINT_OK)
		return FINGERPRINT_OK;
	else
		return FINGERPRINT_PACKETRECIEVEERR;
	return 0;
}

uint8_t Fingerprint_GetTemplateCount(void)
{
  GET_CMD_PACKET(FINGERPRINT_TEMPLATECOUNT);

  templateCount = currPacket.data[1];
  templateCount <<= 8;
  templateCount |= currPacket.data[2];

  return currPacket.data[0];
}

uint8_t Fingerprint_GetImage(void)
{
  SEND_CMD_PACKET(FINGERPRINT_GETIMAGE);
}

uint8_t Fingerprint_Image2Tz(uint8_t slot)
{
  SEND_CMD_PACKET(FINGERPRINT_IMAGE2TZ, slot);
}

uint8_t Fingerprint_CreateModel(void)
{
  SEND_CMD_PACKET(FINGERPRINT_REGMODEL);
}

uint8_t Fingerprint_StoreModel(uint16_t location)
{
  SEND_CMD_PACKET(FINGERPRINT_STORE, 0x01, (uint8_t)(location >> 8), (uint8_t)(location & 0xFF));
}

uint8_t Fingerprint_ReadSystemParameter(void)
{
	GET_CMD_PACKET(FINGERPRINT_READSYSPARA);
	statusRegister = currPacket.data[1];
	statusRegister <<= 8;
	statusRegister |= currPacket.data[2];

	sysIdentifier = currPacket.data[3];
	sysIdentifier <<= 8;
	sysIdentifier |= currPacket.data[4];

	librarySize = currPacket.data[5];
	librarySize <<= 8;
	librarySize |= currPacket.data[6];

	securityLevel = currPacket.data[7];
	securityLevel <<= 8;
	securityLevel |= currPacket.data[8];

	devAddress = currPacket.data[9];
	devAddress <<= 24;
	uint32_t tmp = currPacket.data[10];
	tmp <<= 16;
	devAddress |= tmp;
	tmp = currPacket.data[11];
	tmp <<= 8;
	devAddress |= tmp;
	devAddress |= currPacket.data[12];

	packetSize = currPacket.data[13];
	packetSize <<= 8;
	packetSize |= currPacket.data[14];

	baudSettings = currPacket.data[15];
	baudSettings <<= 8;
	baudSettings |= currPacket.data[16];

	return currPacket.data[0];
}

uint8_t Fingerprint_FingerFastSearch(void)
{
  // high speed search of slot #1 starting at page 0x0000 and page #0x00A3
  GET_CMD_PACKET(FINGERPRINT_HISPEEDSEARCH, 0x01, 0x00, 0x00, 0x00, 0xA3);
  fingerID = 0xFFFF;
  confidence = 0xFFFF;

  fingerID = currPacket.data[1];
  fingerID <<= 8;
  fingerID |= currPacket.data[2];

  confidence = currPacket.data[3];
  confidence <<= 8;
  confidence |= currPacket.data[4];

  return currPacket.data[0];
}

uint8_t Fingerprint_DeleteModel(uint16_t location)
{
  SEND_CMD_PACKET(FINGERPRINT_DELETE, (uint8_t)(location >> 8), (uint8_t)(location & 0xFF), 0x00, 0x01);
}

uint8_t Fingerprint_EmptyDatabase(void)
{
  SEND_CMD_PACKET(FINGERPRINT_EMPTY);
}




