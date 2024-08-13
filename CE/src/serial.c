#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include <srldrvce.h>
#include <fontlibc.h>
#include <sys/timers.h>

#include "font.h"
#include "serial.h"
#include "exception.h"

srl_device_t serial_Device;
bool serial_has_device = false;
bool serial_connected = false;
uint8_t serial_Buffer[1024];

static usb_error_t handle_usb_event(usb_event_t event, void *event_data, usb_callback_data_t *callback_data __attribute__((unused))) {
    usb_error_t err;
    
    if ((err = srl_UsbEventCallback(event, event_data, callback_data)) != USB_SUCCESS) {
        return err;
    }

    if(event == USB_DEVICE_CONNECTED_EVENT && !(usb_GetRole() & USB_ROLE_DEVICE)) {
        usb_device_t device = event_data;
        usb_ResetDevice(device);
    }

    if(event == USB_HOST_CONFIGURE_EVENT || (event == USB_DEVICE_ENABLED_EVENT && !(usb_GetRole() & USB_ROLE_DEVICE))) {
        if(serial_has_device) { return USB_SUCCESS; }

        usb_device_t device;
        if(event == USB_HOST_CONFIGURE_EVENT) {
            device = usb_FindDevice(NULL, NULL, USB_SKIP_HUBS);
            if(device == NULL) return USB_SUCCESS;
        } else {
            device = event_data;
        }

        srl_error_t error = srl_Open(&serial_Device, device, serial_Buffer, sizeof serial_Buffer, SRL_INTERFACE_ANY, 115200);
        if(error) {
            exception_LogInfoNamedInt(0, "Serial Error", error);
            exception_Crash(CONDITION_UNHANDLED_SERIAL_FAILURE);
            return USB_SUCCESS;
        }

        serial_has_device = true;
    }

    if(event == USB_DEVICE_DISCONNECTED_EVENT) {
        usb_device_t device = event_data;
        if(device == serial_Device.dev) {
            srl_Close(&serial_Device);
            serial_has_device = false;
            serial_connected = false;
        }
    }

    if(event == USB_DEVICE_SUSPENDED_EVENT) { serial_has_device = false; }

    return USB_SUCCESS;
}

inline void serial_Setup() {
    const usb_standard_descriptors_t *desc = srl_GetCDCStandardDescriptors();
    usb_error_t usb_error = usb_Init(handle_usb_event, NULL, desc, USB_DEFAULT_INIT_FLAGS);
    if(usb_error) {
        exception_LogInfoNamedInt(0, "USB Error", usb_error);
       exception_Crash(CONDITION_UNHANDLED_SERIAL_FAILURE);
    }
}

void serial_SendPacketHeaderUnsafe(unsigned int size, unsigned int packet_type) {
    srl_Write(&serial_Device, "DC", 2);
    srl_Write(&serial_Device, &size, 3);
    srl_Write(&serial_Device, &packet_type, 3);
}

void serial_DebugPrintUnsafe(const char *str) {
    unsigned int length = strlen(str);
    serial_SendPacketHeaderUnsafe(length, PACKET_DEBUG_PRINT);
    srl_Write(&serial_Device, str, length);
}

void serial_DebugPrintUnsignedUnsafe(uint64_t value) {
    serial_SendPacketHeaderUnsafe(8, PACKET_DEBUG_PRINT_UINT64_T);
    srl_Write(&serial_Device, &value, 8);
}

void serial_DebugPrintSignedUnsafe(int64_t value) {
    serial_SendPacketHeaderUnsafe(8, PACKET_DEBUG_PRINT_INT64_T);
    srl_Write(&serial_Device, &value, 8);
}

void serial_DebugPrintNamedUnsignedUnsafe(const char *str, uint64_t value) {
    unsigned int length = strlen(str);
    serial_SendPacketHeaderUnsafe(8 + length, PACKET_DEBUG_PRINT_NAMED_UINT64);
    srl_Write(&serial_Device, &value, 8);
    srl_Write(&serial_Device, str, length);
}

void serial_DebugPrintNamedSignedUnsafe(const char *str, int64_t value) {
    unsigned int length = strlen(str);
    serial_SendPacketHeaderUnsafe(8 + length, PACKET_DEBUG_PRINT_NAMED_INT64);
    srl_Write(&serial_Device, &value, 8);
    srl_Write(&serial_Device, str, length);
}

void serial_DebugPrintBytesUnsafe(void *data, unsigned int size) {
    serial_SendPacketHeaderUnsafe(size, PACKET_DEBUG_PRINT_BYTES);
    srl_Write(&serial_Device, data, size);
}


void serial_SendPacketHeader(unsigned int size, unsigned int packet_type) {
    if (serial_has_device) { serial_SendPacketHeaderUnsafe(size, packet_type); }
}

void serial_DebugPrint(const char *str) {
    if (serial_has_device) { serial_DebugPrintUnsafe(str); }
}

void serial_DebugPrintUnsigned(uint64_t value) {
    if (serial_has_device) { serial_DebugPrintUnsignedUnsafe(value); }
}

void serial_DebugPrintSigned(int64_t value) {
    if (serial_has_device) { serial_DebugPrintSignedUnsafe(value); }
}

void serial_DebugPrintNamedUnsigned(const char *str, uint64_t value) {
    if (serial_has_device) { serial_DebugPrintNamedUnsignedUnsafe(str, value); }
}

void serial_DebugPrintNamedSigned(const char *str, int64_t value) {
    if (serial_has_device) { serial_DebugPrintNamedSignedUnsafe(str, value); }
}

void serial_DebugPrintBytes(void *data, unsigned int size) {
    if (serial_has_device) { serial_DebugPrintBytesUnsafe(data, size); }
}

uint8_t packet[1024];
unsigned int packet_index = 0;
unsigned int data_size;
unsigned int packet_type;

void serial_ReceivePacket() {
    if (!serial_has_device) { return; }

    if (packet_index < 8) {
        int bytes_read = srl_Read(&serial_Device, &packet[packet_index], 8 - packet_index);
        packet_index += bytes_read;
        if (packet_index < 8) { return; } else {
            data_size = *((unsigned int *)(&packet[2]));
            packet_type = *((unsigned int *)(&packet[sizeof(int) + 2]));
        }
    }
    
    if (packet[0] != 'D' || packet[1] != 'C') {
        exception_LogInfoString(0, "Packet signature incorrect");
        serial_DebugPrint("Header");
        serial_DebugPrintBytes(&packet, 8);
        exception_Crash(CONDITION_CORRUPTED_PACKET);
    }

    if (data_size > 1016) {
        exception_LogInfoString(0, "Packet too large");
        exception_LogInfoNamedInt(1, "Bytes", data_size);
        exception_Crash(CONDITION_CORRUPTED_PACKET);
    }

    int bytes_read = srl_Read(&serial_Device, &packet[packet_index], data_size - packet_index - 8);
    packet_index += bytes_read;
    if (packet_index - 8 < data_size) { return; }

    serial_DebugPrint("Header");
    serial_DebugPrintBytes(&packet, 8);
    serial_DebugPrint("Data");
    serial_DebugPrintBytes(&packet[8], data_size);

    packet_index = 0;
}