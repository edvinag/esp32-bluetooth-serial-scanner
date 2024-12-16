#include <Arduino.h>
#include <BluetoothSerial.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to enable it
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

BluetoothSerial SerialBT;
#define BT_DISCOVER_TIME 10000

void setup()
{
  Serial.begin(115200);
  Serial.println("Starting...");

  // Initialize Bluetooth
  if (!SerialBT.begin("ESP32-Scanner"))
  {
    Serial.println("Failed to initialize Bluetooth");
    abort();
  }

  // Start scanning
  Serial.println("Scanning for devices...");
  SerialBT.discover(BT_DISCOVER_TIME); // Blocking scan for the specified time
  BTScanResults *btDeviceList = SerialBT.getScanResults();

  if (btDeviceList->getCount() > 0)
  {
    Serial.printf("Found %d devices:\n", btDeviceList->getCount());
    for (int i = 0; i < btDeviceList->getCount(); i++)
    {
      BTAdvertisedDevice *device = btDeviceList->getDevice(i);

      // Print basic device info
      Serial.printf(" ----- Address: %s | Name: %s | RSSI: %d\n",
                    device->getAddress().toString().c_str(),
                    device->getName().c_str(),
                    device->getRSSI());

      // Scan for available channels/services on the device
      Serial.println("     Scanning for available channels...");
      std::map<int, std::string> channels = SerialBT.getChannels(device->getAddress());
      if (channels.size() > 0)
      {
        Serial.printf("     Found %d channels:\n", channels.size());
        for (auto const &entry : channels)
        {
          Serial.printf("         Channel %d: %s\n", entry.first, entry.second.c_str());
        }
      }
      else
      {
        Serial.println("     No channels found on this device.");
      }
    }
  }
  else
  {
    Serial.println("No devices found.");
  }
}

void loop()
{
  delay(1000); // No repeated actions in the loop for this example
}
