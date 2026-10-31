#include <WiFi.h>
#include <HTTPClient.h>

// =====================================================
// KAWING PALAYSENSE
// ESP32 + HW-390 Capacitive Sensor V2.0.0
// =====================================================

// =====================================================
// WI-FI SETTINGS
// =====================================================

// Replace with your own credentials
const char* WIFI_SSID = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// =====================================================
// KAWING REST API
// =====================================================

// Replace with your server endpoint and API key
const char* KAWING_API_URL =
  "http://YOUR_SERVER:PORT/YOUR_API_ENDPOINT";

const char* KAWING_API_KEY =
  "YOUR_API_KEY";

// =====================================================
// DEVICE INFORMATION
// =====================================================

const char* DEVICE_ID = "YOUR_DEVICE_ID";

const char* SENSOR_MODEL = "HW-390 V2.0.0";

// =====================================================
// SENSOR SETTINGS
// =====================================================

// HW-390 analog output connected to GPIO 34
const int SENSOR_PIN = 34;

// 12-bit ADC resolution: 0 to 4095
const int ADC_RESOLUTION = 12;

// Average 20 sensor readings
const int SENSOR_SAMPLES = 20;

// Delay between ADC samples
const int SAMPLE_DELAY_MS = 10;

// Read sensor every 5 seconds
const unsigned long READ_INTERVAL = 5000;

// Wi-Fi connection timeout
const unsigned long WIFI_TIMEOUT = 20000;

// =====================================================
// MOISTURE CLASSIFICATION THRESHOLDS
// =====================================================

// Above 17%         = Wet
// 15% to 17%        = Moderate
// Above 11% to <15% = Dry
// 11% and below     = Risk of Grain Breakage

const float WET_THRESHOLD = 17.0;
const float MODERATE_THRESHOLD = 15.0;
const float BREAKAGE_THRESHOLD = 11.0;

// =====================================================
// GRAIN MOISTURE CONVERSION CONSTANTS
// =====================================================

// Adopted conversion model: Putri et al. (2023)
// Normalize 12-bit ADC to 10-bit equivalent

const float ADC_12BIT_MAX = 4095.0;
const float ADC_10BIT_MAX = 1023.0;

const float SENSOR_CORRECTION = 1.0054;
const float DRY_REFERENCE = 898.0;

// =====================================================
// GLOBAL VARIABLES
// =====================================================

unsigned long lastReadingTime = 0;

// =====================================================
// FUNCTION DECLARATIONS
// =====================================================

void connectWiFi();
int readSensorAverage();
float convertToMoisture(int rawValue);
String classifyMoisture(float moisture);
void sendToKAWING(
  int rawValue,
  float moisturePercentage,
  String classification
);

// =====================================================
// SETUP
// =====================================================

void setup() {

  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("====================================");
  Serial.println("          KAWING PALAYSENSE");
  Serial.println("====================================");

  Serial.println(
    "ESP32 Grain Moisture Assessment Device"
  );

  Serial.print("Sensor Model       : ");
  Serial.println(SENSOR_MODEL);

  Serial.println(
    "Moisture Model     : Putri et al. (2023)"
  );

  Serial.println("ADC Resolution     : 12-bit");
  Serial.println("====================================");

  // Configure ESP32 ADC
  analogReadResolution(ADC_RESOLUTION);
  pinMode(SENSOR_PIN, INPUT);

  // Connect to Wi-Fi
  connectWiFi();
}

// =====================================================
// MAIN LOOP
// =====================================================

void loop() {

  // Reconnect Wi-Fi if disconnected
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi disconnected. Reconnecting...");
    connectWiFi();
  }

  // Read moisture sensor at specified interval
  if (millis() - lastReadingTime >= READ_INTERVAL) {

    lastReadingTime = millis();

    // Read and average sensor values
    int rawValue = readSensorAverage();

    // Convert ADC reading into moisture percentage
    float moisturePercentage = convertToMoisture(rawValue);

    // Classify grain moisture
    String classification = classifyMoisture(moisturePercentage);

    // Display sensor results
    Serial.println();
    Serial.println("------------------------------------");

    Serial.print("Sensor Model       : ");
    Serial.println(SENSOR_MODEL);

    Serial.print("Raw ADC Value      : ");
    Serial.println(rawValue);

    Serial.print("Moisture Content   : ");
    Serial.print(moisturePercentage, 2);
    Serial.println("%");

    Serial.print("Classification     : ");
    Serial.println(classification);

    // Grain breakage warning
    if (moisturePercentage <= BREAKAGE_THRESHOLD) {
      Serial.println(
        "Moisture Note      : Low moisture - Risk of Grain Breakage"
      );
    }

    // Display Wi-Fi connection status only
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Wi-Fi Status       : Connected");
    } else {
      Serial.println("Wi-Fi Status       : Disconnected");
    }

    Serial.println("------------------------------------");

    // Send moisture reading to KAWING
    sendToKAWING(rawValue, moisturePercentage, classification);
  }
}

// =====================================================
// READ AND AVERAGE SENSOR VALUES
// =====================================================

int readSensorAverage() {

  unsigned long total = 0;

  for (int i = 0; i < SENSOR_SAMPLES; i++) {
    int reading = analogRead(SENSOR_PIN);
    total += reading;
    delay(SAMPLE_DELAY_MS);
  }

  int average = total / SENSOR_SAMPLES;
  return average;
}

// =====================================================
// CONVERT ADC TO MOISTURE PERCENTAGE
// =====================================================

float convertToMoisture(int rawValue) {

  // STEP 1: Normalize 12-bit ADC to 10-bit equivalent
  float adc10 =
    ((float)rawValue * ADC_10BIT_MAX) / ADC_12BIT_MAX;

  // STEP 2: Apply adopted sensor correction
  float correctedADC = adc10 / SENSOR_CORRECTION;

  // STEP 3: Convert ADC value to moisture percentage
  float moisturePercentage =
    100.0 - ((correctedADC / DRY_REFERENCE) * 100.0);

  // Limit result to 0% - 100%
  moisturePercentage = constrain(moisturePercentage, 0.0, 100.0);

  return moisturePercentage;
}

// =====================================================
// GRAIN MOISTURE CLASSIFICATION
// =====================================================

String classifyMoisture(float moisture) {

  // WET: Above 17%
  if (moisture > WET_THRESHOLD) {
    return "Wet";
  }

  // MODERATE: 15% to 17%
  if (moisture >= MODERATE_THRESHOLD) {
    return "Moderate";
  }

  // DRY: Above 11% but below 15%
  if (moisture > BREAKAGE_THRESHOLD) {
    return "Dry";
  }

  // RISK: 11% and below
  return "Risk of Grain Breakage";
}

// =====================================================
// WI-FI CONNECTION
// =====================================================

void connectWiFi() {

  Serial.println();
  Serial.println("Connecting to Wi-Fi...");

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  unsigned long connectionStarted = millis();

  while (
    WiFi.status() != WL_CONNECTED &&
    millis() - connectionStarted < WIFI_TIMEOUT
  ) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Wi-Fi connected successfully.");

    // Network name, password, and IP are not printed
    Serial.print("Signal Strength : ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
  } else {
    Serial.println("Wi-Fi connection failed.");
    Serial.println("PalaySense will retry automatically.");
  }

  Serial.println();
}

// =====================================================
// SEND SENSOR DATA TO KAWING REST API
// =====================================================

void sendToKAWING(
  int rawValue,
  float moisturePercentage,
  String classification
) {

  // Check connection
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Cannot send reading: Wi-Fi disconnected.");
    return;
  }

  WiFiClient client;
  HTTPClient http;

  Serial.println();
  Serial.println("Connecting to KAWING REST API...");

  if (!http.begin(client, KAWING_API_URL)) {
    Serial.println("Unable to initialize HTTP connection.");
    return;
  }

  // HTTP SETTINGS
  http.setConnectTimeout(10000);
  http.setTimeout(15000);

  http.addHeader("Content-Type", "application/json");
  http.addHeader("X-API-Key", KAWING_API_KEY);

  // CREATE JSON PAYLOAD
  String json = "{";

  json += "\"deviceId\":\"";
  json += DEVICE_ID;
  json += "\",";

  json += "\"sensorModel\":\"";
  json += SENSOR_MODEL;
  json += "\",";

  json += "\"rawValue\":";
  json += String(rawValue);
  json += ",";

  // Adopted conversion model flag
  json += "\"calibrated\":true,";

  json += "\"moisturePercentage\":";
  json += String(moisturePercentage, 2);
  json += ",";

  json += "\"classification\":\"";
  json += classification;
  json += "\"}";

  // HTTP POST REQUEST
  Serial.println("Sending moisture reading to KAWING...");

  // Do not print full request or credentials
  int responseCode = http.POST(json);

  Serial.print("HTTP Response Code: ");
  Serial.println(responseCode);

  // HANDLE API RESPONSE
  if (responseCode > 0) {
    if (responseCode == 200 || responseCode == 201) {
      Serial.println("Reading successfully sent to KAWING!");
    } else if (responseCode == 400) {
      Serial.println("Bad request. Check JSON/API requirements.");
    } else if (responseCode == 401) {
      Serial.println("API authentication failed.");
    } else if (responseCode == 404) {
      Serial.println("KAWING API endpoint not found.");
    } else if (responseCode >= 500) {
      Serial.println("KAWING server error.");
    } else {
      Serial.println("API request returned an unexpected status.");
    }
  } else {
    Serial.println("Failed to send reading.");
    Serial.print("HTTP Error: ");
    Serial.println(HTTPClient::errorToString(responseCode));
  }

  // Close connection
  http.end();
}
