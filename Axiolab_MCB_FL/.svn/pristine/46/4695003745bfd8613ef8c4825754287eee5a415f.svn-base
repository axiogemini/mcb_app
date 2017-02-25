/*
 * SPI.c
 *
 *  Created on: 2015Äê4ÔÂ26ÈÕ
 *      Author: xiatian
 */

#include "include.h"

void
SPI_Init()
{
    SpiaRegs.SPICCR.bit.SPISWRESET = 0;          // Reset On
    SpiaRegs.SPICCR.bit.SPICHAR = 0x0F;          // Rising edge, 8-bit char bits
    SpiaRegs.SPICCR.bit.CLKPOLARITY = 0;
    SpiaRegs.SPICTL.bit.CLK_PHASE = 0;           // Enable master mode, normal phase,
    SpiaRegs.SPICTL.bit.MASTER_SLAVE = 1;        // Master Mode
    SpiaRegs.SPICTL.bit.TALK = 1;                // Enable talk
    SpiaRegs.SPICTL.bit.SPIINTENA = 0;           // Disable SPI int
    SpiaRegs.SPIBRR = 0x002c;                    // 22.5Mhz CLK divided by 4 = 5.625Mhz  now 500k
    SpiaRegs.SPIPRI.bit.SOFT = 0;
    SpiaRegs.SPIPRI.bit.FREE = 0;                // Set so breakpoints don't disturb xmission
    SpiaRegs.SPICCR.bit.SPISWRESET = 1;          // Release SPI module

    SpibRegs.SPICCR.bit.SPISWRESET = 0;          // Reset On
    SpibRegs.SPICCR.bit.SPICHAR = 0x0F;          // Rising edge, 8-bit char bits
    SpibRegs.SPICCR.bit.CLKPOLARITY = 1;
    SpibRegs.SPICTL.bit.CLK_PHASE = 1;           // Enable master mode, normal phase,
    SpibRegs.SPICTL.bit.MASTER_SLAVE = 1;        // Master Mode
    SpibRegs.SPICTL.bit.TALK = 1;                // Enable talk
    SpibRegs.SPICTL.bit.SPIINTENA = 0;           // Disable SPI int
    SpibRegs.SPIBRR = 0x002c;                    // 22.5Mhz CLK divided by 5 = 4.5Mhz
    SpibRegs.SPIPRI.bit.SOFT = 0;
    SpibRegs.SPIPRI.bit.FREE = 0;                // Set so breakpoints don't disturb xmission
    SpibRegs.SPICCR.bit.SPISWRESET = 1;          // Release SPI module
}

void
SPIA_Write(Uint16 TxData)
{
    SpiaRegs.SPIDAT = TxData;
}

void
SPIA_ReadWrite(Uint16 TxData, Uint16 TxBufData)
{
    SpiaRegs.SPIDAT = TxData;
    SpiaRegs.SPITXBUF = TxBufData;
}

void
SPIB_Write(Uint16 TxData)
{
    SpibRegs.SPIDAT = TxData;
}

