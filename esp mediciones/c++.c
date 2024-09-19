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
const char* ipFuerza = "192.168.1.2";

// Inicialización de WiFiClient y HttpClient
WiFiClient wifi;
HttpClient client = HttpClient(wifi, ipDominante, 80);

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
  
  // Configuración de sensores
  // ... (aquí va la configuración de los sensores)
}

void loop() {
  // Obtener datos de los sensores
  String datos = obtenerDatosSensores();
  
  // Enviar datos al ESP32 dominante y al ESP32 de fuerza
  enviarDatos(datos, ipDominante);
  enviarDatos(datos, ipFuerza);

  delay(10000); // Esperar 10 segundos
}

String obtenerDatosSensores() {
  // Leer datos de los sensores y devolverlos como JSON
  String datos = "{\"temperatura\": 23.5, \"humedad\": 60}"; // Reemplazar con la lectura real de los sensores
  return datos;
}

void enviarDatos(String datos, String ip) {
  client.beginRequest();
  client.post("/data");
  client.sendHeader("Content-Type", "application/json");
  client.beginBody();
  client.print(datos);
  client.endRequest();
  
  int statusCode = client.responseStatusCode();
  String response = client.responseBody();
  
  if (statusCode == 200) {
    Serial.println("Respuesta de " + ip + ": " + response);
  } else {
    Serial.println("Error al enviar datos a " + ip + ": " + String(statusCode));
  }
}