#include <WiFi.h>
#include "ESP32_NOW.h"
#include "WiFi.h"
#include <esp_mac.h>  // Para las macros MAC2STR y MACSTR
#include <vector>

/* Definiciones */

#define ESPNOW_WIFI_CHANNEL 6

/* Clases */

// Es necesario crear una nueva clase que herede de la clase ESP_NOW_Peer.

class ESP_NOW_Peer_Class : public ESP_NOW_Peer {
public:
  // Constructor de la clase
  ESP_NOW_Peer_Class(const uint8_t *mac_addr, uint8_t channel, wifi_interface_t iface, const uint8_t *lmk) : ESP_NOW_Peer(mac_addr, channel, iface, lmk) {}

  // Destructor de la clase
  ~ESP_NOW_Peer_Class() {}

  // Función para registrar el par maestro
  bool add_peer() {
    if (!add()) {
      log_e("Failed to register the broadcast peer");
      return false;
    }
    return true;
  }

  // Función para imprimir los mensajes recibidos del master
  void onReceive(const uint8_t *data, size_t len, bool broadcast) {
    Serial.printf("Received a message from master " MACSTR " (%s)\n", MAC2STR(addr()), broadcast ? "broadcast" : "unicast");
    Serial.printf("  Message: %s\n", (char *)data);
  }
};

/* Variables Glovales */

// Lista de todos los maestros. Se completará cuando se registre un nuevo maestro.
std::vector<ESP_NOW_Peer_Class> masters;

/* Devoluciones de llamadas */

// Se llama a la devolución de llamada cuando un par desconocido envía un mensaje
void register_new_master(const esp_now_recv_info_t *info, const uint8_t *data, int len, void *arg) {
  if (memcmp(info->des_addr, ESP_NOW.BROADCAST_ADDR, 6) == 0) {
    Serial.printf("Unknown peer " MACSTR " sent a broadcast message\n", MAC2STR(info->src_addr));
    Serial.println("Registering the peer as a master");

    ESP_NOW_Peer_Class new_master(info->src_addr, ESPNOW_WIFI_CHANNEL, WIFI_IF_STA, NULL);

    masters.push_back(new_master);
    if (!masters.back().add_peer()) {
      Serial.println("Failed to register the new master");
      return;
    }
  } else {
    // El esclavo solo recibirá mensajes de difusión.
    log_v("Recibí un mensaje del maestro " MACSTR, MAC2STR(info->src_addr));
    log_v("Igorning the message");
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }
  conectarWiFi();
  Serial.println("ESP-NOW Example - Broadcast Slave");
  Serial.println("Wi-Fi parameters:");
  Serial.println("  Mode: STA");
  Serial.println("  MAC Address: " + WiFi.macAddress());
  Serial.printf("  Channel: %d\n", ESPNOW_WIFI_CHANNEL);

  // Inicializar el protocolo ESP-NOW
  if (!ESP_NOW.begin()) {
    Serial.println("No se pudo inicializar ESP-NOW");
    Serial.println("Reiniciando en 5 segundos...");
    delay(5000);
    ESP.restart();
  }

  // Registrar la nueva devolución de llamada de pares
  ESP_NOW.onNewPeer(register_new_master, NULL);

  Serial.println("Configuración completa. Esperando que un maestro transmita un mensaje...");
}
void loop() {
}
void conectarWiFi() {
   const char* ssid = "WiFimoviljavi";
   const char* password = "carrionp";
    Serial.println("Estado inicial del Wi-Fi:");
    WiFi.disconnect(true);
    delay(1000);
    WiFi.mode(WIFI_STA);
    WiFi.setChannel(ESPNOW_WIFI_CHANNEL);
    Serial.println("Iniciando conexión a Wi-Fi...");
    WiFi.begin(ssid, password);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 30) {
      delay(1000);
      Serial.print("Intento ");
      Serial.print(attempts + 1);
      Serial.println(" de conexión a Wi-Fi...");
      attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Conexión exitosa");
      Serial.print("Dirección IP: ");
      Serial.println(WiFi.localIP());
    } else {
      Serial.println("Error: No se pudo conectar a Wi-Fi.");
    }
}