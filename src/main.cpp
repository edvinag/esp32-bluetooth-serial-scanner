#include <Arduino.h>
#include <BluetoothSerial.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

BluetoothSerial SerialBT;

#define BT_DISCOVER_TIME 10000
esp_spp_sec_t sec_mask = ESP_SPP_SEC_NONE; // or ESP_SPP_SEC_ENCRYPT|ESP_SPP_SEC_AUTHENTICATE to request pincode confirmation
esp_spp_role_t role = ESP_SPP_ROLE_SLAVE;  // or ESP_SPP_ROLE_MASTER

void setup()
{
  if (!SerialBT.begin("ESP32-Scanner", true))
  {
    Serial.println("Failed to initialize Bluetooth");
    abort();
  }

  Serial.println("Scanning for devices...");
  BTScanResults *btDeviceList = SerialBT.getScanResults(); // maybe accessing from different threads!
  delay(BT_DISCOVER_TIME);
  if (btDeviceList->getCount() > 0)
  {
    BTAddress addr;
    int channel = 0;
    Serial.println("Found devices:");
    for (int i = 0; i < btDeviceList->getCount(); i++)
    {
      BTAdvertisedDevice *device = btDeviceList->getDevice(i);
      Serial.printf(" ----- %s  %s %d\n", device->getAddress().toString().c_str(), device->getName().c_str(), device->getRSSI());
      std::map<int, std::string> channels = SerialBT.getChannels(device->getAddress());
      Serial.printf("scanned for services, found %d\n", channels.size());
      for (auto const &entry : channels)
      {
        Serial.printf("     channel %d (%s)\n", entry.first, entry.second.c_str());
      }
      if (channels.size() > 0)
      {
        addr = device->getAddress();
        channel = channels.begin()->first;
      }
    }
  }
  else
  {
    Serial.println("Didn't find any devices, restarting...");
    delay(500);
    ESP.restart();
  }
}

void loop()
{
  delay(1000);
}
