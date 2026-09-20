#include <WiFi.h>
#include "ThingSpeak.h"
#include <Wire.h>
#include "INA228.h"
#include <Adafruit_MCP9601.h>

// --- WI-FI & THINGSPEAK CREDENTIALS ---
const char* ssid = [SSID];   
const char* password = [YOUR_WIFI_PASSWORD];   
unsigned long myChannelNumber = [CHANNEL_NUMBER];            
const char * myWriteAPIKey = [YOUR_API_KEY];    

WiFiClient client;

// --- Power Sensors (INA228) ---
// Sensor 1: Original sensor at default address 69 (0x45 in Hex)
INA228 INA1(0x45);
// Sensor 2: Second sensor with address 68 (0x44 in Hex)
INA228 INA2(0x44);
// Sensor 3: Third sensor with address 65 (0x41 in Hex)
INA228 INA3(0x41);

// --- Temperature Sensors (MCP9601) ---
Adafruit_MCP9601 mcp1;
Adafruit_MCP9601 mcp2;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  // Wait for serial monitor to open
  while (!Serial) { delay(1); }

  Serial.println("Initializing DAQ System Sensors...");

  // --- INIT POWER SENSOR 1 ---
  if (!INA1.begin()) {
    Serial.println("Failed to find INA228 Sensor 1 at 0x45.");
    while (1) { delay(10); }
  }
  INA1.setMaxCurrentShunt(204.8, 0.0002);
  Serial.println("INA228 Sensor 1 found and calibrated!");

  // --- INIT POWER SENSOR 2 ---
  if (!INA2.begin()) {
    Serial.println("Failed to find INA228 Sensor 2 at 0x44.");
    while (1) { delay(10); }
  }
  INA2.setMaxCurrentShunt(204.8, 0.0002);
  Serial.println("INA228 Sensor 2 found and calibrated!");

  // --- INIT POWER SENSOR 3 ---
  if (!INA3.begin()) {
    Serial.println("Failed to find INA228 Sensor 3 at 0x41.");
    while (1) { delay(10); }
  }
  INA3.setMaxCurrentShunt(204.8, 0.0002);
  Serial.println("INA228 Sensor 3 found and calibrated!");

  // --- INIT TEMPERATURE SENSOR 1 (Default Address) ---
  if (!mcp1.begin(0x67)) {
    Serial.println("Failed to find MCP9601 Sensor 1 at 0x67.");
    while (1) { delay(10); }
  }
  mcp1.setThermocoupleType(MCP9600_TYPE_K);
  Serial.println("MCP9601 Sensor 1 found!");

  // --- INIT TEMPERATURE SENSOR 2 (43k Bridged Address) ---
  if (!mcp2.begin(0x66)) {
    Serial.println("Failed to find MCP9601 Sensor 2 at 0x66.");
    Serial.println("Check your 43k solder bridge!");
    while (1) { delay(10); }
  }
  mcp2.setThermocoupleType(MCP9600_TYPE_K);
  Serial.println("MCP9601 Sensor 2 found!");

  Serial.println("--------------------------------------");
  Serial.println("DAQ INITIALIZATION COMPLETE");
  Serial.println("--------------------------------------");
  
  // --- Connect to Wi-Fi ---
  //Need to delete:WiFi.mode(WIFI_STA); because its unique to esp boards   
  Serial.print("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");
  ThingSpeak.begin(client);  
}

void loop() {
  // 1. Read all sensors into variables first
  float v1 = INA1.getBusVoltage();
  float i1 = INA1.getCurrent();
  
  float v2 = INA2.getBusVoltage();
  float i2 = INA2.getCurrent();
  
  float v3 = INA3.getBusVoltage();
  float i3 = INA3.getCurrent();
  
  float t1 = mcp1.readThermocouple();
  float t2 = mcp2.readThermocouple();

  // 2. Print everything to the Serial Monitor 
  Serial.println("--- POWER SENSOR 1 (0x45) ---");
  Serial.print("Bus Voltage: "); Serial.print(v1, 3); Serial.println(" V");
  Serial.print("Current: "); Serial.print(i1, 3); Serial.println(" A");

  Serial.println("--- POWER SENSOR 2 (0x44) ---");
  Serial.print("Bus Voltage: "); Serial.print(v2, 3); Serial.println(" V");
  Serial.print("Current: "); Serial.print(i2, 3); Serial.println(" A");

  Serial.println("--- POWER SENSOR 3 (0x41) ---");
  Serial.print("Bus Voltage: "); Serial.print(v3, 3); Serial.println(" V");
  Serial.print("Current: "); Serial.print(i3, 3); Serial.println(" A");

  Serial.println("--- TEMP SENSOR 1 (0x67) ---");
  Serial.print("Hot Junction: "); Serial.print(t1); Serial.println(" C");

  Serial.println("--- TEMP SENSOR 2 (0x66) ---");
  Serial.print("Hot Junction: "); Serial.print(t2); Serial.println(" C");

  // 3. Send all 8 variables to ThingSpeak
  ThingSpeak.setField(1, v1); // Field 1: Sensor 1 Voltage
  ThingSpeak.setField(2, i1); // Field 2: Sensor 1 Current
  ThingSpeak.setField(3, v2); // Field 3: Sensor 2 Voltage
  ThingSpeak.setField(4, i2); // Field 4: Sensor 2 Current
  ThingSpeak.setField(5, v3); // Field 5: Sensor 3 Voltage
  ThingSpeak.setField(6, i3); // Field 6: Sensor 3 Current
  ThingSpeak.setField(7, t1); // Field 7: Temp 1
  ThingSpeak.setField(8, t2); // Field 8: Temp 2
  
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  
  if(x == 200){
    Serial.println("=> ThingSpeak update successful.");
  } else {
    Serial.println("=> Problem updating channel. HTTP error code " + String(x));
  }
  
  Serial.println("======================================");

  // ThingSpeak limit is 15 seconds. Wait 20 seconds before the next DAQ cycle.
  delay(20000); 
}