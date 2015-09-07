/*
 * cc1101_defs.h
 *
 *  Created on: Sep 6, 2015
 *      Author: sean
 */

#ifndef CC1101_DEFS_H_
#define CC1101_DEFS_H_

// CC1101 configuration register addresses.
// (read/write; burst access possible)
enum {
    CC1101_REG_IOCFG2         = 0x00,      // GDO2 Output Pin Configuration
    CC1101_REG_IOCFG1         = 0x01,      // GDO1 Output Pin Configuration
    CC1101_REG_IOCFG0         = 0x02,      // GDO0 Output Pin Configuration
    CC1101_REG_FIFOTHR        = 0x03,      // RX FIFO and TX FIFO Thresholds
    CC1101_REG_SYNC1          = 0x04,      // Sync Word, High Byte
    CC1101_REG_SYNC0          = 0x05,      // Sync Word, Low Byte
    CC1101_REG_PKTLEN         = 0x06,      // Packet Length
    CC1101_REG_PKTCTRL1       = 0x07,      // Packet Automation Control
    CC1101_REG_PKTCTRL0       = 0x08,      // Packet Automation Control
    CC1101_REG_ADDR           = 0x09,      // Device Address
    CC1101_REG_CHANNR         = 0x0A,      // Channel Number
    CC1101_REG_FSCTRL1        = 0x0B,      // Frequency Synthesizer Control
    CC1101_REG_FSCTRL0        = 0x0C,      // Frequency Synthesizer Control
    CC1101_REG_FREQ2          = 0x0D,      // Frequency Control Word, High Byte
    CC1101_REG_FREQ1          = 0x0E,      // Frequency Control Word, Middle Byte
    CC1101_REG_FREQ0          = 0x0F,      // Frequency Control Word, Low Byte
    CC1101_REG_MDMCFG4        = 0x10,      // Modem Configuration
    CC1101_REG_MDMCFG3        = 0x11,      // Modem Configuration
    CC1101_REG_MDMCFG2        = 0x12,      // Modem Configuration
    CC1101_REG_MDMCFG1        = 0x13,      // Modem Configuration
    CC1101_REG_MDMCFG0        = 0x14,      // Modem Configuration
    CC1101_REG_DEVIATN        = 0x15,      // Modem Deviation Setting
    CC1101_REG_MCSM2          = 0x16,      // Main Radio Control State Machine Configuration
    CC1101_REG_MCSM1          = 0x17,      // Main Radio Control State Machine Configuration
    CC1101_REG_MCSM0          = 0x18,      // Main Radio Control State Machine Configuration
    CC1101_REG_FOCCFG         = 0x19,      // Frequency Offset Compensation Configuration
    CC1101_REG_BSCFG          = 0x1A,      // Bit Synchronization Configuration
    CC1101_REG_AGCCTRL2       = 0x1B,      // AGC Control
    CC1101_REG_AGCCTRL1       = 0x1C,      // AGC Control
    CC1101_REG_AGCCTRL0       = 0x1D,      // AGC Control
    CC1101_REG_WOREVT1        = 0x1E,      // High Byte Event0 Timeout
    CC1101_REG_WOREVT0        = 0x1F,      // Low Byte Event0 Timeout
    CC1101_REG_WORCTRL        = 0x20,      // Wake On Radio Control
    CC1101_REG_FREND1         = 0x21,      // Front End RX Configuration
    CC1101_REG_FREND0         = 0x22,      // Front End TX Configuration
    CC1101_REG_FSCAL3         = 0x23,      // Frequency Synthesizer Calibration
    CC1101_REG_FSCAL2         = 0x24,      // Frequency Synthesizer Calibration
    CC1101_REG_FSCAL1         = 0x25,      // Frequency Synthesizer Calibration
    CC1101_REG_FSCAL0         = 0x26,      // Frequency Synthesizer Calibration
    CC1101_REG_RCCTRL1        = 0x27,      // RC Oscillator Configuration
    CC1101_REG_RCCTRL0        = 0x28,      // RC Oscillator Configuration
    CC1101_REG_FSTEST         = 0x29,      // Frequency Synthesizer Calibration Control
    CC1101_REG_PTEST          = 0x2A,      // Production Test
    CC1101_REG_AGCTEST        = 0x2B,      // AGC Test
    CC1101_REG_TEST2          = 0x2C,      // Various Test Settings
    CC1101_REG_TEST1          = 0x2D,      // Various Test Settings
    CC1101_REG_TEST0          = 0x2E,      // Various Test Settings

    // Status registers (read-only)
    // Since these are aliased to command strobes, must set burst bit
    // to access them (but this does not do a burst access; it just
    // disambiguates).
    CC1101_REG_PARTNUM        = 0x30,
    CC1101_REG_VERSION        = 0x31,
    CC1101_REG_FREQEST        = 0x32,
    CC1101_REG_LQI            = 0x33,
    CC1101_REG_RSSI           = 0x34,
    CC1101_REG_MARCSTATE      = 0x35,
    CC1101_REG_WORTIME1       = 0x36,
    CC1101_REG_WORTIME0       = 0x37,
    CC1101_REG_PKTSTATUS      = 0x38,
    CC1101_REG_VCO_VC_DAC     = 0x39,
    CC1101_REG_TXBYTES        = 0x3A,
    CC1101_REG_RXBYTES        = 0x3B,
    CC1101_REG_RCCTRL1_STATUS = 0x3C,
    CC1101_REG_RCCTRL0_STATUS = 0x3D,

    // Multi-byte registers.
    CC1101_REG_PATABLE        = 0x3E,
    CC1101_REG_TXFIFO         = 0x3F,
    CC1101_REG_RXFIFO         = 0x3F       // Disambiguated from TXFIFO depending on R/W.
};

// CC1101 command strobes.
enum {
    CC1101_STROBE_SRES    = 0x30,
    CC1101_STROBE_SFSTXON = 0x31,
    CC1101_STROBE_SXOFF   = 0x32,
    CC1101_STROBE_SCAL    = 0x33,
    CC1101_STROBE_SRX     = 0x34,
    CC1101_STROBE_STX     = 0x35,
    CC1101_STROBE_SIDLE   = 0x36,
    CC1101_STROBE_SAFC    = 0x37,
    CC1101_STROBE_SWOR    = 0x38,
    CC1101_STROBE_SPWD    = 0x39,
    CC1101_STROBE_SFRX    = 0x3A,
    CC1101_STROBE_SFTX    = 0x3B,
    CC1101_STROBE_SWORRST = 0x3C,
    CC1101_STROBE_SNOP    = 0x3D
};

#endif /* CC1101_DEFS_H_ */
