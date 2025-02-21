/*----------------------------------------------------------------------------
| File        : TsDAIO.hpp
| Project     : TsAPI
|
| Description : DA/IO interface and automation related testing platform for vector hardware
|-----------------------------------------------------------------------------
| Version     : 3.0
| Author      : Hao Zheng, Mingbo Li
| Date        : 2025/2/20
|---------------------------------------------------------------------------*/

#ifndef TsDAIO_HPP
#define TsDAIO_HPP

#include "Bus.hpp"

// defines
#define RECEIVE_EVENT_COUNT 1


typedef struct {
    XLportHandle xlPortHandle;
    HANDLE       hMsgEvent;
} TStruct_DAIO;

class TS_API DAIO : public IBus {
public:
	DAIO(XLdaioSetPort portCfg, int channel, int appCh = 0, unsigned int cycleTime = 100);
	~DAIO() override;

	XLstatus    daioInit();
    XLstatus    GoOnBus();
    XLstatus    GoOffBus() override;
    void        SetDigitalOut(unsigned int portMask = XL_DAIO_PORT_MASK_DIGITAL_D0, unsigned int valueMask = 0);
    void        SetAnalogOut(unsigned int portMask = XL_DAIO_PORT_MASK_ANALOG_A0, unsigned int valueMask = 0);
    void        updateAnalog(XL_IO_ANALOG_DATA data);
    void        updateDigital(XL_IO_DIGITAL_DATA data);
    int         g_ioPiggyDigitalTriggerCyclic;                  // Cyclic or on-edge digital trigger

private:
    void        daioRxThread();
    XLstatus    daioCreateRxThread();
	XLstatus    daioSetupCab();       //-CANcardXL(and the IOcab8444opto)
	XLstatus    daioSetupFixPiggy();  //- On - board DAIO of VN16xx devices
	XLstatus    daioSetupPiggy();     //- IOpiggy 8642
    BOOL            g_bThreadRun_DAIO;
    XLaccess         m_xlChannelMask;             
    unsigned int     m_xlChannelIndex;            
    XLportHandle     m_xlPortHandle;              // Global porthandle (we use only one!)
    XLdriverConfig   m_xlDrvConfig;              // Contains the actual hardware configuration 
    unsigned short   m_trcvType;                // Transceiver type on DAIO channel
    XLdaioSetPort    m_portCfg;
    std::thread      receiveThread;
    XLhandle         m_hMsgEvent;
    int              m_bInitDone;
    char             m_AppName[XL_MAX_LENGTH + 1] = "TsAPI";  // Application name which is displayed in VHWconf
    unsigned int    appChannel;

    // Measuring Variables
    unsigned int        m_frequency;
    unsigned int        m_outputMilliVolt;
    unsigned int        m_analogData[4];
    unsigned int        m_digitalData;
};

#endif // TsDAIO_HPP