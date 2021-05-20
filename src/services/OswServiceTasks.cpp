#include "services/OswServiceTasks.h"

#include "services/OswServiceTaskBLECompanion.h"
#include "services/OswServiceTaskExample.h"
#include "services/OswServiceTaskGPS.h"

// Define helpers - as some are just defines and do not represent a numeric value (which is needed for the service amount calc)
#ifdef GPS_EDITION
    # define SERVICE_GPS 1
#else
    # define SERVICE_GPS 0
#endif

namespace OswServiceAllTasks {
#if SERVICE_BLE_COMPANION == 1
OswServiceTaskBLECompanion bleCompanion;
#endif
#ifdef GPS_EDITION
OswServiceTaskGPS gps;
#endif
//OswServiceTaskExample example;
}  // namespace OswServiceAllTasks

const unsigned char oswServiceTasksCount = 0 + SERVICE_BLE_COMPANION + SERVICE_GPS;
OswServiceTask* oswServiceTasks[] = {
#if SERVICE_BLE_COMPANION == 1
    &OswServiceAllTasks::bleCompanion,
#endif
#ifdef GPS_EDITION
    &OswServiceAllTasks::gps,
#endif
    //&OswServiceAllTasks::example
};