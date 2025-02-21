#include "Getway.hpp"


void Getway::updateDIOPayload(unsigned int data) {
    printf("\n------------------------------------------------------------------------------\n");
    printf(" - Digital Port: DIO7|DIO6|DIO5|DIO4|DIO3|DI2|DIO1|DIO0|(value)\n");
    printf("                    %d|   %d|   %d|   %d|   %d|  %d|   %d|   %d| (%d)\n\n",
        (data & 128) >> 7,
        (data & 64) >> 6,
        (data & 32) >> 5,
        (data & 16) >> 4,
        (data & 8) >> 3,
        (data & 4) >> 2,
        (data & 2) >> 1,
        (data & 1),
        data);
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