/*
 * MainControl.c
 *
 *  Created on: 2016��7��1��
 *      Author: ZCTIAXIA
 */

#include "include.h"

// each time modify the firmware please update the AppVersion
char AppVersion[] = "00.013";
char AppDate[] = "02/23/2017";
char AppName_Axiolab[] = "GEM_MainControlLab";
char AppName_Axioscope[] = "GEM_MainControlScope";
char AppName_Key[] = "GEM_Keyboard";
char AppName_Lightctrl[] = "GEM_Lightcontrol";
char SerialNumber[] = "13818504432";
char ChangerScaling[] = "increment";
char ServoScaling[] = "increment";
char AxisScaling[] = "nm";
Device g_Device, g_DeviceShadow;
uint32_t g_ulSysTickCount = 0;
uint8_t ParfocalTick = 0;
Uint16 RLMatrix[REFL_NUM][NP_NUM] = {0};
Uint16 TLMatrix[NP_NUM] = {0};
float np_mag[NP_NUM] = {0};  // magnification value

static Bool SaveBlink = false;

bool ReadSnapButton(void)
{
	if (g_Device.MotorZ.isValid)
//		return GpioDataRegs.GPADAT.bit.GPIO23;         // right SNAP key
		return (GpioDataRegs.GPBDAT.bit.GPIO54 || GpioDataRegs.GPADAT.bit.GPIO23);		// temp fix before 2-wheel arrive
	else

		return (GpioDataRegs.GPBDAT.bit.GPIO54 || GpioDataRegs.GPADAT.bit.GPIO23);
}

bool ReadMultiButton(void)
{
	if (g_Device.MotorZ.isValid)
		return GpioDataRegs.GPBDAT.bit.GPIO54;         // left SNAP key
	else
		return 0;
}



void
AD5640WriteData(Uint16 Value)
{
    if (Value > 0x3fff) return;
    SPIB_Write(Value);
}

void
AD5293WriteData(Uint16 Value) // 10 bit resolution
{
    if (Value > 1023) return;
    SPIA_Write(Value | 0x400);
}

void
BulbVoltageWrite(float BulbOut)
{
    if (BulbOut > 13.25) return;

    Uint16 data = 1024 - (BulbOut * 7.87 / 0.97 - 7.87) / 100 * 1023;
    AD5293WriteData(data);
}

void
BulbIntensityWrite(Uint16 Value)
{
    Uint16 data = 1023 - Value;
    AD5293WriteData(data);
}


// the AD5640 is a 14-bit DAC
void
LEDIntensityWrite(Uint16 intensity)
{
    if (intensity <= 64) {
        AD5640WriteData(intensity);
    }else if (intensity <= 1023) {
        AD5640WriteData( (16384 - 64) / (1024 - 64) * (intensity - 64) + 64 );
    }else
        AD5640WriteData(0);
}

void
HALIntensityWrite(Uint16 intensity)
{
    AD5640WriteData(intensity << 4);
}

void
Read_RLTL_ID(void)
{
	uint8_t code;
	if (g_I2cMsg.Status != I2C_MSGSTAT_INACTIVE)
		return;

	g_I2cMsg.Status = I2C_MSGSTAT_SEND_NOSTOP;
	g_I2cMsg.DevAddr = 0x22;
	g_I2cMsg.Len = 1;
	g_I2cMsg.RegAddr.all = 0x02;
	g_I2cMsg.LongRegAddrFlag = false;
	do {
		I2CA_Server();
	}while (g_I2cMsg.Status != I2C_MSGSTAT_INACTIVE);
	code = I2cBuffer[0];
	I2cMsg_Reset();

	if (~code & 0x02) {
		g_Device.ULight.Type = LightType_LED;
	}else {
		if (g_Device.Serie == Axiolab && g_Device.VarType != VAR_Bio)
			g_Device.ULight.Type = LightType_BULB;
		else
			g_Device.ULight.Type = NoLightSource;
	}

	if (~code & 0x01) {
		g_Device.LLight.Type = LightType_LED;
	}else {
		if (g_Device.Serie == Axiolab)
			g_Device.LLight.Type = LightType_BULB;
		else
			g_Device.LLight.Type = NoLightSource;
	}
}

void
Read_Refl_Encoder(void)
{
   uint8_t code;
   if (g_I2cMsg.Status != I2C_MSGSTAT_INACTIVE)
      return;

   g_I2cMsg.Status = I2C_MSGSTAT_SEND_NOSTOP;
   g_I2cMsg.DevAddr = 0x22;
   g_I2cMsg.Len = 1;
   g_I2cMsg.RegAddr.all = 0x01;
   g_I2cMsg.LongRegAddrFlag = false;
   do {
      I2CA_Server();
   }while (g_I2cMsg.Status != I2C_MSGSTAT_INACTIVE);
   code = I2cBuffer[0];
   I2cMsg_Reset();

   g_Device.Reflector.Encoder = ~code & 0x0F;
}

void
Read_NP_Encoder(void)
{
    uint8_t code, d0, d1, d2;
    d0 = GpioDataRegs.GPADAT.bit.GPIO0;
    d1 = GpioDataRegs.GPADAT.bit.GPIO3;
    d2 = GpioDataRegs.GPBDAT.bit.GPIO57;
    code = d2 << 2 | d1 << 1 | d0;
    g_Device.NosePiece.Encoder = code;
}

void
Read_NPRefl_Position(void)
{
	// for nose piece
	if (g_Device.NosePiece.Encoder == g_DeviceShadow.NosePiece.Encoder) {
		if (g_Device.Serie == Axioscope) {
			switch (g_Device.NosePiece.Encoder) {
            case 0x01:
                g_Device.NosePiece.Position = Position1;
                break;
				
            case 0x02:
                g_Device.NosePiece.Position = Position2;
                break;
				
            case 0x03:
                g_Device.NosePiece.Position = Position3;
                g_Device.NosePiece.status |= (1 << CHG_BIT_VALID_POS);
				g_Device.NosePiece.status &= ~(1 << CHG_BIT_MOVING);
                break;
            case 0x04:
                g_Device.NosePiece.Position = Position4;
                break;
				
            case 0x05:
                g_Device.NosePiece.Position = Position5;
                break;
				
            case 0x06:
                g_Device.NosePiece.Position = Position6;
                break;
				
            default :
                g_Device.NosePiece.Position = Position0;
                //g_Device.NosePiece.Settled = false;
                break;
			}
		}else {			//Axiolab
			switch (g_Device.NosePiece.Encoder) {
#if 1
            case 0x04:
                g_Device.NosePiece.Position = Position1;
                break;

            case 0x05:
                g_Device.NosePiece.Position = Position2;
                break;

            case 0x01:
                g_Device.NosePiece.Position = Position3;
                break;

 //           case 0x03:
 			case 0x06:	// modification according to mechanical change 
                g_Device.NosePiece.Position = Position4;
                break;

            case 0x02:
                g_Device.NosePiece.Position = Position5;
                break;
#else

		   case 0x03:
			   g_Device.NosePiece.Position = Position1;
			   break;

		   case 0x04:
			   g_Device.NosePiece.Position = Position2;
			   break;

		   case 0x05:
			   g_Device.NosePiece.Position = Position3;
			   break;

//			 case 0x03:
		   case 0x06:  // modification according to mechanical change 
			   g_Device.NosePiece.Position = Position4;
			   break;

		   case 0x02:
			   g_Device.NosePiece.Position = Position5;
			   break;
#endif


            default :
                g_Device.NosePiece.Position = Position0;
                break;
			}
		}
		g_Device.NosePiece.Settled = g_Device.NosePiece.Position == Position0 ? false : true; // read nosepiece
	}else {
		g_Device.NosePiece.Settled = false;
	}

	//for reflector
	if (g_Device.Reflector.Encoder == g_DeviceShadow.Reflector.Encoder) {
		if (g_Device.Serie == Axioscope) {
			switch (g_Device.Reflector.Encoder) {
            case 0x04:
                g_Device.Reflector.Position = Position1;
                g_Device.Reflector.Type = Pol4;
                break;
				
            case 0x0A:
                g_Device.Reflector.Position = Position2;
                g_Device.Reflector.Type = Pol4;
                break;
				
            case 0x08:
                g_Device.Reflector.Position = Position3;
                g_Device.Reflector.Type = Pol4;
                break;
				
            case 0x0E:
                g_Device.Reflector.Position = Position4;
                g_Device.Reflector.Type = Pol4;
                break;
				
            case 0x0D:
                g_Device.Reflector.Position = Position1;
                g_Device.Reflector.Type = Pol2;
                break;
				
            case 0x0C:
                g_Device.Reflector.Position = Position2;
                g_Device.Reflector.Type = Pol2;
                break;
				
            case 0x06:
                g_Device.Reflector.Position = Position1;
                g_Device.Reflector.Type = Pol6;
                break;
				
            case 0x05:
                g_Device.Reflector.Position = Position2;
                g_Device.Reflector.Type = Pol6;
                break;
				
            case 0x02:
                g_Device.Reflector.Position = Position3;
                g_Device.Reflector.Type = Pol6;
                break;
				
            case 0x07:
                g_Device.Reflector.Position = Position4;
                g_Device.Reflector.Type = Pol6;
                break;
				
            case 0x03:
                g_Device.Reflector.Position = Position5;
                g_Device.Reflector.Type = Pol6;
                break;
				
            case 0x01:
                g_Device.Reflector.Position = Position6;
                g_Device.Reflector.Type = Pol6;
                break;
				
            default :
                g_Device.Reflector.Position = Position0;
                g_Device.Reflector.Type = Pol6;
                //g_Device.Reflector.Settled = false;
                break;
			}
		}else {		// for Axiolab
		    g_Device.Reflector.Type = Pol4;
			switch (g_Device.Reflector.Encoder) {
            case 0x01:
                g_Device.Reflector.Position = Position1;
                break;

            case 0x02:
                g_Device.Reflector.Position = Position2;
                break;

            case 0x06:
                g_Device.Reflector.Position = Position3;
                break;

            case 0x04:
                g_Device.Reflector.Position = Position4;
                break;

            default :
                g_Device.Reflector.Position = Position0;
                break;
			}

		}
		g_Device.Reflector.Settled = g_Device.Reflector.Position == Position0 ? false : true;
	}else {
		g_Device.Reflector.Settled = false;
	}
}

void
ReadSliderDIP(void)
{
    uint8_t code;
    if (g_I2cMsg.Status != I2C_MSGSTAT_INACTIVE)
        return;

    g_I2cMsg.Status = I2C_MSGSTAT_SEND_NOSTOP;
    g_I2cMsg.DevAddr = 0x22;
    g_I2cMsg.Len = 1;
    g_I2cMsg.RegAddr.all = 0x00;
    g_I2cMsg.LongRegAddrFlag = false;
    do {
        I2CA_Server();
    }while (g_I2cMsg.Status != I2C_MSGSTAT_INACTIVE);
    code = I2cBuffer[0];
    I2cMsg_Reset();

    g_Device.DipCode = code & 0x3F;  // read dip switch code
    g_Device.Serie = (g_Device.DipCode & 0x10) ? Axiolab : Axioscope;
    switch (g_Device.DipCode >> 2 & 0x03) {
    case 0x00:
        g_Device.VarType = VAR_MAT; break;
    case 0x01:
        g_Device.VarType = VAR_FL; break;
    case 0x02:
        g_Device.VarType = VAR_Bio; break;
    case 0x03:
        g_Device.VarType = VAR_Vario; break;
    default : break;
    }

    if (g_Device.Serie == Axioscope) {
        switch (g_Device.DipCode & 0x0f) {
        case 0x0000:
            g_Device.TypeName = kMat_Mot;
            break;
        case 0x0001:
            g_Device.TypeName = kMat_Mot_Indm;
            break;
        case 0x0011:
            g_Device.TypeName = kPol;
            break;
        case 0x0010:
            g_Device.TypeName = kMat;
            break;
        case 0x0100:
            g_Device.TypeName = kFL;
            break;
        case 0x1000:
            g_Device.TypeName = kVario;
            break;
        case 0x1001:
            g_Device.TypeName = kBio_HAL50;
            break;
        default: break;
        }
    }else if (g_Device.Serie == Axiolab) {
        switch (g_Device.DipCode & 0x0f) {
        case 0x1000:
            g_Device.TypeName = kBio;
            break;
        case 0x0100:
            g_Device.TypeName = kFL;
            break;
        case 0x0000:
            g_Device.TypeName = kMat;
            break;
        case 0x0001:
            g_Device.TypeName = kPol;
            break;
        case 0x1001:
            g_Device.TypeName = kBio_Cono;
            break;
        default : break;
        }
    }
}

void
Read_Slider_Position(void)
{
	if (g_Device.Serie == Axiolab) {

	}
}

TASKSTATUS
LockTaskMgr(LockTaskObj *pLockObj)
{
	if ( (pLockObj->pLock->StartTime < 4294967195 && (CpuTimer0.InterruptCount - pLockObj->pLock->StartTime) >= SEC10) ||
			(pLockObj->pLock->StartTime >= 4294967195 && CpuTimer0.InterruptCount + 0xFFFFFFFF - pLockObj->pLock->StartTime >= SEC10) ) {
		if (pLockObj->DevId == DEVID_Z_AXIS || pLockObj->DevId == DEVID_X_AXIS || pLockObj->DevId == DEVID_Y_AXIS || pLockObj->DevId == DEVID_NP) {
			C29ID Id;
			uint8_t payload[4];

			Id.all = 0x1900195F | (Uint32)pLockObj->DevId << 16;
			payload[0] = pLockObj->pLock->Pid;
			payload[1] = 0x61;
			payload[2] = pLockObj->DevId;
			payload[3] = 0x00;
			StartECanTxTask(Id, payload, 4, 30);
		}
		g_Device.LockMask &= ~((Uint64)1 << pLockObj->DevId);
		pLockObj->pLock->Status = false;
		pLockObj->pLock->Pid = 0;
		free(pLockObj);
		return TASKSTS_FINISH;
	}else {
		return TASKSTS_ONGOING;
	}
}

void
CreateLockTask(uint8_t DeviceId, uint8_t Pid, LOCK *pLock)
{
	Task *pnewtask;
	LockTaskObj *locktaskobj;
	C29ID Id;
	uint8_t payload[4];

	locktaskobj = malloc(sizeof(LockTaskObj));
	locktaskobj->DevId = DeviceId;
	locktaskobj->pLock = pLock;
	locktaskobj->pLock->StartTime = CpuTimer0.InterruptCount;
	locktaskobj->pLock->Pid = Pid;
	locktaskobj->pLock->Status = true;

	if (DeviceId == DEVID_Z_AXIS || DeviceId == DEVID_X_AXIS || DeviceId == DEVID_Y_AXIS || DeviceId == DEVID_NP) {
		Id.all = 0x1900195F | (Uint32)DeviceId << 16;
		payload[0]= Pid;
		payload[1] = 0x61;
		payload[2] = DeviceId;
		payload[3] = 0x01;
		StartECanTxTask(Id, payload, 4, 30);
	}

	pnewtask = CreateTask(NORMAL);
	pnewtask->Name = DEVLOCK;
	pnewtask->Status = TASKSTS_ONGOING;
	pnewtask->pfn = (TASKSTATUS (*)(void *))LockTaskMgr;
	pnewtask->pTaskObj = locktaskobj;
}

// return 1: dazzle protection effect
static bool
DazzleProtection(void)
{
	if (!g_Device.DazzleProtect || !g_Device.LM.Enable)
		return false;
	if (!g_Device.NosePiece.Settled ||
			(g_Device.Serie == Axiolab && g_Device.VarType == VAR_FL && !g_Device.Slider.Settled)) {
		DisableUBULB();
		DisableLBULB();
		DisableULEDALL();
		DisableLLED();
		HALIntensityWrite(228);
		return true;
	}
	return false;
}

static void
UpGreen_On(void)
{
	LRedOff();
	LGreenOff();
	URedOff();
	UGreenOn();
}
static void
UpRed_On(void)
{
	LRedOff();
	LGreenOff();
	UGreenOff();
	URedOn();
}
static void
UpOrange_On(void)
{
	LRedOff();
	LGreenOff();
	URedOn();
	UGreenOn();
}
static void
LowGreen_On(void)
{
	URedOff();
	UGreenOff();
	LRedOff();
	LGreenOn();
}
static void
LowRed_On(void)
{
	URedOff();
	UGreenOff();
	LGreenOff();
	LRedOn();
}
static void
LowOrange_On(void)
{
	URedOff();
	UGreenOff();
	LRedOn();
	LGreenOn();
}
static void
AllIndicator_Off(void)
{
	URedOff();
	UGreenOff();
	LRedOff();
	LGreenOff();
}

static void
IndicatorRed_On(void)
{
	if (g_Device.VarType == VAR_Bio || g_Device.VarType == VAR_Vario) {
		UpRed_On();
	} else {		
		if (g_Device.RLTLSw.Sw == LSW_UPPER) {
			UpRed_On();
		}else if (g_Device.RLTLSw.Sw == LSW_LOWER) {
			LowRed_On();
		}
	}
}

static void
IndicatorGreen_On(void)
{
	if (g_Device.VarType == VAR_Bio || g_Device.VarType == VAR_Vario) {
		UpGreen_On();
	} else {		
		if (g_Device.RLTLSw.Sw == LSW_UPPER) {
			UpGreen_On();
		}else if (g_Device.RLTLSw.Sw == LSW_LOWER) {
			LowGreen_On();
		}
	}
}



static void
SetBlink(eIndicatorColor Colour)
{
	g_Device.Indc.RequireColour = Colour;
}
static void
CancelBlink(void)
{
	g_Device.Indc.RequireColour = IND_NONE;
	g_Device.Indc.IndSts = Indsts_Norm;
}

static void
Indicator_Control(void)
{
	if (g_Device.Indc.RequireColour == IND_RED) {
		g_Device.Indc.BlinkColour = IND_RED;
		g_Device.Indc.IndSts = Indsts_1;
		g_Device.Indc.RequireColour = IND_NONE;
		AllIndicator_Off();
		g_Device.Indc.BlinkTime = GetSysTick0();
	}else if (g_Device.Indc.RequireColour == IND_GREEN) {
		g_Device.Indc.BlinkColour = IND_GREEN;
		g_Device.Indc.IndSts = Indsts_1;
		g_Device.Indc.RequireColour = IND_NONE;
		AllIndicator_Off();
		g_Device.Indc.BlinkTime = GetSysTick0();
	}else if (g_Device.Indc.RequireColour == IND_ORANGE) {
		g_Device.Indc.BlinkColour = IND_ORANGE;
		g_Device.Indc.IndSts = Indsts_1;
		g_Device.Indc.RequireColour = IND_NONE;
		AllIndicator_Off();
		g_Device.Indc.BlinkTime = GetSysTick0();
	}
	if (g_Device.Indc.IndSts == Indsts_Norm) { // indicator in NORMAL MODE
		if (g_Device.Mode == SETUP)
			IndicatorRed_On();
		else if (g_Device.Mode == NORM)
			IndicatorGreen_On();
	}else {  									// indicator in BLINKING MODE
		if (GetSysTick0() - g_Device.Indc.BlinkTime >= 5) {
			g_Device.Indc.BlinkTime = GetSysTick0();
			if (g_Device.Indc.IndSts%2) {
				if (g_Device.Indc.BlinkColour == IND_RED) {
					if (g_Device.RLTLSw.Sw == LSW_UPPER)
						UpRed_On();
					else if (g_Device.RLTLSw.Sw == LSW_LOWER)
						LowRed_On();
				}else if (g_Device.Indc.BlinkColour == IND_GREEN) {
					if (g_Device.RLTLSw.Sw == LSW_UPPER)
						UpGreen_On();
					else if (g_Device.RLTLSw.Sw == LSW_LOWER)
						LowGreen_On();
				}else if (g_Device.Indc.BlinkColour == IND_ORANGE) {
					if (g_Device.RLTLSw.Sw == LSW_UPPER)
						UpOrange_On();
					else if (g_Device.RLTLSw.Sw == LSW_LOWER)
						LowOrange_On();
				}
				g_Device.Indc.IndSts++;
			}else if (!(g_Device.Indc.IndSts%2)) {
				g_Device.Indc.BlinkTime = GetSysTick0();
				AllIndicator_Off();
				g_Device.Indc.IndSts++;
			}
			g_Device.Indc.IndSts = g_Device.Indc.IndSts == Indsts_8 ? Indsts_Norm : g_Device.Indc.IndSts;
		}
	}
}

static void
SetUpperIntensity(Uint16 intensity)
{
    if (g_Device.Serie == Axioscope) {
        switch (g_Device.VarType) {
        case VAR_MAT:
        case VAR_Vario:
            if (g_Device.ULight.Type == LightType_LED) {
                goto UpperLEDControl;
            }else {
                HALIntensityWrite(intensity);
            }
            break;
        case VAR_FL : //TODO colibri command
        case VAR_Bio: //no upper light
            break;
        default : break;
        }
    }
    else if (g_Device.Serie == Axiolab) {
        switch (g_Device.VarType) {
        case VAR_MAT:
            if (g_Device.ULight.Type == LightType_LED) {
                goto UpperLEDControl;
            }else {
                DisableULEDALL();
                BulbIntensityWrite(intensity);
                if (!intensity)
                    DisableUBULB();
                else
                    EnableUBULB();
            }
            break;
        case VAR_FL:
            goto UpperLEDControl;
            break;
        case VAR_Bio: // no upper light
        case VAR_Vario: // no vario variant
            break;
        default : break;
        }
    }

UpperLEDControl:
    DisableUBULB();
    BulbVoltageWrite(13.15);
    LEDIntensityWrite(intensity);
    if (!(intensity)) {
        DisableULEDALL();
    }else {
        if (g_Device.VarType == VAR_MAT) {
            EnableULED0();
        }else if (g_Device.VarType == VAR_FL) {
            switch (g_Device.Slider.Position) {
            case Position1:
                EnableULED0(); break;
            case Position2:
                EnableULED1(); break;
            case Position3:
                EnableULED2(); break;
            default : break;
            }
        }
    }
}

static void
SetLowerIntensity(Uint16 intensity)
{
    if (g_Device.Serie == Axioscope) {
        switch (g_Device.VarType) {
        case VAR_MAT:
        case VAR_FL:
            if (g_Device.LLight.Type == LightType_LED) {
                goto LowerLEDControl;
            }else {
                HALIntensityWrite(intensity);
            }
        case VAR_Bio:
            if (g_Device.TypeName == kBio_HAL50) {
                goto LowerBULBControl;
            }else if (g_Device.TypeName == kBio) {
                goto LowerLEDControl;
            }
        case VAR_Vario: // no lower light
            break;
        default : break;
        }
    }
    else if (g_Device.Serie == Axiolab) {
        switch (g_Device.VarType) {
        case VAR_MAT:
        case VAR_FL:
        case VAR_Bio:
            if (g_Device.LLight.Type == LightType_LED) {
                goto LowerLEDControl;
            }else {
                goto LowerBULBControl;
            }
        case VAR_Vario: // no lower light
            break;
        default : break;
        }
    }

LowerLEDControl:
    DisableLBULB();
    BulbVoltageWrite(13.15);
    LEDIntensityWrite(intensity);
    if (!(intensity))
        DisableLLED();
    else
        EnableLLED();
    return;

LowerBULBControl:
    DisableLLED();
    BulbIntensityWrite(intensity);
    if (!intensity)
        DisableLBULB();
    else
        EnableLBULB();
    return;
}

static void
LightManager(void)
{
	if (!g_Device.LM.Enable)
		return;

	//RL light
	if (g_Device.NosePiece.Settled && CurrentRLRatio != 0xffff) {
		if (g_Device.NosePiece.Position != g_DeviceShadow.NosePiece.Position || g_Device.Reflector.Position != g_DeviceShadow.Reflector.Position) { // new settle
//			g_Device.ULight.Intensity = g_Device.LM.URatio == 0xffff ? g_Device.ULight.Intensity : g_Device.ULight.Intensity * CurrentRLRatio / g_Device.LM.URatio;
			if (g_Device.LM.URatio != 0xffff) {
				if (g_Device.LM.LRatio != 0) {		// save the last non-zero value to restore from darkness
					g_Device.ULight.baseRatio = g_Device.LM.URatio;
					g_Device.ULight.baseIntensity = g_Device.ULight.Intensity;
				}
				if (g_Device.Mode == NORM) {
					if (g_Device.ULight.Intensity != 0)
						g_Device.ULight.Intensity = g_Device.ULight.Intensity * CurrentRLRatio / g_Device.LM.URatio;
					else
						g_Device.ULight.Intensity = g_Device.ULight.baseIntensity * CurrentRLRatio / g_Device.ULight.baseRatio;
				} else if (g_Device.Mode == SETUP) {	// leilc: during SETUP, the LM rotation only applies for current nosepiece, per Jonee
					if (CurrentRLRatio != 0xffff)
						g_Device.ULight.Intensity = CurrentRLRatio;
				}
			}

			g_Device.ULight.Intensity_I = g_Device.ULight.Intensity +0.5;
			if (g_Device.ULight.Intensity_I == 0 && g_Device.ULight.Intensity > 0)
				g_Device.ULight.Intensity_I = 1;
		}
		g_Device.LM.URatio = CurrentRLRatio;
	}
	//TL light
	if (g_Device.NosePiece.Settled && CurrentTLRatio != 0xffff) {
		if (g_Device.NosePiece.Position != g_DeviceShadow.NosePiece.Position) {  // new settled
//			g_Device.LLight.Intensity = g_Device.LM.LRatio == 0xffff ? g_Device.LLight.Intensity : g_Device.LLight.Intensity * CurrentTLRatio / g_Device.LM.LRatio;
			if (g_Device.LM.LRatio != 0xffff) {
				if (g_Device.LM.LRatio != 0) {		// save the last non-zero value to restore from darkness
					g_Device.LLight.baseRatio = g_Device.LM.LRatio;
					g_Device.LLight.baseIntensity = g_Device.LLight.Intensity;
				}
				if (g_Device.Mode == NORM) {
					if (g_Device.LLight.Intensity != 0)
						g_Device.LLight.Intensity = g_Device.LLight.Intensity * CurrentTLRatio / g_Device.LM.LRatio;
					else
						g_Device.LLight.Intensity = g_Device.LLight.baseIntensity * CurrentTLRatio / g_Device.LLight.baseRatio;
				} else if (g_Device.Mode == SETUP) {	// leilc: during SETUP, the LM rotation only applies for current nosepiece, per Jonee
					if (CurrentTLRatio != 0xffff)
						g_Device.LLight.Intensity = CurrentTLRatio;
				}
			}

			g_Device.LLight.Intensity_I = g_Device.LLight.Intensity +0.5;
			if (g_Device.LLight.Intensity_I == 0 && g_Device.LLight.Intensity > 0)
				g_Device.LLight.Intensity_I = 1;
		}
		g_Device.LM.LRatio = CurrentTLRatio; //record reference
	}
}

static void
KnobIntensityCtrl(void)
{
	int32 relative = 0;
	float TarValue = 0;

	if (g_Device.RLTLSw.Sw == LSW_UPPER) {
		if (g_Device.ULight.Lock.Status == true)
			return;
	} else if (g_Device.RLTLSw.Sw == LSW_LOWER) {
		if (g_Device.LLight.Lock.Status == true)
			return;
	} else
		return;

	if (ReadEQep() != g_Device.Keys.KnobValue) {
		relative = ReadEQep() - g_Device.Keys.KnobValue;
		g_Device.Keys.KnobValue = ReadEQep();
		if (!g_Device.NosePiece.Settled)
			return;
		if (g_Device.LampLed.Lock.Status == true)
			return;
		if (g_Device.RLTLSw.Sw == LSW_UPPER) {
			if (g_Device.ULight.Intensity_I <= 100)
				TarValue = g_Device.ULight.Intensity_I + relative;
			else
				TarValue = (relative << 2) + g_Device.ULight.Intensity_I;
			TarValue = TarValue > MAX_POSITIONS_LIGHTCTRL ? MAX_POSITIONS_LIGHTCTRL : TarValue;
			TarValue = TarValue < 0    ? 0    : TarValue;
			g_Device.ULight.Intensity_I = TarValue;
			g_Device.ULight.Intensity = g_Device.ULight.Intensity_I;

		}else if (g_Device.RLTLSw.Sw == LSW_LOWER) {
			if (g_Device.LLight.Intensity_I <= 100)
				TarValue = g_Device.LLight.Intensity_I + relative;
			else
				TarValue = (relative << 2) + g_Device.LLight.Intensity_I;
			TarValue = TarValue > MAX_POSITIONS_LIGHTCTRL ? MAX_POSITIONS_LIGHTCTRL : TarValue;
			TarValue = TarValue < 0    ? 0    : TarValue;
			g_Device.LLight.Intensity_I = TarValue;
			g_Device.LLight.Intensity = g_Device.LLight.Intensity_I;		
		}
	} else {
		;
	}
}

static void
LightControl(void)
{
	if (DazzleProtection())
		return;
	LightManager();
	KnobIntensityCtrl();

	//Usb Renew
	// leilc: LampLed is OK for USB set but still not ready for get
#if 0
	if (g_Device.LampLed.UsbRenew) { // means usb send a changer
		g_Device.LampLed.Intensity = (int16)g_Device.LampLed.Intensity < 0 ? 0 : g_Device.LampLed.Intensity;
		g_Device.LampLed.Intensity = g_Device.LampLed.Intensity > MAX_POSITIONS_LIGHTCTRL ? MAX_POSITIONS_LIGHTCTRL : g_Device.LampLed.Intensity;
		if (g_Device.RLTLSw.Sw == LSW_UPPER) {
			g_Device.ULight.Intensity = g_Device.LampLed.Intensity;
			g_Device.LampLed.Intensity = 0;
			g_Device.LampLed.UsbRenew = false;
		}else if (g_Device.RLTLSw.Sw == LSW_LOWER) {
			g_Device.LLight.Intensity = g_Device.LampLed.Intensity;
			g_Device.LampLed.Intensity = 0;
			g_Device.LampLed.UsbRenew = false;
		}else if (g_Device.RLTLSw.Sw == LSW_OFF)
			//do nothing, keep lampled.intensity
		return;
	}
#endif

	// light channel and indicator control
	if (SaveBlink == true) {
		DisableLBULB();
		DisableLLED();
		DisableULEDALL();
		DisableUBULB();
		SaveBlink = false;
	}else if (SaveBlink == false) {
		if (g_Device.RLTLSw.Sw == LSW_UPPER) {
			g_Device.LampLed.LampMode = g_Device.ULight.Intensity == 0 ? false : true;
			DisableLBULB();
			DisableLLED();
			SetUpperIntensity(g_Device.ULight.Intensity_I);
		}else if (g_Device.RLTLSw.Sw == LSW_LOWER) {
			g_Device.LampLed.LampMode = g_Device.LLight.Intensity == 0 ? false : true;
			DisableULEDALL();
			DisableUBULB();
			SetLowerIntensity(g_Device.LLight.Intensity_I);
		}
	}

	if (g_Device.Serie == Axiolab) // axiolab haven't hal100
		return;

	switch (g_Device.VarType) {
	case VAR_MAT :
		if (g_Device.ULight.Type == LightType_LED && g_Device.LLight.Type == LightType_LED)
			DisableHAL100();
		else
			EnableHAL100();
		break;
	case VAR_FL :
		if (g_Device.LLight.Type == LightType_LED)
			DisableHAL100();
		else
			EnableHAL100();
		break;
	case VAR_Vario :
		if (g_Device.ULight.Type == LightType_LED)
			DisableHAL100();
		else
			EnableHAL100();
		break;
	case VAR_Bio :
		if (g_Device.LLight.Type == LightType_LED)
			DisableHAL100();
		else
			EnableHAL100();
		break;
	}
}

void
HardwareInit(void)
{
	DisableLLED();
	DisableULEDALL();

	//close all HAL100
	//TODO should disable and write 0 for formal version
	DisableHAL100();

	AD5640WriteData(0x00);

	//close all BULB
	//Disable8640(); TODO disable 8614
	DisableLBULB();
	DisableUBULB();
	DELAY_US(15000L);
	SPIA_Write(0x1802);  // clear AD5293 write protection
	DELAY_US(200L);
	BulbIntensityWrite(0x3ff);  // max 03ff
    LedPowerOn();
	//DELAY_US(200L);
	//SPIA_ReadWrite(0x0800,0x0000);
	//SPIA_Write(0x0800);
	//DELAY_US(200L);
	//SPIA_ReadWrite(0x0800,0x0000);
	//SPIA_Write(0x0000);
	//EnableUBULB();
}

static void
ParfocalExec(void)
{
	// the parfocal/parcenter values need to be reentrant
	static int32 parcenter_x, parcenter_y, parfocal;
	C29ID c29id;
	uint8_t payload[8];

	uint16_t z_handwheel_res;

	C29ID c29id_Z;
	static uint8_t payload_Z[8];

	c29id_Z.field.CmdClass = 0x1b;
	c29id_Z.field.Dest = 0x0f;
	c29id_Z.field.Src = 0x19;
	c29id_Z.field.CmdNum = 0x5f;

	// move Z-axis down when leave settled position
	// only switch the base index of nosepiece if current position has valid parfocal value
	if (!g_Device.NosePiece.Settled && g_DeviceShadow.NosePiece.Settled && g_Device.dodge_z == false)
	{
		DodgeNosePiece();
		if (g_Device.MotorZ.moving == false && CurrentParfocal != -1) {
			g_Device.MotorZ.PosPrevPar = g_Device.MotorZ.Position;
			g_Device.MotorZ.PrevParIndex = g_Device.NosePiece.Position;
		}
		g_Device.MotorZ.moving = true;
		g_Device.dodge_z = true;
	}

	// CAUSION: need to differentiate the nosepiece switching from the power-on period, in case the NosePiece.Position
	//          has not been initiazed. 

//	if (g_Device.NosePiece.Settled && g_Device.NosePiece.Position != g_DeviceShadow.NosePiece.Position && g_Device.dodge_z == DODGE_DOWN) {
	if (g_Device.NosePiece.Settled && g_Device.dodge_z == true && !g_Device.MotorZ.moving) {
		// in case know where we from and where we go
		if (CurrentParfocal != -1 && PrevParfocal != -1 && g_Device.parfocalEnable)			
//		if (CurrentParfocal != -1 && PrevParfocal != -1 && (CurrentParfocal - PrevParfocal < MAX_PARFOCAL_TRAVEL))
			parfocal = g_Device.MotorZ.PosPrevPar + CurrentParfocal - PrevParfocal;
		else 
			parfocal = g_Device.MotorZ.PosPrevPar;

		payload_Z[0] = PID_GENERAL; //pid
		payload_Z[1] = 0x02; //subid
		payload_Z[2] = 0; //devid
		payload_Z[3] = 0;
		payload_Z[4] = parfocal >> 24;
		payload_Z[5] = parfocal >> 16;
		payload_Z[6] = parfocal >> 8;
		payload_Z[7] = parfocal;

		StartECanTxTask(c29id_Z, payload_Z, 8, 30);

		// update Z handwheel resolution
		z_handwheel_res = getHandwheelResolution(np_mag[g_Device.NosePiece.Position - 1]);
		if (z_handwheel_res != 0) {
			payload_Z[0] = PID_GENERAL; //pid
			payload_Z[1] = 0x50; //subid
			payload_Z[2] = 0; //devid
			payload_Z[3] = (z_handwheel_res & 0xff00) >> 8;
			payload_Z[4] = z_handwheel_res & 0xff;
			StartECanTxTask(c29id_Z, payload_Z, 5, 30);
		}
			
		g_Device.MotorZ.moving = true;
		g_Device.dodge_z = false;
			
		// Parcenter_X
		if (CurrentParcenter_X != -1 && PrevParcenter_X != -1) {
			parcenter_x = g_Device.MotorX.PosPrevPar + CurrentParcenter_X - PrevParcenter_X;
			payload[0] = 0; //pid
			payload[1] = 0x02; //subid
			payload[2] = 0x26; //devid
			payload[3] = 0;
			payload[4] = parcenter_x >> 24;
			payload[5] = parcenter_x >> 16;
			payload[6] = parcenter_x >> 8;
			payload[7] = parcenter_x;
			c29id.field.CmdClass = 0x1b;
			c29id.field.Dest = 0x26;
			c29id.field.Src = 0x19;
			c29id.field.CmdNum = 0x5f;
			StartECanTxTask(c29id, payload, 8, 30);
		}
		// Parcenter_Y
		if (CurrentParcenter_Y != -1 && PrevParcenter_Y != -1) {
			parcenter_y = g_Device.MotorY.PosPrevPar + CurrentParcenter_Y - PrevParcenter_Y;
			payload[0] = 0; 		// pid
			payload[1] = 0x02; 		// subid
			payload[2] = 0x27; 		// devid
			payload[3] = 0;
			payload[4] = parcenter_y >> 24;
			payload[5] = parcenter_y >> 16;
			payload[6] = parcenter_y >> 8;
			payload[7] = parcenter_y;
			c29id.field.CmdClass = 0x1b;
			c29id.field.Dest = 0x27;
			c29id.field.Src = 0x19;
			c29id.field.CmdNum = 0x5f;
			StartECanTxTask(c29id, payload, 8, 30);
		}
	}


	if ((PfInitiated & 0x01) == 0 && g_Device.NosePiece.Settled && CurrentParfocal != -1) {
		PfInitiated |= 0x01;
	}
	if ((PfInitiated & 0x02) == 0 && g_Device.NosePiece.Settled && CurrentParcenter_Y != -1) {
		PfInitiated |= 0x02;
	}
	if ((PfInitiated & 0x04) == 0 && g_Device.NosePiece.Settled && CurrentParcenter_X != -1) {
		PfInitiated |= 0x04;
	}
}

static void
RLTLButtonPoll(void)
{
    if (g_Device.VarType != VAR_Vario && g_Device.VarType != VAR_Bio) {
        if (g_Device.Keys.RLTLStatus == sts00) {
            if (!ReadRLButton() && ReadTLButton()) {
                g_Device.Keys.RLTLStatus = sts01;
            }else if (ReadRLButton() && !ReadTLButton()) {
                g_Device.Keys.RLTLStatus = sts10;
            }else if (ReadRLButton() && ReadTLButton()) {
                g_Device.Keys.RLTLStatus = sts11;
            }
        }else if (g_Device.Keys.RLTLStatus == sts01) {
            if (!ReadRLButton() && !ReadTLButton()) {                                      //'0' is release, '1' is press on
                g_Device.Keys.RLTLStatus = sts00;
                if (!g_Device.RLTLSw.Lock.Status)
                    g_Device.RLTLSw.Sw = LSW_LOWER;
                    switchLampLed(g_Device.RLTLSw.Sw);
            }else if (!ReadRLButton() && ReadTLButton()) {
                g_Device.Keys.RLTLStatus = sts01;
            }else if (ReadRLButton() && !ReadTLButton()) {
                g_Device.Keys.RLTLStatus = sts10;
                if (!g_Device.RLTLSw.Lock.Status)
                    g_Device.RLTLSw.Sw = LSW_LOWER;
                    switchLampLed(g_Device.RLTLSw.Sw);
            }else if (ReadRLButton() && ReadTLButton()) {
                g_Device.Keys.RLTLStatus = sts11;
            }
        }else if (g_Device.Keys.RLTLStatus == sts10) {
            if (!ReadRLButton() && !ReadTLButton()) {                                      //'0' is release, '1' is press on
                g_Device.Keys.RLTLStatus = sts00;
                if (!g_Device.RLTLSw.Lock.Status)
                    g_Device.RLTLSw.Sw = LSW_UPPER;
                    switchLampLed(g_Device.RLTLSw.Sw);
            }else if (!ReadRLButton() && ReadTLButton()) {
                g_Device.Keys.RLTLStatus = sts01;
                if (!g_Device.RLTLSw.Lock.Status)
                    g_Device.RLTLSw.Sw = LSW_UPPER;
                    switchLampLed(g_Device.RLTLSw.Sw);
            }else if (ReadRLButton() && !ReadTLButton()) {
                g_Device.Keys.RLTLStatus = sts10;
            }else if (ReadRLButton() && ReadTLButton()) {
                g_Device.Keys.RLTLStatus = sts11;
            }
        }else if (g_Device.Keys.RLTLStatus == sts11) {
            if (!ReadRLButton() && !ReadTLButton()) {                                      //'0' is release, '1' is press on
                g_Device.Keys.RLTLStatus = sts00;
            }else if (!ReadRLButton() && ReadTLButton())
                g_Device.Keys.RLTLStatus = sts01;
            else if (ReadRLButton() && !ReadTLButton())
                g_Device.Keys.RLTLStatus = sts10;
            else if (ReadRLButton() && ReadTLButton())
                g_Device.Keys.RLTLStatus = sts11;
        }
    }
}

static void
LmSnapButtonPoll(void)
{
	int16 DescEntry;

	if (g_Device.Keys.LmSnap == sts00) {
		if (!ReadLmButton() && ReadSnapButton()) {
			g_Device.Keys.LmSnap = sts01;
			g_Device.Keys.StartTime = GetSysTick0();
			if (!ReadLmButton() && ReadSnapButton()) {
				if (GetSysTick0() - g_Device.Keys.StartTime_single_snap < MSEC600) {      // double click of snap
					LoadSample();
				}
			}
			g_Device.Keys.StartTime_single_snap = GetSysTick0();		
		}else if (ReadLmButton() && !ReadSnapButton()) {
			g_Device.Keys.LmSnap = sts10;
			g_Device.Keys.StartTime = GetSysTick0();
		}else if (ReadLmButton() && ReadSnapButton()) {
			g_Device.Keys.LmSnap = sts11;
			g_Device.Keys.StartTime = GetSysTick0();
		}
	}else if (g_Device.Keys.LmSnap == sts01) {
		if (!ReadLmButton() && !ReadSnapButton()) {                                      //'0' is release, '1' is press on
			g_Device.Keys.LmSnap = sts00;
			g_Device.Keys.StartTime = 0;
			//todo event notification
		}else if (ReadLmButton() && !ReadSnapButton()) {
			g_Device.Keys.LmSnap = sts10;
			g_Device.Keys.StartTime = GetSysTick0();
			//todo event notification
		}else if (ReadLmButton() && ReadSnapButton()) {
			g_Device.Keys.LmSnap = sts11;
			g_Device.Keys.StartTime = GetSysTick0();
			//todo event notification
		}
	}else if (g_Device.Keys.LmSnap == sts10) {
		if (GetSysTick0() - g_Device.Keys.StartTime >= SEC3) {
			if (g_Device.Mode == NORM)
				g_Device.Mode = SETUP;
			else if (g_Device.Mode == SETUP) {
				g_Device.Mode = NORM;
				PfClearAll = 1;
			}
			g_Device.Keys.LmSnap = sts100;
		} else if (!ReadLmButton() && !ReadSnapButton()) {                                      //'0' is release, '1' is press on
			g_Device.Keys.LmSnap = sts00;
			if (g_Device.Mode == SETUP) {  // save parfocal and LM
				// save parfocal
				StartParcenterSaveTask(DESC_XYZ, 0x40 + g_Device.NosePiece.Position);
				// save LM
				if (g_Device.RLTLSw.Sw == LSW_UPPER && g_Device.NosePiece.Position) {
					CurrentRLRatio = g_Device.ULight.Intensity +0.5;
					g_Device.LM.URatio = CurrentRLRatio;
					DescEntry = 0x10 + g_Device.Reflector.Position*16 + g_Device.NosePiece.Position - 1;
					WriteDesc(DESC_LIGHTCTRL + g_Device.RLTLSw.Sw, DescEntry, &CurrentRLRatio, 1, DESCFMT_USHORT);
					SaveBlink = true;
				}else if (g_Device.RLTLSw.Sw == LSW_LOWER && g_Device.NosePiece.Position) {
					CurrentTLRatio = g_Device.LLight.Intensity +0.5;
					g_Device.LM.LRatio = CurrentTLRatio;
					DescEntry = 0x10 + g_Device.NosePiece.Position - 1;
					WriteDesc(DESC_LIGHTCTRL + g_Device.RLTLSw.Sw, DescEntry, &CurrentTLRatio, 1, DESCFMT_USHORT);
					SaveBlink = true;
				}
			}
			g_Device.Keys.StartTime = 0;
			//todo event notification
		}else if (!ReadLmButton() && ReadSnapButton()) {
			g_Device.Keys.LmSnap = sts01;
			g_Device.Keys.StartTime = GetSysTick0();
			//todo event notification
		}else if (ReadLmButton() && ReadSnapButton()) {
			g_Device.Keys.LmSnap = sts11;
			g_Device.Keys.StartTime = GetSysTick0();
			//todo event notification
		}
	}else if (g_Device.Keys.LmSnap == sts11) {
		if (GetSysTick0() - g_Device.Keys.StartTime >= SEC3) {
			if ((g_Device.LM.Enable == 0) && (g_Device.Mode == NORM)) {
			// green -> dark -> green, hot fix, need revised into framework
				AllIndicator_Off();
				DELAY_US(500000);
				IndicatorGreen_On();
				DELAY_US(500000);
			} else if ((g_Device.LM.Enable == 1) && (g_Device.Mode == NORM)) {
				// green -> red -> green
				AllIndicator_Off();
				DELAY_US(500000);
				IndicatorRed_On();
				DELAY_US(500000);
			}
			AllIndicator_Off();
			DELAY_US(500000);
		}
		if (GetSysTick0() - g_Device.Keys.StartTime >= SEC3) {
			if (g_Device.Mode == NORM) {	// LM enable/disable only applies for normal mode
				g_Device.LM.Enable ^= 1;
			}
			//todo notification
			g_Device.Keys.LmSnap = sts100;
		}
		if (!ReadLmButton() && !ReadSnapButton()) {                                      //'0' is release, '1' is press on
			g_Device.Keys.LmSnap = sts00;
		}else if (!ReadLmButton() && ReadSnapButton()) {
			g_Device.Keys.LmSnap = sts01;
			g_Device.Keys.StartTime = GetSysTick0();
			//todo event notification
		}else if (ReadLmButton() && !ReadSnapButton()) {
			g_Device.Keys.LmSnap = sts10;
			g_Device.Keys.StartTime = GetSysTick0();
			//todo event notification
		}
	}else if (g_Device.Keys.LmSnap == sts100) {
		// in the first version the only entrance to sts100 is long press snap + LM
		// Charlie expanded the meaning of sts100, that it can also be entered from other key status
		// and use the sts100 as a one-shot trigger for any long-press
		// the only way to get out of sts100 is to release both LM and snap keys 
		if (!ReadLmButton() && !ReadSnapButton()) {                                      
			g_Device.Keys.LmSnap = sts00;
		}
	}
}

static void
MultiButtonPoll(void)
{
	
}


int32 tempX0;
int32 tempX1;
int32 tempY0;
int32 tempY1;
int32 valueX;
int32 valueY;

static Bool snap_x = false, snap_y = false;
static void
stageControl(void)  //move x-y stage
{
    int32 value;
    C29ID c29id;
    uint8_t payload[8] = {0};

    if (ReadHandWheel_X()) {
        if (ReadHandWheel_X() != snap_x) {
            //lock z
            c29id.all = 0x1b0f195f;
            payload[0] = 0x01;
            payload[1] = 0x61;
            payload[2] = 0x00;
            payload[3] = 0x01;
            StartECanTxTask(c29id, payload, 4, 30);
            // snap_x = ReadHandWheel_X();
            snap_x = 1; // leilc: it's safer to store 1 than read it again
//          monitorHandwheel(true);  // leilc: monitor handwheel
        }else if (g_Device.HandWheelCode != g_DeviceShadow.HandWheelCode) {
            value = (g_Device.HandWheelCode - g_DeviceShadow.HandWheelCode) * 10000;

            // leilc debug
            tempX0 = g_DeviceShadow.HandWheelCode;
            tempX1 = g_Device.HandWheelCode;
            valueX = value;

            c29id.all = 0x1b26195f;
            payload[0] = 0x01;
            payload[1] = 0x03;
            payload[2] = 0;
            payload[3] = 0x00;
            payload[4] = (value >> 24) & 0xff;
            payload[5] = (value >> 16) & 0xff;
            payload[6] = (value >> 8) & 0xff;
            payload[7] = value & 0xff;
            StartECanTxTask(c29id, payload, 8, 30);
        }
    }else if (!ReadHandWheel_X()) {
        if (ReadHandWheel_X() != snap_x) {
            //unlock z
            c29id.all = 0x1b0f195f;
            payload[0] = 0x01;
            payload[1] = 0x61;
            payload[2] = 0x00;
            payload[3] = 0x00;
            StartECanTxTask(c29id, payload, 4, 30);
            snap_x = 0;
//          monitorHandwheel(false);
        }
    }

    if (ReadHandWheel_Y()) {
        if (ReadHandWheel_Y() != snap_y) {
            //lock z
            c29id.all = 0x1b0f195f;
            payload[0] = 0x01;
            payload[1] = 0x61;
            payload[2] = 0x00;
            payload[3] = 0x01;
            StartECanTxTask(c29id, payload, 4, 30);
            snap_y = 1;
//          monitorHandwheel(true);
        }else if (g_Device.HandWheelCode != g_DeviceShadow.HandWheelCode) {
            value = (g_Device.HandWheelCode - g_DeviceShadow.HandWheelCode) * 10000;

            // leilc debug
            tempY0 = g_DeviceShadow.HandWheelCode;
            tempY1 = g_Device.HandWheelCode;
            valueY = value;

            c29id.all = 0x1b27195f;
            payload[0] = 0x01;
            payload[1] = 0x03;
            payload[2] = 0;
            payload[3] = 0x00;
            payload[4] = (value >> 24) & 0xff;
            payload[5] = (value >> 16) & 0xff;
            payload[6] = (value >> 8) & 0xff;
            payload[7] = value & 0xff;
            StartECanTxTask(c29id, payload, 8, 30);
        }
    }else if (!ReadHandWheel_Y()) {
        if (ReadHandWheel_Y() != snap_y) {
            //unlock z
            c29id.all = 0x1b0f195f;
            payload[0] = 0x01;
            payload[1] = 0x61;
            payload[2] = 0x00;
            payload[3] = 0x00;
            StartECanTxTask(c29id, payload, 4, 30);
            snap_y = 0;
//          monitorHandwheel(false);
        }
    }
}

TASKSTATUS
MainControl(void)
{
	if (g_ulSysTickCount == CpuTimer0.InterruptCount)
		return TASKSTS_ONGOING;

	g_ulSysTickCount = GetSysTick0();
	stageControl();

	if (g_Device.Mode == NORM || g_Device.Mode == SETUP)
	{
		Read_Refl_Encoder();

		Read_NP_Encoder();

		Read_NPRefl_Position();

		Read_RLTL_ID();

		ParfocalExec();

		monitorExec();

		LightctrlExec();

		RLTLButtonPoll();

		LightControl();		//include USB set LAMPLED, LightManager, Knob LightControl

		Indicator_Control();

		LmSnapButtonPoll();

		TLShutterSet();
		

	}//end of mode NORM/SETUP
	else if (g_Device.Mode == OSDMENU)
	{

	}
	MemCopy((Uint16 *)&g_Device.Mode, (Uint16 *)&g_Device + sizeof(Device), (Uint16 *)&g_DeviceShadow.Mode);
	return TASKSTS_ONGOING;
}

void Reset_gDeviceShadow(void)
{
	memset(&g_DeviceShadow, 0, sizeof(Device));
}

void LoadSample(void)
{
    static int32 relative = LOAD_TRAVEL;
    static int32 prevPosition = 0;
    int32 posError;
    C29ID c29id;
    uint8_t payload[8] = {0};
    uint8_t pid;

    if (g_Device.Mode != NORM)  // judge here to save space for key handlers
        return;

    if (g_Device.MotorZ.moving == true)
        return;

//  pid = GetPid();
    pid = 2;
    posError = prevPosition - g_Device.MotorZ.Position;

    // to protect the nosepiece: the user may forgot now it's in load position!
    // posError%TRAVEL was added in case the g_Device.MotorZ.Position was not refreshed
    if ((posError > 100000 || posError < -100000) && posError%LOAD_TRAVEL)
        relative = -LOAD_TRAVEL;
    else
        relative = -relative;

//  c29id.all = 0x180f195f;
    payload[0] = pid; //pid
    payload[1] = 0x13; //relative locked
    payload[2] = 0x00; //devid
    payload[3] = 0;
    payload[4] = (relative >> 24) & 0xff;
    payload[5] = (relative >> 16) & 0xff;
    payload[6] = (relative >> 8) & 0xff;
    payload[7] = relative & 0xff;
    c29id.field.CmdClass = 0x19;
    c29id.field.Dest = 0x0f;
    c29id.field.Src = 0x19;
    c29id.field.CmdNum = 0x5f;
    StartECanTxTask(c29id, payload, 8, 30);
    prevPosition = g_Device.MotorZ.Position + relative;

    g_Device.MotorZ.moving = true;
}

void DodgeNosePiece(void)
{
	int32 target;
	C29ID c29id;
	uint8_t payload[8] = {0};

	if (g_Device.MotorZ.moving == true)     // the Z-axis is moving up, so just move back to where it's from
		target = g_Device.MotorZ.PosPrevPar + DODGE_NP;
	else                                    // if the Z-axis is still, move down DODGE_NP
		target = g_Device.MotorZ.Position+ DODGE_NP;
	
	c29id.field.CmdClass = 0x19;
	c29id.field.Dest = 0x0f;
	c29id.field.Src = 0x19;
	c29id.field.CmdNum = 0x5f;

	payload[0] = PID_Z_MOVE; //pid
	payload[1] = 0x02; //absolute
	payload[2] = 0x00; //devid
	payload[3] = 0;    // mode
	payload[4] = (target >> 24) & 0xff;
	payload[5] = (target >> 16) & 0xff;
	payload[6] = (target >> 8) & 0xff;
	payload[7] = target & 0xff;

	StartECanTxTask(c29id, payload, 8, 30);
}

Uint16 getHandwheelResolution(float magnification)
{
	Uint16 resolution;
	switch ((int)(magnification * 100))		// magnication x 100. The switch sentense accepts int for branch instead of float
	{
		case 100:
		case 125:
		case 250:
			resolution = 62;
			break;
			
		case 500:
			resolution = 124;
			break;
			
		case 1000:
			resolution = 250;
			break;
			
		case 2000:
			resolution = 500;
			break;
			
		case 3200:
		case 4000:
			resolution = 1000;
			break;

		case 5000:
		case 6300:
			resolution = 4000;
			break;

		case 10000:
			resolution = 16000;
			break;

		default:
			resolution = 0;
	}
	return resolution;
}

void switchLampLed(eLIGHTSW sw)
{
    g_Device.LampLed.pLed = (g_Device.RLTLSw.Sw == LSW_UPPER) ? &g_Device.ULight : &g_Device.LLight;
    g_Device.LampLed.LampMode = (g_Device.LampLed.pLed->Intensity_I == 0) ? false : true;
}



