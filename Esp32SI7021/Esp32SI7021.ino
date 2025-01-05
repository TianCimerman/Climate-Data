/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-i2c-communication-arduino-ide/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/
#include <WiFiMulti.h>
WiFiMulti wifiMulti;
#define DEVICE "ESP32"
#include <esp_sleep.h>



#include "Adafruit_Si7021.h"
Adafruit_Si7021 sensor_2 = Adafruit_Si7021();

// Import the InfluxDB libraries
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

// WiFi AP SSID
#define WIFI_SSID "CimermanWiFi"
// WiFi password
#define WIFI_PASSWORD "dobovica2a"


// Set up your InfluxDB details here

// InfluxDB v2 server url (Use: InfluxDB UI -> Load Data -> Client Libraries)
#define INFLUXDB_URL "http://192.168.1.120:8086"

// InfluxDB v2 server or cloud API authentication token (Use: InfluxDB UI -> Data -> Tokens -> <select token>)
#define INFLUXDB_TOKEN "2B7SPpKmVGAnMCkWYTxRTU1_vtbruS2EBEYN_zqyKR0azK5S-lKyhTNdVf8IsmYleo566wIT3-fTTY_QhHQd9Q=="

// InfluxDB v2 organization id (Use: InfluxDB UI -> User -> About -> Common Ids )
#define INFLUXDB_ORG "family"

// InfluxDB v2 bucket name (Use: InfluxDB UI -> Data -> Buckets)
#define INFLUXDB_BUCKET "data"

// Set timezone string according to <https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html>
#define TZ_INFO "CET-1CEST,M3.5.0,M10.5.0/3"

// InfluxDB client instance with preconfigured InfluxCloud certificate
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

// Create your Data Point here
Point sensor("climate");


//#define I2C_SDA 4
//#define I2C_SCL 5
const int led = 2;





unsigned long delayTime;

class Timer {
private:
  unsigned int setTime;
  unsigned int startTime;
  unsigned int elapsedTime;
  bool enablePrevious;
  bool isStart;

public:
  bool process(bool enable, unsigned int time=0)
  {
    if (!enablePrevious && enable){
      start(time);
    }

    if (enablePrevious && !enable){
      reset();
    }
    enablePrevious = enable;

    if (enable){
      return q();
    }
    else{
      return false;
    }
  }

  void start(unsigned int time) {
    setTime = time;
    isStart = true;
    startTime = millis();
  }

  bool q()
  {
    elapsedTime =  millis() - startTime;
    if ((isStart = true) && (elapsedTime >= setTime)){
      return true;
    }
    else{
      return false;
    }
  }

  bool reset()
  {
    isStart = false;
    startTime = 0;
    elapsedTime = 0;
    return false;
  }
};

class cycleDiagnostics{
  private:
    String coreId;
    char buffer[100];
    Timer diagnosticsDelay;
    unsigned long numberOfCycles, meanCycleTime, maxCycleTime, minCycleTime, previousCycleTimeUs, currentCycleTimeUs;

  public:
    unsigned long getMean(){
      return meanCycleTime;
    }
    unsigned long getMin(){
      return minCycleTime;
    }
    unsigned long getMax(){
      return maxCycleTime;
    }
    void run(){
      numberOfCycles++;
      diagnosticsDelay.process(true, 1000);
      if (diagnosticsDelay.q()){
        diagnosticsDelay.process(false);
        meanCycleTime = 1000000/numberOfCycles;
        sprintf(buffer, "Mean cycle time: %dus, max: %dus, min %dus",meanCycleTime , maxCycleTime, minCycleTime);
        Serial.print(coreId);
        Serial.println(buffer);
        numberOfCycles = 0;
        maxCycleTime = 0;
        minCycleTime = 10000;
      }

      currentCycleTimeUs = micros();
      if ((currentCycleTimeUs - previousCycleTimeUs) > maxCycleTime){
        maxCycleTime = currentCycleTimeUs - previousCycleTimeUs;
      }
      if ((currentCycleTimeUs - previousCycleTimeUs) < minCycleTime){
        minCycleTime = currentCycleTimeUs - previousCycleTimeUs;
      }
      previousCycleTimeUs = currentCycleTimeUs;
    }

    void setCore(String core){
        coreId = core;
    }
};

Timer ledDelay, sensorDelay;

void setup() {
  Serial.begin(115200);

  if (!sensor_2.begin()) {
    Serial.println("Did not find Si7021 sensor!");
    while (true)
      ;
  }

  pinMode(led, OUTPUT);

  


  // Check server connection


  Serial.println("-- Default Test --");
  delayTime = 1000;

  Serial.println();


}

void loop() { 
  // Clear fields for reusing the point. Tags will remain untouched
  sensor.clearFields();

 ledDelay.process(true, 100);
  if (ledDelay.q()){
    ledDelay.process(false);
    digitalWrite(led, !digitalRead(led));
  }

  
  sensorDelay.process(false, 60000);
  if (sensorDelay.q()){
    sensorDelay.process(false);
    printValues();
  }
}

void printValues() {
  InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

// Create your Data Point here
  Point sensor("climate");
  float temperature;
  float humidity;
  temperature=sensor_2.readTemperature();
  humidity=sensor_2.readHumidity();



  bool status;
    // Setup wifi
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to wifi");
  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println();

  sensor.addTag("device", DEVICE);

  // Accurate time is necessary for certificate validation and writing in batches
  // For the fastest time sync, find NTP servers in your area: https://www.pool.ntp.org/zone/
  // Syncing progress and the time will be printed to Serial.
  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");

  if (client.validateConnection()) {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
  }  Serial.print("Temperature = ");
  
  Serial.print(temperature);
  Serial.println(" *C");
  


  Serial.print("Humidity = ");
  Serial.print(humidity);
  Serial.println(" %");
  Serial.println();

  sensor.addField("humidity", humidity);
  sensor.addField("temperature",temperature);

 
   // Print what are we exactly writing
  Serial.print("Writing: ");
  Serial.println(sensor.toLineProtocol());

    // If no Wifi signal, try to reconnect it
  if ((WiFi.RSSI() == 0) && (wifiMulti.run() != WL_CONNECTED)) {
    Serial.println("Wifi connection lost");
  }

  // Write point
  if (!client.writePoint(sensor)) {
    Serial.print("InfluxDB write failed: ");
    Serial.println(client.getLastErrorMessage());
  }
  esp_sleep_enable_timer_wakeup(60 * 1000000);  // 10 seconds
  esp_deep_sleep_start();
  
}
