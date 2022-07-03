#include "services/OswServiceTasks.h"
#ifdef OSW_FEATURE_BLE
#include "services/OswServiceTaskBLECompanion.h"
#endif
#include "services/OswServiceTaskExample.h"
#include "services/OswServiceTaskGPS.h"
#include "services/OswServiceTaskMemMonitor.h"
#ifdef OSW_FEATURE_WIFI
#include "services/OswServiceTaskWiFi.h"
#include "services/OswServiceTaskWebserver.h"
#endif
#include "osw_util.h"

namespace OswServiceAllTasks {
// OswServiceTaskExample example;
#ifdef OSW_FEATURE_BLE
OswServiceTaskBLECompanion bleCompanion;
#endif
#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)
OswServiceTaskGPS gps;
#endif
#ifdef OSW_FEATURE_WIFI
OswServiceTaskWiFi wifi;
OswServiceTaskWebserver webserver;
#endif
#ifndef OSW_EMULATOR
OswServiceTaskMemMonitor memory;
#endif
}  // namespace OswServiceAllTasks

OswServiceTask* oswServiceTasks[] = {
#ifdef OSW_FEATURE_BLE
    & OswServiceAllTasks::bleCompanion,
#endif
#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)

    & OswServiceAllTasks::gps,
#endif
//&OswServiceAllTasks::example,
#ifdef OSW_FEATURE_WIFI
    & OswServiceAllTasks::wifi, &OswServiceAllTasks::webserver,
#endif
#ifndef OSW_EMULATOR
#ifndef NDEBUG
    & OswServiceAllTasks::memory
#endif
#else
    nullptr // To prevent static array with size zero
#endif
};
const unsigned char oswServiceTasksCount = OswUtil::size(oswServiceTasks);
