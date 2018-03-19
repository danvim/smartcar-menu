//
// Created by Daniel on 22/2/2018.
//

#include "flash_storage.h"

FlashStorage::Data FlashStorage::data{};
libbase::k60::Flash* FlashStorage::flash_ptr = nullptr;

void FlashStorage::save() {
    flash_ptr->Write(&data, sizeof(Data));
}

void FlashStorage::load() {
    flash_ptr->Read(&data, sizeof(Data));
}
