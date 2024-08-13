# from client import start_client
from serial_handler import serial_reader
import asyncio

async def main():
    await asyncio.gather(
        # asyncio.create_task(start_client())
        asyncio.create_task(serial_reader())
    )

if __name__ == "__main__":
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        print("Shutting down...")
        exit()

