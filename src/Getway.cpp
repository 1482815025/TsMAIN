#include "Getway.hpp"


void Getway::updateDIOPayload(unsigned int data) {
    if (Payloads != nullptr && dbc != nullptr) {
        if (data & XL_DAIO_PORT_MASK_DIGITAL_D0) {
            dbc->updateSignalValue(0x131, "EpbBtnCtrlSt", 4, *Payloads);
        }
        else if (!(data & XL_DAIO_PORT_MASK_DIGITAL_D0)) {
            dbc->updateSignalValue(0x131, "EpbBtnCtrlSt", 2, *Payloads);
        }
    }
}

void Getway::updateAINPayload(XL_IO_ANALOG_DATA data) {}