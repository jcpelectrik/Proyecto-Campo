
#include "ESP32_NOW.h"
#include "WiFi.h"
#include <esp_mac.h>  // Para las macros MAC2STR y MACSTR

/* Definiciones */

#define ESPNOW_WIFI_CHANNEL 6

/* Clases */

// Es necesario crear una nueva clase que herede de la clase ESP_NOW_Peer.

class ESP_NOW_Broadcast_Peer : public ESP_NOW_Peer {
public:

  // Constructor de la clase que utiliza la dirección de difusión
  ESP_NOW_Broadcast_Peer(uint8_t channel, wifi_interface_t iface, const uint8_t *lmk) : ESP_NOW_Peer(ESP_NOW.BROADCAST_ADDR, channel, iface, lmk) {}

  // Destructor de la clase
  ~ESP_NOW_Broadcast_Peer() {
    remove();
  }

  // Función para inicializar correctamente el ESP-NOW y registrar el par de transmisión
  bool begin() {
    if (!ESP_NOW.begin() || !add()) {
      log_e("Failed to initialize ESP-NOW or register the broadcast peer");
      return false;
    }
    return true;
  }

  // Función para enviar un mensaje a todos los dispositivos dentro de la red
  bool send_message(const uint8_t *data, size_t len) {
    if (!send(data, len)) {
      log_e("Failed to broadcast message");
      return false;
    }
    return true;
  }
};

/* Variables globales */

uint32_t msg_count = 0;

// Crear un objeto de transmisión peer
ESP_NOW_Broadcast_Peer broadcast_peer(ESPNOW_WIFI_CHANNEL, WIFI_IF_STA, NULL);

/* Main */

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  // Inicializar el módulo Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.setChannel(ESPNOW_WIFI_CHANNEL);
  while (!WiFi.STA.started()) {
    delay(100);
  }

  Serial.println("ESP-NOW Example - Broadcast Master");
  Serial.println("Wi-Fi parameters:");
  Serial.println("  Mode: STA");
  Serial.println("  MAC Address: " + WiFi.macAddress());
  Serial.printf("  Channel: %d\n", ESPNOW_WIFI_CHANNEL);

  // Registrar el par de transmisión
  if (!broadcast_peer.begin()) {
    Serial.println("Failed to initialize broadcast peer");
    Serial.println("Reebooting in 5 seconds...");
    delay(5000);
    ESP.restart();
  }

  Serial.println("Configuración completa. Se transmiten mensajes cada 5 segundos.");
}

void loop() {
  // Transmitir un mensaje a todos los dispositivos dentro de la red
  char data[32];
  snprintf(data, sizeof(data), "Hello, World! y Adios Mundo #%lu", msg_count++);
  
  Serial.printf("Broadcasting mensaje: %s\n", data);

  if (!broadcast_peer.send_message((uint8_t *)data, sizeof(data))) {
    Serial.println("No se pudo transmitir el mensaje.");
  }

  delay(5000);
}