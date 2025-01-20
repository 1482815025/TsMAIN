// #include <iostream>
// #include <sstream>
// #include <stdexcept>
#include <windows.h>
#include "ldf_parser.hpp"
#include "sendFunctions.hpp"
#include "readConfig.hpp"

// global variables
DbcParser dbcFile;
LdfParser LDFfile;
Monitor* monitorCAN = new Monitor();
Monitor* monitorLIN = new Monitor();
std::thread sendThreadCAN;
std::thread sendThreadLIN;
std::thread inputThread;
Payloads_CAN encodedPayloadsCAN;
Payloads_LIN encodedPayloadsLIN;
std::shared_ptr<CANFD> canfd;
std::shared_ptr<LIN> lin;

Config test_config;
bool TsMAINLogFlag = true;

int main() {

    if (!readIniValue(test_config)) {
        std::cerr << "Error reading config.ini file." << std::endl;
        return 1;
    }

		HardwareInit();
		dbcFile.parse(test_config.dbc_file);
		monitorCAN->setDB(&dbcFile);
		dbcFile.NodeMsgGenerator(test_config.dbc_node, encodedPayloadsCAN);
		for (auto& signal : test_config.can_signals) {
			dbcFile.updateSignalValue(std::get<0>(signal), std::get<1>(signal), std::get<2>(signal), encodedPayloadsCAN);
		}
		canfd = std::make_shared<CANFD>(3,0,dbcFile.Baudrate, dbcFile.BaudRateCANFD, TsMAINLogFlag);
		canfd->attachMonitor(monitorCAN);
		canfd->CANFDGoOnBus();
		
		sendThreadCAN = std::thread(SendCANFDEncodedPayloadsThread, canfd, std::ref(encodedPayloadsCAN));
		LDFfile.parse(test_config.ldf_file);
		monitorLIN->setDB(&LDFfile);
		unsigned int linSpeed = LDFfile.getLinSpeed()*1000;

		LDFfile.payloadsGenerator(test_config.ldf_node, encodedPayloadsLIN);
		lin = std::make_shared<LIN>(1,0,linSpeed, XL_LIN_VERSION_2_1, MASTER, TsMAINLogFlag);

		lin->attachMonitor(monitorLIN);
		unsigned char* p;
		for (auto& payload : encodedPayloadsLIN.getPayloads()) {
			p = payload.getPayload().data();
			lin->linSetSlave(payload.frameId, p, payload.dlc);
		}
		lin->LINGoOnBus();
		sendThreadLIN = std::thread(SendMasterRequestByIdThread, lin, std::ref(LDFfile), 1);
		Sleep(1000);
		inputThread = std::thread(CinInputThreadBothFD, canfd, std::ref(dbcFile), lin, std::ref(LDFfile), std::ref(encodedPayloadsCAN), monitorCAN, monitorLIN);
		
		if (inputThread.joinable()) {
			inputThread.join();
			LogTask->waitForAllTasks();
			canfd->GoOffBus();
			lin->GoOffBus();
			Sleep(100);
		}




	// if (!test_config.dbc_file.empty()) {
	// 	dbcFile.parse(test_config.dbc_file);
	// 	monitorCAN->setDB(&dbcFile);
	// 	if (dbcFile.databaseBusType == BusType::CAN_FD) {
	// 		if (!test_config.dbc_node.empty()) {
	// 			dbcFile.NodeMsgGenerator(test_config.dbc_node, encodedPayloadsCAN);
	// 			if (!test_config.can_signals.empty()) {
	// 				for (auto& signal : test_config.can_signals) {
	// 					dbcFile.updateSignalValue(std::get<0>(signal), std::get<1>(signal), std::get<2>(signal), encodedPayloadsCAN);
	// 				}
	// 			}
	// 		}
	// 		canfd = std::make_shared<CANFD>(3,0,dbcFile.Baudrate, dbcFile.BaudRateCANFD, TsMAINLogFlag);
	// 		canfd->attachMonitor(monitorCAN);
	// 		canfd->CANFDGoOnBus();
	// 		sendThreadCAN = std::thread(SendCANFDEncodedPayloadsThread, canfd, std::ref(encodedPayloadsCAN));
	// 	}
	// 	else if (dbcFile.databaseBusType == BusType::CAN) {
	// 		if (!test_config.dbc_node.empty()) {
	// 			dbcFile.NodeMsgGenerator(test_config.dbc_node, encodedPayloadsCAN);
	// 			if (!test_config.can_signals.empty()) {
	// 				for (auto& signal : test_config.can_signals) {
	// 					dbcFile.updateSignalValue(std::get<0>(signal), std::get<1>(signal), std::get<2>(signal), encodedPayloadsCAN);
	// 				}
	// 			}
	// 		}
	// 		can = std::make_shared<CAN>(3,0,dbcFile.Baudrate, TsMAINLogFlag);
	// 		can->attachMonitor(monitorCAN);
	// 		can->CANGoOnBus();
	// 		sendThreadCAN = std::thread(SendCANEncodedPayloadsThread, can, std::ref(encodedPayloadsCAN));
			
	// 	}
	// 	else {
	// 		std::cout << "Invalid bus type." << std::endl;
	// 		return 1;
	// 	}
	// }
	// if (!test_config.ldf_file.empty()) {
	// 	LDFfile.parse(test_config.ldf_file);
	// 	monitorLIN->setDB(&LDFfile);
	// 	unsigned int linSpeed = LDFfile.getLinSpeed()*1000;
	// 	if (test_config.lin_master && !test_config.ldf_node.empty()) {
	// 		LDFfile.payloadsGenerator(test_config.ldf_node, encodedPayloadsLIN);
	// 		lin = std::make_shared<LIN>(1,0,linSpeed, XL_LIN_VERSION_2_1, MASTER, TsMAINLogFlag);
	// 		lin->attachMonitor(monitorLIN);
	// 		// SetSlaveAndCreateRxThread(lin, encodedPayloadsLIN);
	// 		unsigned char* p;
	// 		for (auto& payload : encodedPayloadsLIN.getPayloads()) {
	// 			p = payload.getPayload().data();
	// 			lin->linSetSlave(payload.frameId, p, payload.dlc);
	// 		}
	// 		if (!test_config.lin_signals.empty()) {
	// 			unsigned char updatePayload[100];
	// 			for (auto& signal : test_config.lin_signals) {
	// 				LDFfile.updateSignalValue(std::get<0>(signal), std::get<1>(signal), std::get<2>(signal), updatePayload);
	// 				unsigned int dlc = LDFfile.getFramesLibrary().find(std::get<0>(signal))->second.getDlc();
	// 				lin->linSetSlave(std::get<0>(signal), updatePayload, dlc);
	// 			}
	// 		}
	// 		if (test_config.routinTableId >= 0)
	// 			sendThreadLIN = std::thread(SendMasterRequestByIdThread, lin, std::ref(LDFfile), test_config.routinTableId);
	// 		else {
	// 			sendThreadLIN = std::thread(SendMasterRequestByIdThread, lin, std::ref(LDFfile), 1);
	// 		}
	// 	}
	// 	else if (!test_config.lin_master) {
	// 		LDFfile.payloadsGenerator(test_config.ldf_node, encodedPayloadsLIN);
	// 		lin = std::make_shared<LIN>(linSpeed, XL_LIN_VERSION_2_1, SLAVE, TsMAINLogFlag);
	// 		lin->attachMonitor(monitorLIN);
	// 		SetSlaveAndCreateRxThread(lin, encodedPayloadsLIN);
	// 		if (!test_config.lin_signals.empty()) {
	// 			unsigned char updatePayload[100];
	// 			for (auto& signal : test_config.lin_signals) {
	// 				LDFfile.updateSignalValue(std::get<0>(signal), std::get<1>(signal), std::get<2>(signal), updatePayload);
	// 				unsigned int dlc = LDFfile.getFramesLibrary().find(std::get<0>(signal))->second.getDlc();
	// 				lin->linSetSlave(std::get<0>(signal), updatePayload, dlc);
	// 			}
	// 		}
	// 	}
	// 	else {
	// 		std::cout << "Invalid configuration." << std::endl;
	// 		return 1;
	// 	}
	// }
	
	// if (!test_config.dbc_file.empty() && !test_config.ldf_file.empty()) {
	// 	if (dbcFile.databaseBusType == BusType::CAN_FD) {
	// 		inputThread = std::thread(CinInputThreadBoth<std::shared_ptr<CANFD>>, canfd, std::ref(dbcFile), lin, std::ref(LDFfile), std::ref(encodedPayloadsCAN), monitorCAN, monitorLIN);
	// 	}
	// 	else {
	// 		inputThread = std::thread(CinInputThreadBoth<std::shared_ptr<CAN>>, can, std::ref(dbcFile), lin, std::ref(LDFfile), std::ref(encodedPayloadsCAN), monitorCAN, monitorLIN);
	// 	}
	// }
	// else if (!test_config.dbc_file.empty() && test_config.ldf_file.empty() && dbcFile.databaseBusType == BusType::CAN_FD) {
	// 	inputThread = std::thread(CinInputThreadCAN<std::shared_ptr<CANFD>>, canfd, std::ref(dbcFile), std::ref(encodedPayloadsCAN), monitorCAN);
	// }
	// else if (!test_config.dbc_file.empty() && test_config.ldf_file.empty() && dbcFile.databaseBusType == BusType::CAN) {
	// 	inputThread = std::thread(CinInputThreadCAN<std::shared_ptr<CAN>>, can, std::ref(dbcFile), std::ref(encodedPayloadsCAN), monitorCAN);
	// }
	// else if (test_config.dbc_file.empty() && !test_config.ldf_file.empty()) {
	// 	inputThread = std::thread(CinInputThreadLIN, lin, std::ref(LDFfile), monitorLIN);
	// }
	// else {
	// 	std::cout << "Invalid configuration." << std::endl;
	// }

	system("pause");
	return 0;
}