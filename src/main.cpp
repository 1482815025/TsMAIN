/*----------------------------------------------------------------------------
| File        : main.cpp
|
| Description : SPA3 HVBM HVCAN Getway
|               0x70 && 0x76 HVBM Tx simulated
|-----------------------------------------------------------------------------
| Version     : 2.0
| Author      : Hao Zheng
| Date        : 2025/1/21
|---------------------------------------------------------------------------*/
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
		// getwayB->setOverWriteMsgId({0x510});
		HardwareInit();
		dbcFile.parse("GPAsystem_MAIN_196_HighVoltage1CANCfg_240514.dbc");
		dbcFile.NodeMsgGenerator("HVBM", encodedPayloadsCAN);
		dbcFile.updateSignalValue(0x76, "HvBattEgyAvlDcha4", 3000, encodedPayloadsCAN);
		dbcFile.updateSignalValue(0x70, "HvBattDchaPwrLimSafe1DchaPwr", 10, encodedPayloadsCAN);
		for (auto payload : encodedPayloadsCAN.getPayloads()) {
			if (payload.id == 0x70 || payload.id == 0x76) {
				filteredPayloads.addPayload(payload);
			}
		}
		// printf("filteredPayloads: \n");
		// for (auto payload : filteredPayloads.getPayloads()) {
		// 	printf("id: %d Payload: ", payload.id);
		// 	for (const auto& value : payload.payload) {
		// 		printf("%x ", value);
		// 	}
		// }
		// printf("\n");
		
		canfd = std::make_shared<CANFD>(3, fdParams, 0);
		canfd->CANFDGoOnBus();
		canfd->attachMonitor(getwayA);
		canfd2 = std::make_shared<CANFD>(4, fdParams, 1);
		canfd2->attachMonitor(getwayB);
		getwayA->setOutput(canfd2.get());
		getwayB->setOutput(canfd.get());
		sendThreadCAN = std::thread(SendCANFDEncodedPayloadsThread, canfd2, std::ref(filteredPayloads));

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
