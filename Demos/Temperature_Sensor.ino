
#include <CurieBLE.h>

BLEPeripheral blePeripheral;
//BLEService ledService("19B10000-E8F2-537E-4F6C-D104768A1214"); // create service
BLEService intService("19B10000-E8F2-537E-4F6C-D104768A1214");  // BLE int Service

BLEIntCharacteristic tempChar("19B10002-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);
BLEIntCharacteristic moistChar("19B10003-E8F2-537E-4F6C-D104768A1214", BLERead);


// create switch characteristic and allow remote device to read and write
BLEIntCharacteristic switchCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);

// Define the pin to which the temperature sensor is connected.
const int pinTemp = A3;

// TODO: pint for moisuture
//const int pinMoisture = A2;

// Define the B-value of the thermistor.
// This value is a property of the thermistor used in the Grove - Temperature Sensor,
// and used to convert from the analog value it measures and a temperature value.
const int B = 3975;

int oldTemperature = 0;
int oldMoisture = 0;
int delayTime = 1000;


void setup()
{
    // Configure the serial communication line at 9600 baud (bits per second.)
    Serial.begin(9600);

    // TODO: change to a grove LED
    pinMode(13, OUTPUT);   // initialize the LED on pin 13 to indicate when a central is connected


    /* Set a local name for the BLE device
     This name will appear in advertising packets
     and can be used by remote devices to identify this BLE device
     The name can be changed but maybe be truncated based on space left in advertisement packet */
    blePeripheral.setLocalName("GardenArduino");
    //blePeripheral.setAdvertisedServiceUuid(ledService.uuid());  // add the service UUID
    blePeripheral.setAdvertisedServiceUuid(intService.uuid());
    
    //blePeripheral.addAttribute(ledService);
    blePeripheral.addAttribute(intService);
    blePeripheral.addAttribute(switchCharacteristic);
    blePeripheral.addAttribute(tempChar);
    blePeripheral.addAttribute(moistChar);

    tempChar.setValue(oldTemperature);
    moistChar.setValue(oldMoisture);
  
    switchCharacteristic.setEventHandler(BLEWritten, switchCharacteristicWritten);
    switchCharacteristic.setValue(delayTime);

    /* Now activate the BLE device.  It will start continuously transmitting BLE
     advertising packets and will be visible to remote BLE central devices
     until it receives a new connection */
    blePeripheral.begin();
    Serial.println("Bluetooth device active, waiting for connections...");
    
}

void loop()
{

    // listen for BLE peripherals to connect:
    BLECentral central = blePeripheral.central();
    
    // if a central is connected to peripheral:
    if (central) {
      Serial.print("Connected to central: ");
      // print the central's MAC address:
      Serial.println(central.address());
      // turn on the LED to indicate the connection:
      digitalWrite(13, HIGH);
  
      // check the battery level every 200ms
      // as long as the central is still connected:
      while (central.connected()) {

        updateTemperature();
        
        // Wait between measurements.
        delay(delayTime);
      }
      // when the central disconnects, turn off the LED:
      digitalWrite(13, LOW);
      Serial.print("Disconnected from central: ");
      Serial.println(central.address());
    }
    blePeripheral.poll();
}

void updateTemperature() {
    /*
     * Read the temperature from A0 analog input pin
    */
    // Get the (raw) value of the temperature sensor.
    int val = analogRead(pinTemp);
  
    // Determine the current resistance of the thermistor based on the sensor value.
    float resistance = (float)(1023-val)*10000/val;
  
    // Calculate the temperature based on the resistance value.
    float temperature = 1/(log(resistance/10000)/B+1/298.15)-273.15;
    if (temperature != oldTemperature) {
        // Print the temperature to the serial console.
        Serial.println("Temperatre (Celcius) is now: ");
        Serial.println(temperature);
        tempChar.setValue(temperature);
        oldTemperature = temperature;
    }
    
}

//void updateMoisture() {
//    /*
//     * Read the Moisture from A1 analog input pin
//    */
//    // Get the (raw) value of the moisture sensor.
//    int moistureVal = analogRead(sensorPin);
//  
//    if (moistureVal != oldMoisture) {
//        // Print the moisture value to the serial console.
//        Serial.println("Moisture is now: ");
//        Serial.println(moistureVal);
//        moistChar.setValue(moistureVal);
//        oldMoisture = moistureVal;
//    }
//    
//}

void switchCharacteristicWritten(BLECentral& central, BLECharacteristic& characteristic) {
    // central wrote new value to characteristic, update time delay
    Serial.print("Characteristic event, written: ");
  
    if(switchCharacteristic.value()) {
        Serial.println(switchCharacteristic.value());
        if(switchCharacteristic.value() > 100) {
            delayTime = switchCharacteristic.value();
        }
    }
 
      
        
} 
