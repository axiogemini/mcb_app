/*
 * CmdHandler.c
 *
 *  Created on: 2017?1?10?
 *      Author: zctiaxia
 */

#include "include.h"

void *
MemCpy(void *DestAddr, void const *SrcAddr, int16 Len)
{
    uint8_t *dest = DestAddr;
    uint8_t const *src = SrcAddr;
    int16 len = Len;
    while (len-- > 0) {
        *dest++ = *src++;
    }
    return dest;
}

void
MakeupAnswer(uint8_t *answer, uint8_t const *cmd)
{
    uint8_t temp = GetByte(cmd, USBIDX_TARGET);
    memcpy(answer, cmd, GetByte(cmd, USBIDX_LEN) + 9);
    WriteByte(answer, USBIDX_TARGET, GetByte(cmd, USBIDX_SRC));
    WriteByte(answer, USBIDX_SRC, temp);
    WriteByte(answer, USBIDX_CMDCL, GetByte(cmd, USBIDX_CMDCL) & 0x0f);
}

void
DLD_Handler(uint8_t *pFrame)
{
    UsbTxTask usbtxobj;
    uint8_t buffer[32];
    usbtxobj.pBuf = buffer;
    switch (GetByte(pFrame, USBIDX_SUBID)) {
    case 0x00:  // get download state
        MakeupAnswer(buffer, pFrame);
        usbtxobj.Len = ((WriteByte(buffer, USBIDX_LEN, 4))) + 9;
        WriteByte(buffer, 10, 0x00);
        WriteShort(buffer, 11, 0x1003);
        break;
    case 0x01:  // force download
        Usb_Updater();
        break;
    case 0x02:  // get download type
        MakeupAnswer(buffer, pFrame);
        usbtxobj.Len = ((WriteByte(buffer, USBIDX_LEN, 5))) + 9;
        WriteShort(buffer, 10, 0x07);
        WriteShort(buffer, 12, 0x1003);
        break;
    case 0x03:  // set app state
        MakeupAnswer(buffer, pFrame);
        usbtxobj.Len = ((WriteByte(buffer, USBIDX_LEN, 5))) + 9;
        WriteShort(buffer, 10, 0x00);
        WriteShort(buffer, 12, 0x1003);
        //todo set app_active flage
        break;
    case 0x05:  // get num of memory
        MakeupAnswer(buffer, pFrame);
        usbtxobj.Len = ((WriteByte(buffer, USBIDX_LEN, 3))) + 9;
        WriteShort(buffer, 9, 0x06);
        WriteShort(buffer, 11, 0x1003);
        break;
    case 0x06: { // get mem range
        uint32_t startaddress = 0x3d8000 + GetByte(pFrame, USBIDX_BIOS_PAYLOAD) * 0x4000;
        MakeupAnswer(buffer, pFrame);
        usbtxobj.Len = ((WriteByte(buffer, USBIDX_LEN, 11))) + 9;
        WriteLong(buffer, 10, startaddress);
        WriteLong(buffer, 14, 0x4000);
        WriteShort(buffer, 18, 0x1003);
        break;
    }case 0x10:  // get mem type identifier
        MakeupAnswer(buffer, pFrame);
        usbtxobj.Len = ((WriteByte(buffer, USBIDX_LEN, 11))) + 9;
        WriteLong(buffer, 10, 0x00);
        WriteLong(buffer, 14, 0x00);
        WriteShort(buffer, 18, 0x1003);
        break;
    default : break;
    }
    StartUsbTxTask(&usbtxobj);
}
