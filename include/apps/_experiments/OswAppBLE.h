#pragma once

#include <osw_hal.h>
#include <OswAppV2.h>

class NimBLEServer;
class NimBLECharacteristic;

class OswAppBLE : public OswAppV2 {
  public:
    OswAppBLE();

    const char* getAppId() override;
    const char* getAppName() override;

    void onStart() override;
    void onLoop() override;
    void onDraw() override;
    void onStop() override;
    void onButton(Button id, bool up, ButtonStateNames state) override;

  private:
    NimBLEServer* pServer = NULL;
    NimBLECharacteristic* pCharacteristic1 = NULL;
    NimBLECharacteristic* pCharacteristic2 = NULL;
    NimBLECharacteristic* pCharacteristicBat = NULL;
    bool currentDeviceWantsNotify = false;
};
