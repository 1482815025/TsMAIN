// #include <iostream>
// #include <sstream>
// #include <stdexcept>
#include <windows.h>
#include "sendFunctions.hpp"
#include "Getway.hpp"

// global variables
DbcParser dbcFile;
std::thread sendThreadCAN;
Payloads_CAN encodedPayloadsCAN;
Payloads_CAN filteredPayloads;
std::shared_ptr<CANFD> canfd;
std::shared_ptr<CANFD> canfd2;
Getway* getwayA = new Getway();
Getway* getwayB = new Getway();
XLcanFdConf fdParams;

int main() {
    memset(&fdParams, 0, sizeof(fdParams));
    // arbitration bitrate
    fdParams.arbitrationBitRate = 625000;
    fdParams.tseg1Abr = 101;
    fdParams.tseg2Abr = 26;
    fdParams.sjwAbr = 26;

    // data bitrate
    fdParams.dataBitRate = 1428571;
    fdParams.tseg1Dbr = 33;
    fdParams.tseg2Dbr = 22;
    fdParams.sjwDbr = 22;
	try{
		getwayA->setUnpassedMsgId({0x70, 0x76});
		getwayB->setOverWriteMsgId({0x510});
		HardwareInit();
		dbcFile.parse("SPA3");
		dbcFile.NodeMsgGenerator("HVBM", encodedPayloadsCAN);
		dbcFile.updateSignalValue(0x70, "signal", 3000, encodedPayloadsCAN);
		dbcFile.updateSignalValue(0x76, "signal", 10, encodedPayloadsCAN);
		for (auto payload : encodedPayloadsCAN.getPayloads()) {
			if (payload.id == 0x70 || payload.id == 0x76) {
				filteredPayloads.addPayload(payload);
			}
		}
		
		canfd = std::make_shared<CANFD>(3, fdParams, 0);
		canfd->CANFDGoOnBus();
		canfd->attachMonitor(getwayA);
		canfd2 = std::make_shared<CANFD>(4, fdParams, 1);
		canfd2->attachMonitor(getwayB);
		getwayA->setOutput(canfd2.get());
		getwayB->setOutput(canfd.get());
		sendThreadCAN = std::thread(SendCANFDEncodedPayloadsThread, canfd2, std::ref(encodedPayloadsCAN));

		while(true) {
			Sleep(10000);
		}

		canfd->GoOffBus();
		canfd2->GoOffBus();

	} catch (std::invalid_argument& err) {
		std::cerr << "[Exception catched] " << err.what() << '\n';
	}
	
	system("pause");
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

// 	system("pause");
// 	return 0;
// }