#include <iostream>
#include <sstream>
#include <stdexcept>
#include <thread>
#include <windows.h>
#include "sendFunctions.hpp"
#include "readConfig.hpp"

// global variables
DbcParser dbcFile;
LdfParser ldfFile;
SendFunc* monitorCAN = new SendFunc();
SendFunc* monitorLIN = new SendFunc();
std::thread sendThreadCAN;
std::thread sendThreadLIN;
std::thread inputThread;
Payloads_CAN encodedPayloadsCAN;
Payloads_LIN encodedPayloadsLIN;
std::shared_ptr<CANFD> canfd;
std::shared_ptr<CAN> can;
std::shared_ptr<LIN> lin;
Config test_config;

int main() {
    if (!readIniValue(test_config)) {
        std::cerr << "Error reading config.ini file." << std::endl;
        return 1;
    }

	if (!test_config.dbc_file.empty()) {
		dbcFile.parse(test_config.dbc_file);
		monitorCAN->setDB(&dbcFile);
		if (dbcFile.databaseBusType == BusType::CAN_FD) {
			if (!test_config.dbc_node.empty()) {
				dbcFile.NodeMsgGenerator(test_config.dbc_node, encodedPayloadsCAN);
				if (!test_config.can_signals.empty()) {
					for (auto& signal : test_config.can_signals) {
						dbcFile.updateSignalValue(std::get<0>(signal), std::get<1>(signal), std::get<2>(signal), encodedPayloadsCAN);
					}
				}
			}
			canfd = std::make_shared<CANFD>(500000, 2000000);
			canfd->attach(monitorCAN);
			sendThreadCAN = std::thread(SendCANFDEncodedPayloadsThread, canfd, std::ref(encodedPayloadsCAN));
		}
		else if (dbcFile.databaseBusType == BusType::CAN) {
			if (!test_config.dbc_node.empty()) {
				dbcFile.NodeMsgGenerator(test_config.dbc_node, encodedPayloadsCAN);
				if (!test_config.can_signals.empty()) {
					for (auto& signal : test_config.can_signals) {
						dbcFile.updateSignalValue(std::get<0>(signal), std::get<1>(signal), std::get<2>(signal), encodedPayloadsCAN);
					}
				}
			}
			can = std::make_shared<CAN>(500000);
			can->attach(monitorCAN);
			sendThreadCAN = std::thread(SendCANEncodedPayloadsThread, can, std::ref(encodedPayloadsCAN));
		}
		else {
			std::cout << "Invalid bus type." << std::endl;
			return 1;
		}
	}

	if (!test_config.ldf_file.empty()) {
		ldfFile.parse(test_config.ldf_file);
		monitorLIN->setDB(&ldfFile);
		unsigned int linSpeed = ldfFile.getLinSpeed()*1000;
		if (test_config.lin_master && !test_config.ldf_node.empty()) {
			ldfFile.payloadsGenerator(test_config.ldf_node, encodedPayloadsLIN);
			lin = std::make_shared<LIN>(linSpeed, XL_LIN_VERSION_2_1, MASTER);
			lin->attach(monitorLIN);
			SetSlaveAndCreateRxThread(lin, encodedPayloadsLIN);
			if (!test_config.lin_signals.empty()) {
				unsigned char updatePayload[100];
				for (auto& signal : test_config.lin_signals) {
					ldfFile.updateSignalValue(std::get<0>(signal), std::get<1>(signal), std::get<2>(signal), updatePayload);
					unsigned int dlc = ldfFile.getFramesLibrary().find(std::get<0>(signal))->second.getDlc();
					lin->linSetSlave(std::get<0>(signal), updatePayload, dlc);
				}
			}
			if (test_config.routinTableId >= 0)
				sendThreadLIN = std::thread(SendMasterRequestByIdThread, lin, std::ref(ldfFile), test_config.routinTableId);
			else {
				sendThreadLIN = std::thread(SendMasterRequestByIdThread, lin, std::ref(ldfFile), 1);
			}
		}
		else if (!test_config.lin_master) {
			ldfFile.payloadsGenerator(test_config.ldf_node, encodedPayloadsLIN);
			lin = std::make_shared<LIN>(linSpeed, XL_LIN_VERSION_2_1, SLAVE);
			lin->attach(monitorLIN);
			SetSlaveAndCreateRxThread(lin, encodedPayloadsLIN);
			if (!test_config.lin_signals.empty()) {
				unsigned char updatePayload[100];
				for (auto& signal : test_config.lin_signals) {
					ldfFile.updateSignalValue(std::get<0>(signal), std::get<1>(signal), std::get<2>(signal), updatePayload);
					unsigned int dlc = ldfFile.getFramesLibrary().find(std::get<0>(signal))->second.getDlc();
					lin->linSetSlave(std::get<0>(signal), updatePayload, dlc);
				}
			}
		}
		else {
			std::cout << "Invalid configuration." << std::endl;
			return 1;
		}
	}
	
	if (!test_config.dbc_file.empty() && !test_config.ldf_file.empty()) {
		if (dbcFile.databaseBusType == BusType::CAN_FD) {
			inputThread = std::thread(CinInputThreadBoth<std::shared_ptr<CANFD>>, canfd, std::ref(dbcFile), lin, std::ref(ldfFile), std::ref(encodedPayloadsCAN), monitorCAN, monitorLIN);
		}
		else {
			inputThread = std::thread(CinInputThreadBoth<std::shared_ptr<CAN>>, can, std::ref(dbcFile), lin, std::ref(ldfFile), std::ref(encodedPayloadsCAN), monitorCAN, monitorLIN);
		}
	}
	else if (!test_config.dbc_file.empty() && test_config.ldf_file.empty() && dbcFile.databaseBusType == BusType::CAN_FD) {
		inputThread = std::thread(CinInputThreadCAN<std::shared_ptr<CANFD>>, canfd, std::ref(dbcFile), std::ref(encodedPayloadsCAN), monitorCAN);
	}
	else if (!test_config.dbc_file.empty() && test_config.ldf_file.empty() && dbcFile.databaseBusType == BusType::CAN) {
		inputThread = std::thread(CinInputThreadCAN<std::shared_ptr<CAN>>, can, std::ref(dbcFile), std::ref(encodedPayloadsCAN), monitorCAN);
	}
	else if (test_config.dbc_file.empty() && !test_config.ldf_file.empty()) {
		inputThread = std::thread(CinInputThreadLIN, lin, std::ref(ldfFile), monitorLIN);
	}
	else {
		std::cout << "Invalid configuration." << std::endl;
	}

	if (sendThreadCAN.joinable()) {
		sendThreadCAN.join();
	}
	if (sendThreadLIN.joinable()) {
		sendThreadLIN.join();
		lin->GoOffBus();
	}
	if (inputThread.joinable()) {
		inputThread.join();
	}

	system("pause");
	return 0;
}