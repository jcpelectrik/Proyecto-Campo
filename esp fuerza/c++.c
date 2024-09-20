#include <WiFi.h>
#include <FirebaseESP32.h>
#include <ArduinoHttpClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Configuración de WiFi
const char* ssid = "tu_SSID";
const char* password = "tu_contraseña";

// Configuración de Firebase
#define FIREBASE_HOST "jcpelectrik.firebaseio.com"
#define FIREBASE_AUTH "AIzaSyBTCKZE-jy1aGe_YqJY5WFFkh1Fa52ixQM"
FirebaseData firebaseData;

// Configuración del LCD
LiquidCrystal_I2C lcd(0x27, 16, 2); 

// IP de otros ESP32
const char* ipDominante = "192.168.1.1";
const char* ipMediciones = "192.168.1.3";

// Inicialización de WiFiClient y HttpClient
WiFiClient wifi;
HttpClient client = HttpClient(wifi, ipMediciones, 80);

void setup() {
  Serial.begin(115200);
  
  // Conexión WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }
  Serial.println("Conectado a WiFi");
  
  // Inicializar Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  // Inicializar LCD
  lcd.init();
  lcd.backlight();
  
  // Configuración del servidor web
  // ... (aquí va la configuración del servidor web)
}

void loop() {
  // Obtener datos de mediciones
  obtenerDatosMediciones();
  
  // Enviar estado de los relés al ESP32 dominante
  enviarEstadoReles();

  delay(10000); // Esperar 10 segundos
}

void obtenerDatosMediciones() {
  client.beginRequest();
  client.get("/data");
  client.endRequest();
  
  int statusCode = client.responseStatusCode();
  String response = client.responseBody();
  
  if (statusCode == 200) {
    Serial.println("Datos de mediciones: " + response);
    // Procesar datos y controlar relés
  } else {
    Serial.println("Error al obtener datos de mediciones: " + String(statusCode));
  }
}

void enviarEstadoReles() {
  String estadoReles = "{\"relay1\": true, \"relay2\": false}"; // Reemplazar con los estados reales
  Firebase.pushJSON(firebaseData, "/estadoReles", estadoReles);
}