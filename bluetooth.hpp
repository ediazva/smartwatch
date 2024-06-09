#pragma once

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <blepp/blestatemachine.h>

static inline bool is_bluetooth_open() {
   return hci_get_route(nullptr) != -1;
}