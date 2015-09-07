/*
 * cc1101_defs.h
 *
 *  Created on: Sep 6, 2015
 *      Author: sean
 */

#ifndef CC1101_DEFS_H_
#define CC1101_DEFS_H_

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
