#include <Arduino.h>
#include "CQRobotTDS.h"
#include <WiFi.h>
#include <HTTPClient.h>

#define TDS_SENSOR_PIN 27
const int pinSensorTurbidity = 35;
const int ph_pin = 34;

void setup() {
  pinMode(ph_pin, INPUT);
  Serial.begin(115200);
  konfigurasiWiFi("Pinky", "Awalbulan");
}

void loop() {
  int nilaiADC = analogRead(pinSensorTurbidity);
  float nilaiNTU = konversiADCKeNTU(nilaiADC); // Nilai Turbidity

  Serial.println("=========================");
  Serial.print("Nilai NTU: ");
  Serial.println(nilaiNTU);

  float tdsValue = readTDS(nilaiADC); // Nilai TDS
  Serial.print("TDS Value: ");
  Serial.println(tdsValue);

  int nilai_analog_pH = analogRead(ph_pin); // Nilai pH
  Serial.print("Nilai ADC pH: ");
  Serial.println(nilai_analog_pH);

  float teganganPH = (3.3 / 4095.0) * nilai_analog_pH;
  Serial.print("Tegangan pH: ");
  Serial.println(teganganPH, 3);

  float pH_step = 0.0; // Variable untuk perhitungan pH (disesuaikan dengan kalibrasi)
  float phValue = 0.0; // Nilai pH Cairan

  // Perhitungan nilai pH
  // Disesuaikan dengan kalibrasi nilai pH yang tepat
  float PH4 = 3.16; // Perbaiki nilai PH4 sesuai kalibrasi
  float PH7 = 2.66; // Perbaiki nilai PH7 sesuai kalibrasi
  float PH9 = 2.33; // Perbaiki nilai PH9 sesuai kalibrasi

  pH_step = (PH4 - PH7) / 3;
  phValue = 7.00 - ((teganganPH - PH7) / pH_step);

  Serial.print("Nilai pH Cairan: ");
  Serial.println(phValue, 2);

  // Kirim nilai NTU, TDS, dan pH ke API PHP
  kirimKeAPI("http://192.168.1.2/sensor/api.php?nilaiNTU=" + String(nilaiNTU) + "&nilaiTDS=" + String(tdsValue) + "&nilaiPH=" + String(phValue));


  Serial.println("=========================");
  delay(5000);
}

float konversiADCKeNTU(int nilaiADC) {
  float faktorKonversi = 0.1;
  float offset = 0.0;
  float nilaiNTU = (float)nilaiADC * faktorKonversi + offset;
  return nilaiNTU;
}

float readTDS(int nilaiADC) {
  // Ganti nilai-nilai berikut dengan karakteristik sensor TDS Anda
  int nilaiMinADC = 1204; // Nilai ADC minimum yang dibaca dari sensor
  int nilaiMaxADC = 4095; // Nilai ADC maksimum yang dibaca dari sensor
  float tdsMinValue = 100.0; // Nilai TDS minimum yang diukur oleh sensor
  float tdsMaxValue = 500.0; // Nilai TDS maksimum yang diukur oleh sensor
  
  // Lakukan konversi nilai ADC ke nilai TDS menggunakan metode yang sesuai
  float tdsValue = map(nilaiADC, nilaiMinADC, nilaiMaxADC, tdsMinValue, tdsMaxValue);
  return tdsValue;
}

void konfigurasiWiFi(const char *ssid, const char *password) {
  Serial.print("Menghubungkan ke WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(5000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi tersambung");
  Serial.println("IP address: " + WiFi.localIP().toString());
}

void kirimKeAPI(String url) {
  HTTPClient httpClient;
  // Set URL API PHP
  httpClient.begin(url);
  // Lakukan permintaan HTTP GET
  int httpCode = httpClient.GET();
  // Periksa kode respons HTTP
  if (httpCode == 200) {
    // Data berhasil dikirim
    Serial.println("Data berhasil dikirim");
  } else {
    // Terjadi kesalahan
    Serial.println("Gagal mengirim data");
    Serial.println("Kode respons: " + String(httpCode));
  }
  // Tutup koneksi HTTP
  httpClient.end();
}
