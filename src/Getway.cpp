#include "Getway.hpp"

XLcanTxEvent canfdMsg1;

Getway::Getway() {
    this->outputCANFD = nullptr;
    this->unpassedMsgId = {};
    this->overWriteMsgId = {};
}

Getway::~Getway() {
    this->outputCANFD = nullptr;
    this->unpassedMsgId = {};
    this->overWriteMsgId = {};
}

void Getway::updatePayload(std::pair<std::vector<unsigned long>, std::string> id_length_dir, unsigned char* payload) {
    if (outputCANFD != nullptr && id_length_dir.second != "Tx") {
        // 检查 unpassedMsgId 中是否存在 id_length_dir.first[0]
        for (auto MsgId : unpassedMsgId) {
            if (MsgId == id_length_dir.first[0]) {
                return;  // 如果存在，则直接返回
            }
        }

        // 如果在 overWriteMsgId 中找到匹配的 MsgId，则覆盖 payload 内容
        if (overWriteMsgId.size() > 0) {
            for (auto MsgId : overWriteMsgId) {
                if (MsgId == id_length_dir.first[0]) {
                    // 使用 memset 或 memcpy 来覆盖 payload 内容
                    memset(payload, 0xFF, id_length_dir.first[1]);  // 填充为 0xFF
                    break;  // 找到后立即跳出循环，避免重复处理
                }
            }
        }

        // 清零 canfdMsg1 内容
        memset(&canfdMsg1, 0, sizeof(canfdMsg1));

        // 设置 canfdMsg1 的各项字段
        canfdMsg1.tag = XL_CAN_EV_TAG_TX_MSG;
        canfdMsg1.tagData.canMsg.canId = id_length_dir.first[0];
        canfdMsg1.tagData.canMsg.dlc = getDlcFromDataLength(id_length_dir.first[1]);
        canfdMsg1.tagData.canMsg.msgFlags = XL_CAN_TXMSG_FLAG_EDL | XL_CAN_TXMSG_FLAG_BRS;

        // 复制数据到 canfdMsg1.data
        for (unsigned int j = 0; j < id_length_dir.first[1]; j++) {
            canfdMsg1.tagData.canMsg.data[j] = payload[j];
        }

        // 发送数据
        outputCANFD->CANFDSend(&canfdMsg1, 1);
    }
}
