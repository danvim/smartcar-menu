//
// Created by Daniel on 22/2/2018.
//

#include "flash_storage.h"

void FlashStorage::save() {
    auto *buff = reinterpret_cast<Byte*>(&data);
    flash_ptr->Write(buff, sizeof(Data));
}

void FlashStorage::load() {
    auto *buff = new Byte[sizeof(Data)];
    flash_ptr->Read(buff, sizeof(Data));
    memcpy(&data, buff, sizeof(Data));
}
