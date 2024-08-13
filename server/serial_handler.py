import serial
import threading
import asyncio
import struct
from PIL import Image
from images import encode_image

SERIAL_PORT = '/dev/ttyACM0'
BAUD_RATE = 115200

serial_lock = threading.Lock()
serial_port = None

    

async def init_serial():
    global serial_port
    print("Serial device is asleep.")
    while not serial_port:
        try:
            serial_port = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1, stopbits=serial.STOPBITS_TWO)
            print("Serial device connected.")
        except serial.SerialException as e:
            # print(e)
            await asyncio.sleep(0.25)

def int_to_3_bytes(value):
    if not (0 <= value < 2**24):
        raise ValueError("Value must be in the range 0 to 16777215 (0xFFFFFF).")
    return struct.pack('<I', value)[:3]

def send_packet(packet_type, data):
    serial_port.write(b'DC')
    serial_port.write(int_to_3_bytes(len(data)))
    serial_port.write(int_to_3_bytes(packet_type))
    # serial_port.write(data)

def read_int(size):
    return int.from_bytes(serial_port.read(size), "little")

async def read_serial():
    global serial_port

    if serial_port.in_waiting < 8:
        return False
    
    magic = serial_port.read(2)
    if magic != b'DC':
        print("Recieved Corrupted Packet!")
        serial_port.close()
        serial_port = None
        return False
    data_size = read_int(3)
    packet_type = read_int(3)

    while serial_port.in_waiting < data_size:
        await asyncio.sleep(0.001)

    data = serial_port.read(data_size)

    match packet_type:
        case 0: # Debug print
            print(f"C: {data.decode('ascii')}")
        case 1: # Debug print uint64_t
            print(f"C: {struct.unpack('<Q', data)[0]}")
        case 2: # Debug print int64_t
            print(f"C: {struct.unpack('<q', data)[0]}")
        case 3: # Device Exception
            print("**DEVICE EXCEPTION**")
        
        case 128: # Debug send pfp
            input_image_path = 'test_pfp.png'
            image = Image.open(input_image_path)
            image = image.convert('RGB')

            encoded = encode_image(image, (24,24))
            print("Sending pfp")

            send_packet(0, encoded)
            print("done")

        case _:
            print("Unknown Packet type recieved!")
            serial_port.close()
            serial_port = None
            return




async def serial_reader():
    global serial_port
    global poll_cache
    while True:
        with serial_lock:
            if serial_port:
                try:
                    await read_serial()
                except serial.SerialException:
                    print("Serial communication error. Reconnecting...")
                    if serial_port:
                        serial_port.close()
                        serial_port = None
                    await init_serial()
                except OSError:
                    print("Serial device disconnected.")
                    if serial_port:
                        serial_port.close()
                        serial_port = None
                    await init_serial()
            else:
                await init_serial()
        await asyncio.sleep(0.001)