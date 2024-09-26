/*----------------------------------------------------------------------------
| File        : TsLIN.h
| Project     : TsAPI
|
| Description : LIN interface and automation related testing platform for vector hardware
|-----------------------------------------------------------------------------
| Version     : 1.0
| Author      : Hao Zheng, Mingbo Li
| Date        : 2024/7/22
|---------------------------------------------------------------------------*/

#ifndef TSLIN_H
#define TSLIN_H

#include "Bus.h"
#include "vxlapi.h"

#define RECEIVE_EVENT_SIZE       1     // DO NOT EDIT! Currently 1 is supported only
#define MASTER                   1     //!< channel is a master
#define SLAVE                    0     //!< channel is a slave
#define DEFAULT_LIN_DLC          8     //!< default DLC for master/slave
#define DEFAULT_LIN_BAUDRATE 19200     //!< default LIN baudrate

//global vriables

typedef struct {
    XLportHandle xlPortHandle;
    HANDLE      hMsgEvent;
} TStruct_LIN;

TS_API typedef struct{
    unsigned int    ID;
    unsigned int    dlc;
    std::string     dir;
    std::vector<unsigned char>  payload;
} rxPayload;

class TS_API LIN : public IBus
{
public:

    /**
     * @brief Constructs a LIN object with the specified parameters.
     * 
     * @param baudrate The baudrate for LIN communication.
     * @param LinVersion The version of the LIN protocol.
     * @param isMaster Specifies whether the LIN object is a master or slave.
     */
    LIN(int baudrate, unsigned int LinVersion, bool isMaster = true);
    virtual ~LIN();


    /**
     * Sends a master request on the LIN bus.
     * WARNING: To be used only for a master simulation.
     *
     * @param linID The LIN ID of the master request.
     * @return The status of the request.
     */
    XLstatus            LINSendMasterReq(unsigned int linID);

    /**
     * @brief Closes the LIN communication and releases associated resources.
     *
     * This function deactivates the LIN channels, closes the LIN port, and closes the LIN driver.
     * It also waits for the receive thread to finish before returning.
     *
     * @return The status of the LIN close operation. Returns XL_SUCCESS (0) if successful, or an error code if there was a failure.
     */
    XLstatus            GoOffBus() override;

    /**
     * Sets the LIN slave configuration for the specified LIN ID.
     *
     * @param linID The LIN ID of the slave.
     * @param data An array of 8 bytes representing the data to be sent by the slave.
     * @param dlc The data length code (DLC) specifying the number of bytes in the data array.
     * @return The XLstatus indicating the success or failure of the operation.
     */
    XLstatus            linSetSlave(unsigned int linID, unsigned char data[8], unsigned int dlc);



    /**
     * @brief Creates and starts a receive thread for LIN communication.
     * 
     * This function creates a receive thread for LIN communication and starts it. 
     * The receive thread is responsible for receiving LIN messages and handling them.
     * 
     * @return The status of the operation. Returns XL_SUCCESS if successful, otherwise an error code.
     */
    XLstatus            linCreateRxThread();
    std::thread&        getRxThread() { return receiveThread; }
    XLstatus            updateRxPayloads(XLevent* pEvent);
    void                printRxPayloads();
    std::vector<rxPayload> getRxPayloads() { return linRxPayloads; }

private:

    std::shared_mutex mtxLIN;
    /**
     * @brief Retrieves the LIN device and initializes the LIN communication.
     * 
     * This function opens the LIN driver, retrieves the LIN channel mask,
     *  and ensures that at least one LIN channel is available for MASTER/SLAVE configuration.
     * 
     * @return The status of the LIN device retrieval and initialization.
     */
    XLstatus LINGetDevice();

    /**
     * Initializes the LIN communication.
     *
     * @param baudrate The desired baud rate for the LIN communication.
     * @param LinVersion The version of the LIN protocol to be used.
     * @param isMaster Specifies whether the LIN communication is in master mode or not.
     * @return The status of the LIN initialization.
     */
    XLstatus LINInit(int baudrate = DEFAULT_LIN_BAUDRATE, unsigned int LinVersion = XL_LIN_VERSION_2_0, bool isMaster = true);

    /**
     * Retrieves the channel mask for the LIN bus.
     * Check hardware support for LIN and get the channel mask.
     * 
     * @return The XLstatus indicating the success or failure of the operation.
     */
    XLstatus         linGetChannelMask();
    /**
     * Initializes the LIN channel as a master with the specified baudrate and LIN version.
     * 
     * Including setting the master mode, and set checksum(XL_LIN_CHECKSUM_ENHANCED) and DLC(8) for all LINId.
     *
     * @param baudrate The desired baudrate for the LIN communication.
     * @param LinVersion The LIN version to be used.
     * @return The status of the initialization process.
     */
    XLstatus         linInitMaster(int baudrate, unsigned int LinVersion);
    /**
     * Initializes the LIN channel as a slave with the specified baudrate and LIN version.
     * Including setting the slave mode, and set checksum(XL_LIN_CHECKSUM_ENHANCED) and DLC(8) for all LINId.
     *
     * @param baudrate The baudrate to set for the LIN channel.
     * @param LinVersion The LIN version to set for the LIN channel.
     * @return The status of the initialization process.
     */
    XLstatus         linInitSlave(int baudrate, unsigned int LinVersion);

    /**
     * Activates the LIN channel.
     * 
     * This function activates the LIN channel by calling the xlActivateChannel function with the appropriate parameters.
     * It also sets the timer rate to 100 and flushes the receive queue.
     * 
     * @return The status of the activation process. Returns XL_SUCCESS if successful, otherwise an error code.
     */
    XLstatus            linActivateChannel();

    XLaccess         m_xlChannelMask;
    int              m_xlChannelIndex;
    XLportHandle     m_xlPortHandle;
    XLhandle         m_hMsgEvent;
    HANDLE           m_hThread;
    int              m_baudrate;
    unsigned int     m_LinVersion;
    char            m_AppName[XL_MAX_APPNAME + 1] = "TsLIN";               //!< Application name which is displayed in VHWconf
    std::thread     receiveThread;

    // id, dlc, Tx/Rx, payload
    std::vector<rxPayload> linRxPayloads;
};

TS_API void RxThread_LIN(LIN& lin, TStruct_LIN& pTh);

#endif // TSLIN_H