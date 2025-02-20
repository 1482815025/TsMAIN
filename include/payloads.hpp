/*
 *  payloads.hpp
 *
 *  Created on: 07/28/2024
 *      Author: Hao Zheng
 */

#ifndef payloads_hpp
#define payloads_hpp

#include <string>
#include <vector>
#include <iostream>
#include "_dllspec.hpp"

class PARSER_API Payload_LIN {

public:

	std::string frameName;
	unsigned int frameId;
	unsigned int dlc;
	std::vector<unsigned char> getPayload() const { return payload; }
	void setPayload(unsigned char values[], size_t size = 8);
	void printPayload();
	
private:

	std::vector<unsigned char> payload;

};

class PARSER_API Payloads_LIN {

public:

    Payloads_LIN() : sender(""), payloads() {
        payloads.reserve(1000);
    }
	std::string sender;
	std::vector<Payload_LIN> getPayloads() const {
		return payloads;
	};
	void addPayload(Payload_LIN& _payload);
	void updatePayload(unsigned int frameId, unsigned char newValues[], size_t size = 8);

private:

	std::vector<Payload_LIN> payloads;

};

#endif /* payloads_hpp */