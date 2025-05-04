/**
 * @file FlowRate.ino
 * @brief Contoh sederhana pengukuran laju aliran menggunakan MPS20N0040D
 * @author-team Ana Maulida, GALIH RIDHO UTOMO
 */
#include "MPS20N0040D.h"

// Definisi pin untuk sensor
const int CLOCK_PIN = 3;
const int DATA_PIN = 2;

// Membuat instance sensor
MPS20N0040D pressureSensor(CLOCK_PIN, DATA_PIN);

void setup() {
  Serial.begin(9600);
  Serial.println(F("MPS20N0040D Flow Rate Example"));
  
  // Inisialisasi sensor
  pressureSensor.begin();
  
  // Mengatur geometri pipa (diameter dalam meter)
  pressureSensor.setGeometry(0.020, 0.010); // pipa utama 20mm, throat 10mm
  
  Serial.println(F("Sensor initialized"));
}

void loop() {
  // Membaca tekanan
  float pressure = pressureSensor.readPressure1();
  
  // Menggunakan perbedaan tekanan untuk menghitung laju aliran
  float pressureDiff = pressureSensor.calculatePressureDifference();
  
  // Menghitung laju aliran dalam m³/s
  float flowRate = pressureSensor.calculateFlowRateFromPressureDiff(pressureDiff);
  
  // Menghitung laju aliran dalam L/min untuk tampilan yang lebih praktis
  float flowRateLPM = flowRate * 60000.0; // m³/s * 1000 L/m³ * 60 s/min
  
  // Menampilkan hasil
  Serial.print(F("Pressure: "));
  Serial.print(pressure);
  Serial.println(F(" Pa"));
  
  Serial.print(F("Pressure Difference: "));
  Serial.print(pressureDiff);
  Serial.println(F(" Pa"));
  
  Serial.print(F("Flow Rate: "));
  Serial.print(flowRate, 6);
  Serial.println(F(" m³/s"));
  
  Serial.print(F("Flow Rate: "));
  Serial.print(flowRateLPM, 2);
  Serial.println(F(" L/min"));
  
  Serial.println();
  delay(1000);
}
