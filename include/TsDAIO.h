/*----------------------------------------------------------------------------
| File        : TsDAIO.h
| Project     : TsAPI
|
| Description : DA/IO interface and automation related testing platform for vector hardware
|-----------------------------------------------------------------------------
| Version     : 1.0
| Author      : Zheng, Hao
| Date        : 2024/7/22
|---------------------------------------------------------------------------*/

#ifndef TsDAIO_H
#define TsDAIO_H

#include "Bus.h"

// defines
#define RECEIVE_EVENT_COUNT 1


typedef struct {
    XLportHandle xlPortHandle;
    HANDLE       hMsgEvent;
} TStruct_DAIO;

class TS_API DAIO : public IBus
{
public:
	DAIO(unsigned short portType, unsigned int cycleTime = 1000); //unit of cycleTime:ms
	~DAIO();

	XLstatus  daioInit();
    XLstatus  GoOnBus();
    XLstatus  GoOffBus() override;
	XLstatus  daioSetupCab();       //-CANcardXL(and the IOcab8444opto)
	XLstatus  daioSetupFixPiggy();  //- On - board DAIO of VN16xx devices
	XLstatus  daioSetupPiggy();     //- IOpiggy 8642
	void      CloseExample();
    void      SetDigitalOut(unsigned int portMask = XL_DAIO_PORT_MASK_DIGITAL_D0, unsigned int valueMask = 0);
    void      SetAnalogOut(unsigned int portMask = XL_DAIO_PORT_MASK_ANALOG_A0, unsigned int valueMask = 0);
    int       g_ioPiggyDigitalTriggerCyclic;                  // Cyclic or on-edge digital trigger

private:
    XLstatus         daioCreateRxThread();

    XLaccess         m_xlChannelMask;             
    unsigned int     m_xlChannelIndex;            
    XLportHandle     m_xlPortHandle;              // Global porthandle (we use only one!)
    XLaccess         m_xlChannelMask_both;
    XLaccess         m_xlPermissionMask;         // Global permissionmask (includes all founded channels)
    XLdriverConfig   m_xlDrvConfig;              // Contains the actual hardware configuration 

    unsigned short   m_trcvType;                // Transceiver type on DAIO channel
    unsigned short  m_portType;                // Port type on DAIO channel

    HANDLE           m_Thread;
    std::thread      receiveThread;
    XLhandle         m_hMsgEvent;
    int              m_bInitDone;

    char             m_AppName[XL_MAX_LENGTH + 1] = "TsDAIO";  // Application name which is displayed in VHWconf



    // Measuring Variables
    unsigned int        m_frequency;
    unsigned int        m_outputMilliVolt;
};

void daioRxThread(DAIO& daio, TStruct_DAIO& pTh);

#endif // TsDAIO_H