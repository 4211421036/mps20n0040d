/**
 * @file MPS20N0040D.cpp
 * @author-team Nama Author 1 Ana Maulida (anamaulida@students.unnes.ac.id), 2. GALIH RIDHO UTOMO (g4lihru@students.unnes.ac.id)
 * @link GitHub Repository: https://github.com/4211421036/mps20n0040d-sensor
 * 
 * @brief Implementasi driver sensor tekanan MPS20N0040D-S dengan dukungan venturi dua pipa.
 * 
 * @details
 * 1. Konversi ADC:
 * - Menggunakan protokol HX710B (24-bit)
 * - Penguatan tetap 128 (datasheet HX710 hal.1)
 * 
 * 2. Perhitungan Tekanan:
 * - Model linier: P = (V_out / V_fs) * P_range
 * 
 * 3. Aliran Venturi:
 * - Implementasi persamaan Bernoulli (fluida inkompresibel)
 * - Konfigurasi dinamis rasio beta (d2/d1)
 * - Dukungan untuk konfigurasi venturi pipa tunggal dan ganda
 * 
 * @result pengujian:
 * - Akurasi: ± 0,5% FS (dengan kalibrasi)
 * - Sampling Rate: 10 Hz (default HX710)
 */

#include "MPS20N0040D.h"

// Konstruktor - Untuk konfigurasi pipa tunggal
MPS20N0040D::MPS20N0040D(uint8_t pdSckPin, uint8_t doutPin) {
  _pdSckPin1 = pdSckPin;
  _doutPin1 = doutPin;
  _venturiType = SINGLE_PIPE;  // Default ke konfigurasi pipa tunggal
  
  // Nilai default untuk geometri pipa
  _diameter1 = 0.020;  // 20mm
  _diameter2 = 0.010;  // 10mm
  _beta = _diameter2 / _diameter1;
  _area1 = PI * pow(_diameter1/2, 2);
  _area2 = PI * pow(_diameter2/2, 2);
}

// Konstruktor - Untuk konfigurasi pipa ganda
MPS20N0040D::MPS20N0040D(uint8_t pdSckPin1, uint8_t doutPin1, uint8_t pdSckPin2, uint8_t doutPin2) {
  _pdSckPin1 = pdSckPin1;
  _doutPin1 = doutPin1;
  _pdSckPin2 = pdSckPin2;
  _doutPin2 = doutPin2;
  _venturiType = DOUBLE_PIPE;  // Set ke konfigurasi pipa ganda
  
  // Nilai default untuk geometri pipa
  _diameter1 = 0.020;  // 20mm
  _diameter2 = 0.010;  // 10mm
  _beta = _diameter2 / _diameter1;
  _area1 = PI * pow(_diameter1/2, 2);
  _area2 = PI * pow(_diameter2/2, 2);
}

// Fungsi untuk memulai komunikasi dengan sensor
void MPS20N0040D::begin() {
  pinMode(_pdSckPin1, OUTPUT);
  pinMode(_doutPin1, INPUT);
  digitalWrite(_pdSckPin1, LOW);
  
  if (_venturiType == DOUBLE_PIPE) {
    pinMode(_pdSckPin2, OUTPUT);
    pinMode(_doutPin2, INPUT);
    digitalWrite(_pdSckPin2, LOW);
  }
}

// Mengatur tipe venturi
void MPS20N0040D::setVenturiType(VenturiType type) {
  _venturiType = type;
}

// Mengatur geometri venturi
void MPS20N0040D::setGeometry(float diameter1, float diameter2) {
  _diameter1 = diameter1;
  _diameter2 = diameter2;
  _beta = _diameter2 / _diameter1;
  _area1 = PI * pow(_diameter1/2, 2);
  _area2 = PI * pow(_diameter2/2, 2);
}

// Membaca ADC dari sensor yang ditentukan
long MPS20N0040D::readADC(uint8_t pdSckPin, uint8_t doutPin) {
  // Menunggu hingga chip siap mengirim data
  unsigned long startTime = millis();
  while (digitalRead(doutPin)) {
    // Timeout setelah 100ms untuk mencegah program terjebak jika sensor tidak merespons
    if (millis() - startTime > 100) {
      return 0;
    }
  }
  
  // Membaca 24-bit data
  long count = 0;
  for (uint8_t i = 0; i < 24; i++) {
    digitalWrite(pdSckPin, HIGH);
    delayMicroseconds(1);
    count = count << 1;
    digitalWrite(pdSckPin, LOW);
    delayMicroseconds(1);
    if (digitalRead(doutPin)) count++;
  }
  
  // Pulsa ke-25 untuk mode
  digitalWrite(pdSckPin, HIGH);
  delayMicroseconds(1);
  digitalWrite(pdSckPin, LOW);
  
  return count ^ 0x800000;
}

// Konversi ADC ke tegangan
float MPS20N0040D::adcToVoltage(long adcValue) {
  return (adcValue / 8388608.0) * V_REF / 128.0;
}

// Konversi tegangan ke tekanan
float MPS20N0040D::voltageToPressure(float voltage) {
  return (voltage / FS_OUTPUT) * PRESSURE_RANGE;
}

// Membaca tekanan dari sensor 1
float MPS20N0040D::readPressure1() {
  long raw = readADC(_pdSckPin1, _doutPin1);
  float voltage = adcToVoltage(raw);
  return voltageToPressure(voltage);
}

// Membaca tekanan dari sensor 2
float MPS20N0040D::readPressure2() {
  if (_venturiType != DOUBLE_PIPE) {
    return 0.0;  // Return 0 jika bukan konfigurasi pipa ganda
  }
  
  long raw = readADC(_pdSckPin2, _doutPin2);
  float voltage = adcToVoltage(raw);
  return voltageToPressure(voltage);
}

// Menghitung perbedaan tekanan antara dua posisi
float MPS20N0040D::calculatePressureDifference() {
  float p1 = readPressure1();
  
  if (_venturiType == DOUBLE_PIPE) {
    float p2 = readPressure2();
    return p1 - p2;
  } else {
    // Untuk konfigurasi pipa tunggal, asumsikan tekanan atmosfer di throat (perkiraan)
    return p1 - 101325.0;  // 101325 Pa adalah tekanan atmosfer standar
  }
}

// Menghitung laju aliran berdasarkan konfigurasi yang diatur
float MPS20N0040D::calculateFlowRate(float density) {
  float pressureDiff = calculatePressureDifference();
  return calculateFlowRateFromPressureDiff(pressureDiff, density);
}

// Menghitung laju aliran dari perbedaan tekanan yang diberikan
float MPS20N0040D::calculateFlowRateFromPressureDiff(float pressureDiff, float density) {
  // Persamaan Venturi: Q = A2 * sqrt(2 * ΔP / (ρ * (1 - (A2/A1)²)))
  // atau Q = A1 * sqrt(2 * ΔP / (ρ * (1 - β⁴)))
  
  if (pressureDiff <= 0) {
    return 0.0;  // Pastikan pressureDiff positif untuk perhitungan yang valid
  }
  
  // Formula venturi perbaikan dengan discharge coefficient (Cd ≈ 0.98 untuk venturi standar)
  const float Cd = 0.98;
  
  // Akar dari perbedaan tekanan dibagi dengan faktor yang tergantung pada dimensi pipa
  float flowRateFactor = sqrt(2 * pressureDiff / (density * (1 - pow(_beta, 4))));
  
  // Debit (m³/s) = Area penampang * kecepatan * koefisien discharge
  return Cd * _area2 * flowRateFactor;
}
