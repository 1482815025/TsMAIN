//
//  signal.hpp
//  LIN_Database_Encode_Decode_Tool
//
//  Created on: 06/14/2023
//      Author: Hao Zheng, Yifan Wang
//

#ifndef SIGNAL_LIN_H
#define SIGNAL_LIN_H

#include <map>
#include "signal_encoding_type.hpp"

constexpr int MAX_FRAME_LEN = 8;

class PARSER_API Signal_LIN {

public:

	// Getters
	std::string getName() const { return name; }
	int getstartBit() const { return startBit; }
	int getInitValue() const { return initValue; }
	int getSignalSize() const { return signalSize; }
	SignalEncodingType* getEncodingType() { return encodingType; }
	// Setters
	void setName(const std::string& name) { this->name = name; }
	void setStartBit(const int& startBit) { this->startBit = startBit; }
	void setInitValue(const int& initValue) { this->initValue = initValue; }
	void setSignalSize(const int& signalSize) { this->signalSize = signalSize; }
	void setPublisher(const std::string& publisher) { this->publisher = publisher; }
	void setEncodingType(SignalEncodingType* encodingType) { this->encodingType = encodingType; }
	void setSubscribers(const std::vector<std::string>& subscribers) { this->subscribers = subscribers; }
	// Decode/Encode
	uint64_t encodeSignal(double valueToEncode, bool isInitialValue);
	std::tuple<double, std::string, LinSigEncodingValueType>
		decodeSignal(unsigned char rawPayload[MAX_FRAME_LEN]);
	// Operator overload, allows parsing of signals info
	friend std::istream& operator>>(std::istream& in, Signal_LIN& sig);
	friend std::ostream& operator<<(std::ostream& os, const Signal_LIN& sig);

private:

	int startBit = -1; // Initialize with invalid value
	int initValue{};
	int signalSize{};
	std::string name{};
	std::string publisher{};
	SignalEncodingType* encodingType{};
	std::vector<std::string> subscribers;

};

#endif /* SIGNAL_LIN_H */
