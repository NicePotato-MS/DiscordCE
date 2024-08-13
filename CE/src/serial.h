#pragma once

#include <stdint.h>

#include <srldrvce.h>

#define PACKET_DEBUG_PRINT 0
#define PACKET_DEBUG_PRINT_UINT64_T 1
#define PACKET_DEBUG_PRINT_INT64_T 2
#define PACKET_EXCEPTION_OCCURED 3

#define PACKET_DEBUG_PFP_TEST 128

extern srl_device_t serial_Device;
extern bool serial_has_device;
extern bool serial_connected;
extern uint8_t serial_Buffer[1024];

extern void serial_Setup();
void serial_SendPacketHeaderUnsafe(unsigned int size, unsigned int packet_type);
void serial_DebugPrintUnsafe(const char* string);
void serial_DebugPrintUnsignedUnsafe(uint64_t value);
void serial_DebugPrintSignedUnsafe(int64_t value);

void serial_SendPacketHeader(unsigned int size, unsigned int packet_type);
void serial_DebugPrint(const char* string);
void serial_DebugPrintUnsigned(uint64_t value);
void serial_DebugPrintSigned(int64_t value);

void serial_ReceivePacket();