#ifndef SENDFUNCTIONS_HPP
#define SENDFUNCTIONS_HPP

#include <stdexcept>
#include <chrono>
#include <memory>
#include "dbc_parser.hpp"
#include "ldf_parser.hpp"
#include "TsCAN.h"
#include "TsCANFD.h"
#include "TsLIN.h"

#ifdef SEND_FUNCTION_EXPORTS
#define SENDFUNCTION_API __declspec(dllexport)
#elif defined(SEND_FUNCTION_STATIC)
#define SENDFUNCTION_API
#else
#define SENDFUNCTION_API __declspec(dllimport)
#endif

static uint8_t crc_table[256] =
{
0x00, 0x1D, 0x3A, 0x27, 0x74, 0x69, 0x4E, 0x53,
0xE8, 0xF5, 0xD2, 0xCF, 0x9C, 0x81, 0xA6, 0xBB,
0xCD, 0xD0, 0xF7, 0xEA, 0xB9, 0xA4, 0x83, 0x9E,
0x25, 0x38, 0x1F, 0x02, 0x51, 0x4C, 0x6B, 0x76,
0x87, 0x9A, 0xBD, 0xA0, 0xF3, 0xEE, 0xC9, 0xD4,
0x6F, 0x72, 0x55, 0x48, 0x1B, 0x06, 0x21, 0x3C,
0x4A, 0x57, 0x70, 0x6D, 0x3E, 0x23, 0x04, 0x19,
0xA2, 0xBF, 0x98, 0x85, 0xD6, 0xCB, 0xEC, 0xF1,
0x13, 0x0E, 0x29, 0x34, 0x67, 0x7A, 0x5D, 0x40,
0xFB, 0xE6, 0xC1, 0xDC, 0x8F, 0x92, 0xB5, 0xA8,
0xDE, 0xC3, 0xE4, 0xF9, 0xAA, 0xB7, 0x90, 0x8D,
0x36, 0x2B, 0x0C, 0x11, 0x42, 0x5F, 0x78, 0x65,
0x94, 0x89, 0xAE, 0xB3, 0xE0, 0xFD, 0xDA, 0xC7,
0x7C, 0x61, 0x46, 0x5B, 0x08, 0x15, 0x32, 0x2F,
0x59, 0x44, 0x63, 0x7E, 0x2D, 0x30, 0x17, 0x0A,
0xB1, 0xAC, 0x8B, 0x96, 0xC5, 0xD8, 0xFF, 0xE2,
0x26, 0x3B, 0x1C, 0x01, 0x52, 0x4F, 0x68, 0x75,
0xCE, 0xD3, 0xF4, 0xE9, 0xBA, 0xA7, 0x80, 0x9D,
0xEB, 0xF6, 0xD1, 0xCC, 0x9F, 0x82, 0xA5, 0xB8,
0x03, 0x1E, 0x39, 0x24, 0x77, 0x6A, 0x4D, 0x50,
0xA1, 0xBC, 0x9B, 0x86, 0xD5, 0xC8, 0xEF, 0xF2,
0x49, 0x54, 0x73, 0x6E, 0x3D, 0x20, 0x07, 0x1A,
0x6C, 0x71, 0x56, 0x4B, 0x18, 0x05, 0x22, 0x3F,
0x84, 0x99, 0xBE, 0xA3, 0xF0, 0xED, 0xCA, 0xD7,
0x35, 0x28, 0x0F, 0x12, 0x41, 0x5C, 0x7B, 0x66,
0xDD, 0xC0, 0xE7, 0xFA, 0xA9, 0xB4, 0x93, 0x8E,
0xF8, 0xE5, 0xC2, 0xDF, 0x8C, 0x91, 0xB6, 0xAB,
0x10, 0x0D, 0x2A, 0x37, 0x64, 0x79, 0x5E, 0x43,
0xB2, 0xAF, 0x88, 0x95, 0xC6, 0xDB, 0xFC, 0xE1,
0x5A, 0x47, 0x60, 0x7D, 0x2E, 0x33, 0x14, 0x09,
0x7F, 0x62, 0x45, 0x58, 0x0B, 0x16, 0x31, 0x2C,
0x97, 0x8A, 0xAD, 0xB0, 0xE3, 0xFE, 0xD9, 0xC4
};

class SendFunc : public Observer {
public:
    SendFunc() {
        this->db = nullptr;
        realTimeSignals = {};
    }
    void setDB(dbParser* db) { this->db = db; }
    void update(std::pair<std::vector<unsigned long>, std::string> id_length_dir, unsigned char* payload) override;
    std::map<unsigned long, std::map<std::string, double>> getReadTimeSignals() {return realTimeSignals;}
    void printRealTimeSignals() {
        for (auto& m : realTimeSignals) {
            std::cout << "\nMessage id: 0x" << std::hex << m.first << std::dec << std::endl;
            for (auto& s : m.second) {
                std::cout << "\t" << s.first << ": " << s.second << std::endl;
            }
        }
    }
private:
    dbParser* db;
    std::map<unsigned long, std::map<std::string, double>> realTimeSignals;
};

/**
 * @brief Generates a CRC checksum for the given data.
 *
 * @param data Pointer to the data array.
 * @param len Length of the data array.
 * @return The calculated CRC checksum.
 */
static uint8_t gencrc(uint8_t *data, size_t len);

/**
 * @brief Generates the E2E payload.
 * 
 * This function generates the E2E payload based on the given CRC and stores it in the provided payload vector.
 * 
 * @param payload The raw data payload and will be updated with the E2E payload after function execution.
 * @param crc The CRC values to be used for payload generation. Including the start byte index, protected data length, and data ID.
 * @param crctype The type of CRC Algorithm to be used (default value is 1).
 */
void E2E_PayloadGeneration(std::vector<unsigned char>& payload, std::vector<unsigned short> crc, int crctype = 1);

/**
 * @brief Calculates the Data Length Code (DLC) based on the given data length.
 *
 * @param length The data length for which to calculate the DLC.
 * @return The calculated Data Length Code (DLC).
 */
SENDFUNCTION_API unsigned char getDlcFromDataLength(unsigned int length);

/**
 * Sends CAN encoded payloads to the specified CAN FD bus.
 * 
 * @param canfd The CAN FD object used for communication.
 * @param encodedPayloads The Payloads_CAN object containing the payloads to be sent.
 */
SENDFUNCTION_API void SendCANEncodedPayloadsThread(std::shared_ptr<CAN> can, Payloads_CAN& encodedPayloads);

/**
 * Sends CAN FD encoded payloads to the specified CAN FD bus.
 * 
 * @param canfd The CAN FD object used for communication.
 * @param encodedPayloads The Payloads_CAN object containing the payloads to be sent.
 */
SENDFUNCTION_API void SendCANFDEncodedPayloadsThread(std::shared_ptr<CANFD> canfd, Payloads_CAN& encodedPayloads);

/**
 * Sets the slave and creates a receive thread for LIN communication.
 * 
 * @param lin The LIN object for communication.
 * @param payloads The Payloads object containing the payloads to be sent.
 */
SENDFUNCTION_API void SetSlaveAndCreateRxThread(std::shared_ptr<LIN> lin, Payloads_LIN& payloads);

/**
 * Updates the slave with the specified LIN ID by setting the value of the given signal.
 * 
 * @param lin The LIN object used for communication.
 * @param ldfFile The LdfParser object used for parsing LDF files.
 * @param linid The LIN ID of the slave to be updated.
 * @param signalName The name of the signal to be updated.
 * @param value The new value to be set for the signal.
 */
SENDFUNCTION_API void UpdateSlave(std::shared_ptr<LIN> lin, LdfParser& ldfFile, unsigned int linid, std::string signalName, double value);

/**
 * Sends a master request by schedule table ID.
 *
 * @param lin The LIN bus object.
 * @param ldfFile The LDF file object.
 * @param scheduleTableId The ID of the schedule table.
 */
SENDFUNCTION_API void SendMasterRequestByIdThread(std::shared_ptr<LIN> lin, LdfParser& ldfFile, unsigned int scheduleTableId);

/**
 * Sends LIN master requests by name.
 *
 * This function sends LIN master requests based on the provided table name.
 * It iterates through the schedule table associated with the given name and sends the corresponding frame IDs.
 * The function also includes a delay between each request based on the schedule's delay value.
 *
 * @param lin The LIN object used for communication.
 * @param ldfFile The LdfParser object containing the parsed LIN description file.
 * @param tableName The name of the schedule table to send requests from.
 */
SENDFUNCTION_API void SendMasterRequestByNameThread(std::shared_ptr<LIN> lin, LdfParser& ldfFile, std::string tableName);

// CinInputThread template functions
template<typename BUS_TYPE>
SENDFUNCTION_API void CinInputThreadCAN(BUS_TYPE bus, DbcParser& dbcFile, Payloads_CAN& encodedPayloads, SendFunc* sendFuncCAN);

SENDFUNCTION_API void CinInputThreadLIN(std::shared_ptr<LIN> lin, LdfParser& ldfFile, SendFunc* sendFuncLIN);

template<typename BUS_TYPE>
SENDFUNCTION_API void CinInputThreadBoth(BUS_TYPE bus, DbcParser& dbcFile, std::shared_ptr<LIN> lin, LdfParser& ldfFile,
 Payloads_CAN& encodedPayloads, SendFunc* sendFuncCAN, SendFunc* sendFuncLIN);

#endif // SENDFUNCTIONS_HPP