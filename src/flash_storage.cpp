//
// Created by Daniel on 22/2/2018.
//

#include "flash_storage.h"

FlashStorage::Data FlashStorage::data{};
libbase::k60::Flash* FlashStorage::flash_ptr = nullptr;

void FlashStorage::save() {
    Byte bytes[sizeof(Data)];
    memcpy(bytes, &data, sizeof(Data));
    flash_ptr->Write(bytes, sizeof(Data));
}

void FlashStorage::load() {
    Byte bytes[sizeof(Data)];
    flash_ptr->Read(bytes, sizeof(Data));
    memcpy(&data, bytes, sizeof(Data));
}
