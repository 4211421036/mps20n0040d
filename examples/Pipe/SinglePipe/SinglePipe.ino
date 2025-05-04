/**
 * @file SinglePipe.ino
 * @brief Contoh penggunaan library MPS20N0040D untuk venturi meter konfigurasi pipa tunggal
 * @author-team Ana Maulida, GALIH RIDHO UTOMO
 */
#include "MPS20N0040D.h"

// Definisi pin untuk sensor
const int CLOCK_PIN = 3;
const int DATA_PIN = 2;

// Membuat instance sensor dengan konfigurasi pipa tunggal
MPS20N0040D pressureSensor(CLOCK_PIN, DATA_PIN);

void setup() {
  Serial.begin(9600);
  Serial.println(F("MPS20N0040D Venturi Meter - Single Pipe Example"));
  
  // Inisialisasi sensor
  pressureSensor.begin();
  
  // Mengatur tipe venturi ke SINGLE_PIPE
  pressureSensor.setVenturiType(SINGLE_PIPE);
  
  // Mengatur geometri pipa (diameter dalam meter)
  // Contoh: pipa utama 20mm dan throat 10mm
  pressureSensor.setGeometry(0.020, 0.010);
  
  Serial.println(F("Sensor initialized"));
  Serial.println(F("Format: Pressure (Pa), Estimated Difference (Pa), Flow Rate (L/min)"));
}

void loop() {
  // Membaca tekanan dari sensor
  float pressure = pressureSensor.readPressure1();
  
  // Menghitung perbedaan tekanan (pada konfigurasi pipa tunggal, ini
  // akan mengestimasi berdasarkan tekanan atmosfer pada throat)
  float pressureDiff = pressureSensor.calculatePressureDifference();
  
  // Menghitung laju aliran (konversi m³/s ke L/min)
  float flowRate = pressureSensor.calculateFlowRate() * 60000.0; // m³/s * 1000 L/m³ * 60 s/min
  
  // Menampilkan hasil
  Serial.print(pressure, 2);
  Serial.print(F(" Pa, Diff: "));
  Serial.print(pressureDiff, 2);
  Serial.print(F(" Pa, Flow: "));
  Serial.print(flowRate, 4);
  Serial.println(F(" L/min"));
  
  // Menampilkan informasi tambahan untuk pemahaman lebih baik
  Serial.print(F("Estimasi berdasarkan tekanan atmosfer standar (101325 Pa) pada throat."));
  Serial.println();
  
  delay(1000);  // Update setiap detik
}
