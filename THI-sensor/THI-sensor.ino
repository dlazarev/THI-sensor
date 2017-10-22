#include <Wire.h>
#include <ESP8266WiFi.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <WiFiUDP.h>

WiFiUDP udp;
Adafruit_BME280 bme; // I2C
const char* ssid = "BOSON2.4";
const char* passwd = "kartoshka";

const byte influxdb_host[] = {138, 201, 158, 136}; // the IP address of your InfluxDB host
const int influxdb_port = 8089; // UDP port of your InfluxDB host

float temperature, pressure, humidity, illumination;

void setup() {
  Serial.begin(115200);
  delay(10);
  Wire.begin(0x76);
  Wire.setClock(100000);

  // Connecting to WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.print(ssid);

  WiFi.begin(ssid, passwd);
  while (WiFi.status() != WL_CONNECTED) {
  	delay(500);
  	Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected.");

  // Print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  
  Serial.println(WiFi.localIP());

  if (!bme.begin()) {
  	Serial.println("Could not find a valid BME280 sensor, check wiring!");
  	while (1);
  }


}

void getWeather() {
	temperature = bme.readTemperature();
	pressure = bme.readPressure();
	humidity = bme.readHumidity();
}

void getIllumination() {
	illumination = analogRead(0);
}

void loop() {
  String udp_data;

  getWeather();
  getIllumination();

  Serial.println();
  Serial.print("Temperature = ");
  Serial.println(temperature);

  Serial.print("Pressure = ");
  Serial.println(pressure);

  Serial.print("Humidity = ");
  Serial.println(humidity);

  Serial.print("Illumination = ");
  Serial.println(illumination);

  udp_data = String("BME280,device_id=001 temperature=" + String(temperature));
  udp_data += String(",humidity=" + String(humidity));
  udp_data += String(",pressure=" + String(pressure));
  udp_data += String(",illumination=" + String(illumination));
  Serial.println("Sending UDP packet...");
  Serial.println(udp_data);
  udp.beginPacket(influxdb_host, influxdb_port);
  udp.print(udp_data);
  udp.endPacket();

  delay(5000);
}
