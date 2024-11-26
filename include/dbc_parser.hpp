/*
 *  dbc_parser.hpp
 *
 *  Created on: 04/28/2023
 *      Author: Hao Zheng, Yifan Wang
 */

#ifndef DBCPARSER_HPP
#define DBCPARSER_HPP

#include <mutex>
#include <shared_mutex>
#include "message.hpp"

constexpr unsigned short MAX_MSG_LEN_CAN = 8;
constexpr unsigned short MAX_MSG_LEN_CAN_FD = 64;
enum class BusType {
    NotSet,
    Unknown,
    CAN,
    CAN_FD
};

class PARSER_API Payload_CAN {
public:
    unsigned long id;
    unsigned long cycleTime;
    unsigned short int dlc;
    unsigned short int sendType;
    bool CRC = false;
    std::vector<unsigned char> payload;
    // use to store chks information.
    //1. Start_byte_index    2. Protected_Data_Length   3. DataID
    std::vector<std::vector<unsigned short>> CRCs;
    
    Payload_CAN() {
        payload.reserve(100);
        CRCs.reserve(100);
    }
};


class PARSER_API Payloads_CAN {
public:
    Payloads_CAN() {
        payloads.reserve(1000);
    }

    void addPayload(Payload_CAN);
    void updatePayload(unsigned long id, unsigned char newValues[], size_t newSize);
    std::vector<Payload_CAN> getPayloads();
private:
    mutable std::shared_mutex       mtxPayloads;
    // std::vector<unsigned long> id_cycleTime_sendtype_Dlc, std::vector<unsigned char> payload
    std::vector<Payload_CAN> payloads;
};

class PARSER_API DbcParser : public dbParser {
public:
    DbcParser() {
        messageLibrary.reserve(1000);
        messagesInfo.reserve(1000);
        NodeSignals.reserve(1000);
    }

    unsigned int Baudrate = 0;
    unsigned int BaudRateCANFD = 0;

    // Construct using either a File or a Stream of a DBC-File
    // A bool is used to indicate whether parsing succeeds or not
    bool parse(const std::string& filePath) override;
    // Decode
    std::map<std::string, double> decode(
        unsigned long msgId,
        unsigned int msgSize,
        unsigned char payLoad[]
    );
    // Encode
    unsigned int encode(
        unsigned long msgId,
        std::vector<std::pair<std::string, double> >& signalsToEncode,
        unsigned char encodedPayload[],
        unsigned int encodedPayloadSize
    );
    // Print DBC Info
    friend std::ostream& operator<<(std::ostream& os, const DbcParser& dbcFile);
    std::unordered_map<unsigned long, Message> getMessagesInfo()  { return messageLibrary; }

    /**
     * @brief Generate messages for a specific node in the DBC file.
     * 
     * This function generates messages for a specific node in the DBC file.
     * It takes the node name and the encoded payloads object as parameters.
     * 
     * @param Node The name of the node.
     * @param encodedPayloads The encoded payloads object to update.
     */
    void NodeMsgGenerator(std::string Node, Payloads_CAN& encodedPayloads);
    BusType databaseBusType = BusType::NotSet; // CAN or CAN FD
    unsigned short MAX_MSG_LEN;


    /**
     * @brief Update the value of a signal in the DBC file.
     * 
     * This function updates the value of a signal in the DBC file.
     * It takes the Message ID, signal name, new value, and the encoded payloads as parameters.
     * 
     * @param MessageId The ID of the message containing the signal.
     * @param signalName The name of the signal to update.
     * @param newValue The new value for the signal.
     * @param encodedPayloads The encoded payloads object to update.
     */
    void updateSignalValue(unsigned long MessageId, std::string signalName, double newValue, Payloads_CAN& encodedPayloads);

private:

    typedef std::unordered_map<unsigned long, Message>::iterator messageLibrary_iterator;
    bool isEmptyLibrary = true; // A bool to indicate whether DBC file has been loaded or not
    double sigGlobalInitialValue; // BA_DEF_DEF_  "GenSigStartValue"
    double sigGlobalInitialValueMin; // BA_DEF_ SG_  "GenSigStartValue"
    double sigGlobalInitialValueMax; // BA_DEF_ SG_  "GenSigStartValue"
    // Stores all info of messages. <Message id, Message object>
    std::unordered_map<unsigned long, Message> messageLibrary;
    // Contains all the messages which got parsed from the DBC-File
    std::vector<Message*> messagesInfo;
    // Function used to parse DBC file
    void loadAndParseFromFile(std::istream& in);
    void consistencyCheck();
    std::vector<std::pair<unsigned long,std::vector<std::pair<std::string, double>>>> NodeSignals;

};

#endif // DBCPARSER_HPP