/**
 * @file MPS20N0040D.h
 * @author-team Nama Author 1 Ana Maulida (anamaulida@students.unnes.ac.id), 2. GALIH RIDHO UTOMO (g4lihru@students.unnes.ac.id)
 * @link GitHub Repositori: https://github.com/4211421036/mps20n0040d-sensor
 * @version 1.0
 * @date 2025-10-4 (Updated 2025-04-11)
 * 
 * @feature:
 * - Kisaran tekanan 0-40 kPa (lembar datasheet)
 * - Perhitungan laju aliran Venturi
 * - Perhitungan Tekanan aliran Venturi
 * - Support untuk konfigurasi Venturi pipa tunggal dan ganda
 * 
 * @ref:
 * 1. Lembar Data MPS20N0040D-S (e-radionica.com)
 * 2. Lembar Data HX710B (AVIA Semiconductor)
 * 
 */
#ifndef MPS20N0040D_H
#define MPS20N0040D_H

#include <Arduino.h>

// Mendefinisikan tipe-tipe Venturi yang didukung
enum VenturiType {
  SINGLE_PIPE,  // Konfigurasi dengan satu sensor/pipa
  DOUBLE_PIPE   // Konfigurasi dengan dua sensor/pipa
};

class MPS20N0040D {
  public:
    // Konstruktor untuk konfigurasi pipa tunggal
    MPS20N0040D(uint8_t pdSckPin, uint8_t doutPin);
    
    // Konstruktor untuk konfigurasi pipa ganda
    MPS20N0040D(uint8_t pdSckPin1, uint8_t doutPin1, uint8_t pdSckPin2, uint8_t doutPin2);
    
    // Fungsi inisialisasi
    void begin();
    
    // Menentukan tipe venturi (SINGLE_PIPE atau DOUBLE_PIPE)
    void setVenturiType(VenturiType type);
    
    // Mengatur geometri venturi dengan diameter pipa
    void setGeometry(float diameter1, float diameter2);
    
    // Membaca tekanan dari sensor pertama (atau satu-satunya jika SINGLE_PIPE)
    float readPressure1();
    
    // Membaca tekanan dari sensor kedua (hanya jika DOUBLE_PIPE)
    float readPressure2();
    
    // Menghitung perbedaan tekanan antara dua sensor/posisi
    float calculatePressureDifference();
    
    // Menghitung laju aliran berdasarkan konfigurasi venturi yang diatur
    float calculateFlowRate(float density = 1000.0);
    
    // Menghitung laju aliran dengan nilai perbedaan tekanan yang eksplisit
    float calculateFlowRateFromPressureDiff(float pressureDiff, float density = 1000.0);
    
  private:
    // Pin untuk sensor 1
    uint8_t _pdSckPin1, _doutPin1;
    
    // Pin untuk sensor 2 (hanya digunakan jika DOUBLE_PIPE)
    uint8_t _pdSckPin2, _doutPin2;
    
    // Tipe venturi
    VenturiType _venturiType;
    
    // Diameter pipa
    float _diameter1; // Diameter pipa utama
    float _diameter2; // Diameter throat/pipa kedua
    
    // Konstanta untuk perhitungan
    float _beta;      // Rasio diameter (d2/d1)
    float _area1;     // Luas penampang pipa 1
    float _area2;     // Luas penampang pipa 2
    
    // Membaca nilai ADC dari sensor
    long readADC(uint8_t pdSckPin, uint8_t doutPin);
    
    // Mengkonversi nilai ADC ke tegangan
    float adcToVoltage(long adcValue);
    
    // Mengkonversi tegangan ke nilai tekanan
    float voltageToPressure(float voltage);
    
    // Konstanta untuk kalibrasi sensor berdasarkan datasheet
    const float V_REF = 5.0;          // Tegangan referensi untuk ADC HX710 (5 volt)
    const float FS_OUTPUT = 0.1;      // Output maksimum sensor adalah 100mV (0.1V)
    const float PRESSURE_RANGE = 40000.0; // Rentang tekanan sensor 0-40 kPa (40000 Pascal)
};

#endif
