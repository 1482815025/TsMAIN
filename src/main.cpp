// #include <iostream>
// #include <sstream>
// #include <stdexcept>
#include <windows.h>
#include "ldf_parser.hpp"
#include "Getway.hpp"
// #include "readConfig.hpp"

// global variables
DbcParser dbcFile;
Getway* GetwayA = new Getway();
std::thread sendThreadCAN;
static XLcanTxEvent canfdMsg[100];
const unsigned int DIO_ALL = 255U;
bool TsMAINLogFlag = false;
XLdaioSetPort    portCfg;
XLcanFdConf fdParams;


int main() {
	try {
		memset(&portCfg, 0x00, sizeof(portCfg));  //necessary to set reserved bits to zero
		memset(&fdParams, 0, sizeof(fdParams));

		fdParams.arbitrationBitRate = 500000;
		fdParams.tseg1Abr = 55;
		fdParams.tseg2Abr = 24;
		fdParams.sjwAbr = 2;
		fdParams.dataBitRate = 2000000;
		fdParams.tseg1Dbr = 29;
		fdParams.tseg2Dbr = 10;
		fdParams.sjwDbr = 10;

		portCfg.portType = XL_DAIO_PORT_TYPE_MASK_DIGITAL;
		portCfg.portMask = DIO_ALL;
		portCfg.portFunction[0] = XL_DAIO_PORT_DIGITAL_IN;
		portCfg.portFunction[1] = XL_DAIO_PORT_DIGITAL_IN;
		portCfg.portFunction[2] = XL_DAIO_PORT_DIGITAL_IN;
		portCfg.portFunction[3] = XL_DAIO_PORT_DIGITAL_IN;
		portCfg.portFunction[4] = XL_DAIO_PORT_DIGITAL_IN;
		portCfg.portFunction[5] = XL_DAIO_PORT_DIGITAL_IN;
		portCfg.portFunction[6] = XL_DAIO_PORT_DIGITAL_IN;
		portCfg.portFunction[7] = XL_DAIO_PORT_DIGITAL_IN;
	

		canfdMsg[0].tag = XL_CAN_EV_TAG_TX_MSG;
		canfdMsg[0].tagData.canMsg.canId = 0x123;
		canfdMsg[0].tagData.canMsg.msgFlags = XL_CAN_TXMSG_FLAG_EDL | XL_CAN_TXMSG_FLAG_BRS;
		canfdMsg[0].tagData.canMsg.dlc = 8;
		canfdMsg[0].tagData.canMsg.data[0] = 0x01;
		canfdMsg[0].tagData.canMsg.data[1] = 0x01;
		canfdMsg[0].tagData.canMsg.data[2] = 0x01;
		canfdMsg[0].tagData.canMsg.data[3] = 0x01;
		canfdMsg[0].tagData.canMsg.data[4] = 0x01;
		canfdMsg[0].tagData.canMsg.data[5] = 0x01;


		HardwareInit();
		dbcFile.parse("BrakesystemSPA2_MAIN_19_BCMA2CAN_230705.dbc");
		auto encodedPayloadsCAN = std::make_shared<Payloads_CAN>();
		dbcFile.NodeMsgGenerator("BCMA", *encodedPayloadsCAN);
		GetwayA->setDbcParser(&dbcFile);
		GetwayA->setPayloads(encodedPayloadsCAN.get());
		auto canfd = std::make_shared<CANFD>(12, fdParams, 0);
		auto daio = std::make_shared<DAIO>(portCfg, 20, 1);
		canfd->CANFDGoOnBus();
		daio->GoOnBus();
		daio->attachMonitor(GetwayA);
		
		sendThreadCAN = std::thread(SendCANFDEncodedPayloadsThread, canfd, encodedPayloadsCAN);
		while (1) {
			Sleep(20);
			canfd->CANFDSend(canfdMsg,1);
		}
	

	} catch (std::exception& e) {
		std::cout << "Error: " << e.what() << std::endl;
	}

	system("pause");
	return 0;
}