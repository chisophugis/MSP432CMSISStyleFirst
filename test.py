from __future__ import print_function
import serial
import time

def main():
    ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=1)
    test_d9e7c978(ser)
    print('ALL TESTS PASSED')
    repeatedly_send(ser)

def test_d9e7c978(ser):
    # Derived from commit message of d9e7c978

    # Send the SNOP (0x3D) command strobe to get the status byte.
    # 0x3D byte for "write" access, 0xBD for "read" access (this
    # affects whether the CC1101 chip status byte gives info about the
    # TX or RX FIFO, which leads to different results assuming the
    # chip is in a quiescent state).

    time.sleep(.1)
    ser.write('\x3D')
    b = ser.read()
    print('byte = {:02X}'.format(ord(b)))
    # Chip status byte = IDLE state, 15+ bytes available in TX FIFO
    assert ord(b) == 0x0F

    time.sleep(.1)
    ser.write('\xBD')
    b = ser.read()
    print('byte = {:02X}'.format(ord(b)))
    # Chip status byte = IDLE state, 0 bytes available in RX FIFO
    assert ord(b) == 0x00

def repeatedly_send(ser):
    i = 0
    while True:
        ser.write('\x3D')
        b = ser.read()
        print(i, '(byte = {:02X})'.format(ord(b)))
        time.sleep(.3)
        i += 1

main()
