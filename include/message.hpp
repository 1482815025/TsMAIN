/*
 *  message.hpp
 *
 *  Created on: 04/28/2023
 *      Author: Hao Zheng, Yifan Wang
 */

#ifndef MESSAGE_H
#define MESSAGE_H

#include <map>
#include "signal.hpp"

struct signalGroup {
    std::string groupName;
    unsigned short dataID;
    unsigned short startByte;
    unsigned short length;
    std::vector<std::string> signalNames;
    signalGroup() : length(0) {
        signalNames.reserve(10);
    }
};

class PARSER_API Message {
    
public:
    Message() {
        
        signalsLibrary.reserve(1000);
        signalGroups.reserve(10);
    }

    // Getter functions for all the possible data one can request from a Message
    unsigned long getId() const { return id; }
    unsigned int getDlc() const { return messageSize; }
    unsigned int getCycleTime() const { return cycleTime; }
    unsigned int getMsgSendType() const { return msgSendType; }
    std::string getName() const { return name; }
    std::string getSenderNames() const { return senderName; }
    std::unordered_map<std::string, Signal> getSignalsInfo() const { return signalsLibrary; }
    std::vector<signalGroup> getSigGroups() const { return signalGroups; }
    // Parse signal value descrption
    std::istream& parseSigInitialValue(std::istream& in);
    std::istream& parseSigDataID(std::istream& in);
    std::istream& parseSigFuncType(std::istream& in);
    std::istream& parseCycleTime(std::istream& in);
    std::istream& parseMsgSendType(std::istream& in);
    std::istream& parseSigValueDescription(std::istream& in);
    std::istream& parseAdditionalSigValueType(std::istream& in);
    std::istream& parseSigGroup(std::istream& in);
    // Used to encode/decode messages
    std::map<std::string, double> decode(
        unsigned char const rawPayload[],
        unsigned short const MAX_MSG_LE,
        unsigned int const dlc
    );
    unsigned int encode(
        std::vector<std::pair<std::string, double> >& signalsToEncode,
        unsigned char encodedPayload[],
        unsigned short const MAX_MSG_LE,
        double const defaultGlobalInitialValue
    );
    // Overload of operator>> to enable parsing of Messages from streams of DBC-Files
    friend std::istream& operator>>(std::istream& in, Message& msg);

private:

    typedef std::unordered_map<std::string, Signal>::iterator signalsLibrary_iterator;
    // Name of the Message
    std::string name{};
    // The CAN-ID assigned to this specific Message
    unsigned long id{};
    // The length of this message in Bytes. Allowed values are between 0 and 8
    unsigned int messageSize{};
    // String containing the name of the Sender of this Message if one exists in the DB
    std::string senderName{};
    // A hash table containing all Signals that are present in this Message <Signal name, Signal object>
    std::unordered_map<std::string, Signal> signalsLibrary{};
    // The cycle time of the message in ms
    unsigned int cycleTime{};
    // The message send type
    unsigned int msgSendType{};
    // signal group
    std::vector<signalGroup> signalGroups;

};
#endif
