/*
 * fingerprint.h
 *
 *  Created on: 09.05.2018
 *      Author: Michael Sauer
 *      Based on Adafruits Arduino Library
 */

#ifndef LIB_FINGERPRINT_H_
#define LIB_FINGERPRINT_H_

#include "stm32f10x.h"
#include "general.h"
#include "uart1.h"

#define FINGERPRINT_OK 0x00
#define FINGERPRINT_PACKETRECIEVEERR 0x01
#define FINGERPRINT_NOFINGER 0x02
#define FINGERPRINT_IMAGEFAIL 0x03
#define FINGERPRINT_IMAGEMESS 0x06
#define FINGERPRINT_FEATUREFAIL 0x07
#define FINGERPRINT_NOMATCH 0x08
#define FINGERPRINT_NOTFOUND 0x09
#define FINGERPRINT_ENROLLMISMATCH 0x0A
#define FINGERPRINT_BADLOCATION 0x0B
#define FINGERPRINT_DBRANGEFAIL 0x0C
#define FINGERPRINT_UPLOADFEATUREFAIL 0x0D
#define FINGERPRINT_PACKETRESPONSEFAIL 0x0E
#define FINGERPRINT_UPLOADFAIL 0x0F
#define FINGERPRINT_DELETEFAIL 0x10
#define FINGERPRINT_DBCLEARFAIL 0x11
#define FINGERPRINT_PASSFAIL 0x13
#define FINGERPRINT_INVALIDIMAGE 0x15
#define FINGERPRINT_FLASHERR 0x18
#define FINGERPRINT_INVALIDREG 0x1A
#define FINGERPRINT_ADDRCODE 0x20
#define FINGERPRINT_PASSVERIFY 0x21

#define FINGERPRINT_STARTCODE 0xEF01

#define FINGERPRINT_COMMANDPACKET 0x1
#define FINGERPRINT_DATAPACKET 0x2
#define FINGERPRINT_ACKPACKET 0x7
#define FINGERPRINT_ENDDATAPACKET 0x8

#define FINGERPRINT_TIMEOUT 0xFF
#define FINGERPRINT_BADPACKET 0xFE

#define FINGERPRINT_GETIMAGE 0x01
#define FINGERPRINT_IMAGE2TZ 0x02
#define FINGERPRINT_REGMODEL 0x05
#define FINGERPRINT_STORE 0x06
#define FINGERPRINT_LOAD 0x07
#define FINGERPRINT_UPLOAD 0x08
#define FINGERPRINT_DELETE 0x0C
#define FINGERPRINT_EMPTY 0x0D
#define FINGERPRINT_READSYSPARA 0x0F
#define FINGERPRINT_SETPASSWORD 0x12
#define FINGERPRINT_VERIFYPASSWORD 0x13
#define FINGERPRINT_HISPEEDSEARCH 0x1B
#define FINGERPRINT_TEMPLATECOUNT 0x1D

#define FINGERPRINT_DEBUG

#define DEFAULTTIMEOUT 900000  ///< UART reading timeout in milliseconds

typedef struct {
  uint16_t start_code;      ///< "Wakeup" code for packet detection
  uint8_t address[4];       ///< 32-bit Fingerprint sensor address
  uint8_t type;             ///< Type of packet
  uint16_t length;          ///< Length of packet
  uint8_t data[64];         ///< The raw buffer for packet payload
} Fingerprint_Packet;

/// The matching location that is set by fingerFastSearch()
uint16_t fingerID;
/// The confidence of the fingerFastSearch() match, higher numbers are more confidents
uint16_t confidence;
/// The number of stored templates in the sensor, set by getTemplateCount()
uint16_t templateCount;

uint16_t statusRegister;
uint16_t sysIdentifier;
uint16_t librarySize;
uint16_t securityLevel;
uint32_t devAddress;
uint16_t packetSize;
uint16_t baudSettings;

void Fingerprint_Init(uint32_t password);
uint8_t Fingerprint_GetTemplateCount(void);
uint8_t Fingerprint_CheckPassword(void);
uint8_t Fingerprint_GetImage(void);
uint8_t Fingerprint_Image2Tz(uint8_t slot);
uint8_t Fingerprint_CreateModel(void);
uint8_t Fingerprint_StoreModel(uint16_t location);
uint8_t Fingerprint_ReadSystemParameter(void);
uint8_t Fingerprint_FingerFastSearch(void);
uint8_t Fingerprint_DeleteModel(uint16_t location);
uint8_t Fingerprint_EmptyDatabase(void);

#endif /* LIB_FINGERPRINT_H_ */
