#include <gfx_util.h>
#include <osw_hal.h>
#include <osw_ui.h>

#include "apps/_experiments/OswAppBLE.h"

#define CONFIG_BT_NIMBLE_ROLE_PERIPHERAL
#include <NimBLEDevice.h>
// BLE_STORE_MAX_BONDS


bool currentDeviceWantsNotify = false;
bool deviceConnected = false;

#define SERVICE_UUID            "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID_1   "beb5483e-36e1-4688-b7f5-aaaaaaaaaaaa"
#define CHARACTERISTIC_UUID_2   "beb5483e-36e1-4688-b7f5-bbbbbbbbbbbb"

#define BATTERY_SERVICE_UUID "0000180F-0000-1000-8000-00805f9b34fb"
#define BATTERY_LEVEL_CHARACTERISTIC_UUID "00002A19-0000-1000-8000-00805f9b34fb"

/**  None of these are required as they will be handled by the library with defaults. **
 **                       Remove as you see fit for your needs                        */
class MyServerCallbacks: public NimBLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
        deviceConnected = false;
    }
    /***************** New - Security handled here ********************
    ****** Note: these are the same return values as defaults ********/
    uint32_t onPassKeyRequest() {
        Serial.println("Server PassKeyRequest");
        return 123456;
    }

    bool onConfirmPIN(uint32_t pass_key) {
        Serial.print("The passkey YES/NO number: ");
        Serial.println(pass_key);
        return true;
    }

    void onAuthenticationComplete(ble_gap_conn_desc desc) {
        Serial.println("Starting BLE work!");
    }
    /*******************************************************************/
};

/** Handler class for characteristic actions */
class CharacteristicCallbacks: public NimBLECharacteristicCallbacks {
    void onRead(NimBLECharacteristic* pCharacteristic) {
        Serial.print(pCharacteristic->getUUID().toString().c_str());
        Serial.print(": onRead(), value: ");
        Serial.println(pCharacteristic->getValue().c_str());
    };

    void onWrite(NimBLECharacteristic* pCharacteristic) {
        Serial.print(pCharacteristic->getUUID().toString().c_str());
        Serial.print(": onWrite(), value: ");
        Serial.println(pCharacteristic->getValue().c_str());
    };
    /** Called before notification or indication is sent,
     *  the value can be changed here before sending if desired.
     */
    void onNotify(NimBLECharacteristic* pCharacteristic) {
        //Serial.println("Sending notification to clients");
    };


    /** The status returned in status is defined in NimBLECharacteristic.h.
     *  The value returned in code is the NimBLE host return code.
     */
    void onStatus(NimBLECharacteristic* pCharacteristic, Status status, int code) {
        // String str = ("Notification/Indication status code: ");
        // str += status;
        // str += ", return code: ";
        // str += code;
        // str += ", ";
        // str += NimBLEUtils::returnCodeToString(code);
        // Serial.println(str);
    };

    void onSubscribe(NimBLECharacteristic* pCharacteristic, ble_gap_conn_desc* desc, uint16_t subValue) {
        String str = "Client ID: ";
        str += desc->conn_handle;
        str += " Address: ";
        str += std::string(NimBLEAddress(desc->peer_ota_addr)).c_str();
        if(subValue == 0) {
            str += " Unsubscribed to ";
        } else if(subValue == 1) {
            str += " Subscribed to notfications for ";
            currentDeviceWantsNotify = true;
        } else if(subValue == 2) {
            str += " Subscribed to indications for ";
            currentDeviceWantsNotify = false;
        } else if(subValue == 3) {
            str += " Subscribed to notifications and indications for ";
        }
        str += std::string(pCharacteristic->getUUID()).c_str();

        Serial.println(str);
    };
};

/** Handler class for characteristic actions */
class BatteryCharacteristicCallbacks: public NimBLECharacteristicCallbacks {
    void onRead(NimBLECharacteristic* pCharacteristic) {
        Serial.print(pCharacteristic->getUUID().toString().c_str());
        Serial.print(": onRead(), value: ");
        Serial.println(pCharacteristic->getValue().c_str());
        static uint8_t batLvlByte = 69;
        pCharacteristic->setValue(&batLvlByte, 1);
    };
};

static CharacteristicCallbacks chrCallbacks1;
static CharacteristicCallbacks chrCallbacks2;
static BatteryCharacteristicCallbacks chrCallbacksBat;




OswAppBLE::OswAppBLE() {

}

const char* OswAppBLE::getAppId() {
    return "osw.exp.ble";
}

const char* OswAppBLE::getAppName() {
    return "BLE TEST";
}

void OswAppBLE::onStart() {
    OswAppV2::onStart(); // always make sure to call the base class method!
    // This is where you initialise code, gets called before this app is shown

    this->viewFlags = (OswAppV2::ViewFlags) (this->viewFlags | OswAppV2::ViewFlags::KEEP_DISPLAY_ON); // by default we want to keep the display on

    NimBLEDevice::init("OSW-BLE");
    NimBLEDevice::setSecurityAuth(true, false, false);
    // NimBLEDevice::setSecurityIOCap(BLE_HS_IO_DISPLAY_YESNO);

    // Create the BLE Server
    pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());
    Serial.println(String("Free Heap at line ") + __LINE__ + ": " + ESP.getFreeHeap() + "B");

    // Create the BLE Service
    BLEService* pService = pServer->createService(SERVICE_UUID);
    Serial.println(String("Free Heap at line ") + __LINE__ + ": " + ESP.getFreeHeap() + "B");

    // Create a BLE Characteristic
    pCharacteristic1 = pService->createCharacteristic(
                           CHARACTERISTIC_UUID_1,
                           /******* Enum Type NIMBLE_PROPERTY now *******
                                 BLECharacteristic::PROPERTY_READ   |
                                 BLECharacteristic::PROPERTY_WRITE  |
                                 BLECharacteristic::PROPERTY_NOTIFY |
                                 BLECharacteristic::PROPERTY_INDICATE
                               );
                           **********************************************/
                           NIMBLE_PROPERTY::READ   |
                           NIMBLE_PROPERTY::WRITE  |
                           NIMBLE_PROPERTY::NOTIFY |
                           NIMBLE_PROPERTY::INDICATE
                       );
    pCharacteristic1->setCallbacks(&chrCallbacks1);
    pCharacteristic2 = pService->createCharacteristic(
                           CHARACTERISTIC_UUID_2,
                           /******* Enum Type NIMBLE_PROPERTY now *******
                                 BLECharacteristic::PROPERTY_READ   |
                                 BLECharacteristic::PROPERTY_WRITE  |
                                 BLECharacteristic::PROPERTY_NOTIFY |
                                 BLECharacteristic::PROPERTY_INDICATE
                               );
                           **********************************************/
                           NIMBLE_PROPERTY::READ   |
                           NIMBLE_PROPERTY::WRITE  |
                           NIMBLE_PROPERTY::NOTIFY |
                           NIMBLE_PROPERTY::INDICATE
                       );
    pCharacteristic2->setCallbacks(&chrCallbacks2);

    BLEService* pServiceBat = pServer->createService(BATTERY_SERVICE_UUID);
    pCharacteristicBat = pServiceBat->createCharacteristic(
                             BATTERY_LEVEL_CHARACTERISTIC_UUID,
                             NIMBLE_PROPERTY::READ
                         );
    pCharacteristicBat->setCallbacks(&chrCallbacksBat);

    // Start the service
    pService->start();
    pServiceBat->start();
    Serial.println(String("Free Heap at line ") + __LINE__ + ": " + ESP.getFreeHeap() + "B");

    // Start advertising
    BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->addServiceUUID(BATTERY_SERVICE_UUID);
    pAdvertising->setScanResponse(false);
    /** Note, this could be left out as that is the default value */
    pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter

    BLEDevice::startAdvertising();
    Serial.println("Waiting a client connection to notify...");



}

void OswAppBLE::onLoop() {
    OswAppV2::onLoop(); // always make sure to call the base class method!


}

void OswAppBLE::onDraw() {
    OswAppV2::onDraw(); // always make sure to call the base class method!

    hal->gfx()->setTextCursor(DISP_W / 2, DISP_H / 2);
    hal->gfx()->print("OK");
}

void OswAppBLE::onButton(Button id, bool up, ButtonStateNames state) {
    OswAppV2::onButton(id, up, state); // always make sure to call the base class method!

}

void OswAppBLE::onStop() {
    OswAppV2::onStop(); // always make sure to call the base class method!

    BLEDevice::deinit(true);
}
