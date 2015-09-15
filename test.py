from __future__ import print_function
import serial
import time

def main():
    ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=1)
    test_d9e7c978(ser)
    print('ALL TESTS PASSED')
    scan_pll_lock_frequencies(ser)
    #repeatedly_send(ser)

def test_d9e7c978(ser):
    # Derived from commit message of d9e7c978

    # The 'r' is for the "relay to cc1101" command.

    # Send the SNOP (0x3D) command strobe to get the status byte.
    # 0x3D byte for "write" access, 0xBD for "read" access (this
    # affects whether the CC1101 chip status byte gives info about the
    # TX or RX FIFO, which leads to different results assuming the
    # chip is in a quiescent state).

    time.sleep(.1)
    ser.write('r\x3D')
    b = ser.read()
    print('byte = {:02X}'.format(ord(b)))
    # Chip status byte = IDLE state, 15+ bytes available in TX FIFO
    assert ord(b) == 0x0F

    time.sleep(.1)
    ser.write('r\xBD')
    b = ser.read()
    print('byte = {:02X}'.format(ord(b)))
    # Chip status byte = IDLE state, 0 bytes available in RX FIFO
    assert ord(b) == 0x00

def calculate_freqregs_settings(f_carrier):
    f_carrier *= 1000. * 1000. # convert to MHz

    f_xosc = 26. * 1000. * 1000. # 26 MHz crystal

    # f_carrier = (f_xosc / 2^16) * FREQ[23:0]
    freq = int((f_carrier / f_xosc) * 2.**16)
    freqregs = [(freq & 0xFF0000) >> 16, (freq & 0xFF00) >> 8, freq & 0xFF]
    return ''.join(chr(x) for x in freqregs)

def scan_pll_lock_frequencies(ser):
    # 'f' is "set frequency" command.

    # Documentation for FREQ[23:22] in the FREQ2 register says:
    # "the FREQ2 register is less than 36 with 26-27 MHz crystal"

    for freq in range(10, 1000, 10): # MHz
        freqstr = calculate_freqregs_settings(freq)
        msg = 'f' + freqstr
        ser.write(msg)
        b = ser.read(1)
        print(freq, '{:02X}'.format(ord(b)))
        time.sleep(.1)

def repeatedly_send(ser):
    # 'p' is "packet" command.
    i = 0
    while True:
        ser.write('p')
        b = ser.read()
        print(i, '(byte = {:02X})'.format(ord(b)))
        time.sleep(.3)
        i += 1

main()
