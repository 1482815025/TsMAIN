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
	void setPayload(unsigned char values[], size_t size = 8) {
		std::vector<unsigned char> temp(values, values + size);
		this->payload = temp;
	}
	void printPayload() {
		for (unsigned int i = 0; i < dlc; i++) {
			std::cout << std::hex << int(payload[i]) << " ";
		}
		std::cout << std::dec <<std::endl;
	}
	
private:

	std::vector<unsigned char> payload;

};

class Payloads_LIN {

public:

	std::string sender;
	std::vector<Payload_LIN> getPayloads() const { return payloads; }
	void addPayload(Payload_LIN& _payload) { this->payloads.push_back(_payload); }
	void updatePayload(unsigned int frameId, unsigned char newValues[], size_t size = 8) {
		bool found = false;
		for (auto& payload : payloads) {
			if (payload.frameId == frameId) {
				payload.setPayload(newValues, size);
				found = true;
			}
		}
		if (!found) {
			std::cerr << "Frame with id "<< frameId <<" not found in the payloads" << std::endl;
		}
	}

private:

	std::vector<Payload_LIN> payloads;

};



#endif /* payloads_hpp */