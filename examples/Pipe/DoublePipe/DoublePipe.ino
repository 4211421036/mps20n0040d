/**
 * @file DoublePipe.ino
 * @brief Contoh penggunaan library MPS20N0040D untuk venturi meter
 * @author-team Ana Maulida, GALIH RIDHO UTOMO
 */

#include "MPS20N0040D.h"

// Definisi pin untuk sensor 1
const int CLOCK_PIN_1 = 3;
const int DATA_PIN_1 = 2;

// Definisi pin untuk sensor 2 (untuk konfigurasi DOUBLE_PIPE)
const int CLOCK_PIN_2 = 5;
const int DATA_PIN_2 = 4;

// Membuat instance sensor dengan konfigurasi yang diinginkan
// Pilih salah satu dari dua baris berikut, sesuai dengan konfigurasi Anda

// Untuk konfigurasi pipa tunggal:
// MPS20N0040D pressureSensor(CLOCK_PIN_1, DATA_PIN_1);

// Untuk konfigurasi pipa ganda:
MPS20N0040D pressureSensor(CLOCK_PIN_1, DATA_PIN_1, CLOCK_PIN_2, DATA_PIN_2);

void setup() {
  Serial.begin(9600);
  Serial.println(F("MPS20N0040D Venturi Meter Example"));
  
  // Inisialisasi sensor
  pressureSensor.begin();
  
  // Mengatur tipe venturi (SINGLE_PIPE atau DOUBLE_PIPE)
  pressureSensor.setVenturiType(DOUBLE_PIPE);
  
  // Mengatur geometri pipa (diameter dalam meter)
  // Contoh: pipa utama 20mm dan throat 10mm
  pressureSensor.setGeometry(0.020, 0.010);
  
  Serial.println(F("Sensor initialized"));
  Serial.println(F("Format: Pressure1 (Pa), Pressure2 (Pa), Difference (Pa), Flow Rate (L/min)"));
}

void loop() {
  // Membaca tekanan dari kedua sensor
  float pressure1 = pressureSensor.readPressure1();
  float pressure2 = pressureSensor.readPressure2();
  
  // Menghitung perbedaan tekanan
  float pressureDiff = pressureSensor.calculatePressureDifference();
  
  // Menghitung laju aliran (konversi m³/s ke L/min)
  float flowRate = pressureSensor.calculateFlowRate() * 60000.0; // m³/s * 1000 L/m³ * 60 s/min
  
  // Menampilkan hasil
  Serial.print(pressure1, 2);
  Serial.print(F(" Pa, "));
  Serial.print(pressure2, 2);
  Serial.print(F(" Pa, Diff: "));
  Serial.print(pressureDiff, 2);
  Serial.print(F(" Pa, Flow: "));
  Serial.print(flowRate, 4);
  Serial.println(F(" L/min"));
  
  delay(1000);  // Update setiap detik
}
