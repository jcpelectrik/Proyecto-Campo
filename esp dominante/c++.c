#include <WiFi.h>
#include <FirebaseESP32.h>
#include <ArduinoHttpClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WebServer.h>

// Configuración de WiFi
const char* ssid = "";
const char* password = "";

// Configuración de Firebase
#define FIREBASE_HOST "control-de-jardin.firebaseio.com"
#define FIREBASE_AUTH "apiKey"
FirebaseData firebaseData;

// Configuración del LCD
LiquidCrystal_I2C lcd(0x27, 16, 2); 

// Configuración del servidor público
const char* Pagina = "agrosensores.azurewebsites.net";
const int port = 443;
const char* resource = "/api/Medicion/mediciones";

// IP de otros ESP32
const char* ipFuerza = "";
const char* ipMediciones = "";

// Inicialización de WiFiClient y HttpClient
WiFiClient wifi;
HttpClient client = HttpClient(wifi, Pagina, port);

//configuracion del servidor
WebServer server(80);

void setup() {
  Serial.begin(115200);
  
  // Conexión WiFi
  Serial.println("Estado inicial del Wi-Fi:");
  WiFi.disconnect(true);
  delay(1000);
  WiFi.mode(WIFI_STA);
  Serial.println("Iniciando conexión a Wi-Fi...");
  WiFi.begin(ssid, password);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a Wi-Fi...");
    attempts++;
    //printWiFiStatus();
    if (attempts >= 30) {
      Serial.println("Error: No se pudo conectar a Wi-Fi.");
      Serial.println("Reiniciando el módulo Wi-Fi...");
      WiFi.disconnect(true);
      delay(1000);
      WiFi.begin(ssid, password);
      attempts = 0;
    }
  }
  Serial.println("Conexión exitosa");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());
  
  // Inicializar Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  // Inicializar LCD
  lcd.init();
  lcd.backlight();
  lcd.print("IP: ");
  lcd.print(WiFi.localIP().toString().c_str());
  
  // Configuración del servidor web
  server.on("/", HTTP_GET, handleRoot);
  server.on("/Rele1on", HTTP_GET, handleRele1On);
  server.on("/Rele1off", HTTP_GET, handleRele1Off);
  server.on("/Rele2on", HTTP_GET, handleRele2On);
  server.on("/Rele2off", HTTP_GET, handleRele2Off);
  server.on("/Rele3on", HTTP_GET, handleRele3On);
  server.on("/Rele3off", HTTP_GET, handleRele3Off);
  server.on("/Rele4on", HTTP_GET, handleRele4On);
  server.on("/Rele4off", HTTP_GET, handleRele4Off);
  server.on("/Rele5on", HTTP_GET, handleRele5On);
  server.on("/Rele5off", HTTP_GET, handleRele5Off);
  server.on("/Rele6on", HTTP_GET, handleRele6On);
  server.on("/Rele6off", HTTP_GET, handleRele6Off);
  server.on("/Rele7on", HTTP_GET, handleRele7On);
  server.on("/Rele7off", HTTP_GET, handleRele7Off);
  server.on("/Rele8on", HTTP_GET, handleRele8On);
  server.on("/Rele8off", HTTP_GET, handleRele8Off);
}

void loop() {
  // Obtener datos de mediciones
  obtenerDatosMediciones();

  // Enviar datos al servidor público
  enviarDatosServidorPublico();

  // Obtener órdenes del servidor público y enviarlas a ESP32 de fuerza
  obtenerYEnviarOrdenes();

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
    Firebase.pushJSON(firebaseData, "/mediciones", response);
  } else {
    Serial.println("Error al obtener datos de mediciones: " + String(statusCode));
  }
}

void enviarDatosServidorPublico() {
  client.beginRequest();
  client.post("/api/data");
  client.sendHeader("Content-Type", "application/json");
  client.beginBody();
  
  String postData = "{\"data\": \"example\"}"; // Reemplazar con los datos reales
  client.print(postData);
  
  client.endRequest();
  
  int statusCode = client.responseStatusCode();
  String response = client.responseBody();
  
  if (statusCode == 200) {
    Serial.println("Respuesta del servidor: " + response);
  } else {
    Serial.println("Error al enviar datos al servidor público: " + String(statusCode));
  }
}

void obtenerYEnviarOrdenes() {
  client.beginRequest();
  client.post("/command");
  client.sendHeader("Content-Type", "application/json");
  client.beginBody();
  
  String commandData = "{\"relay\": 1, \"state\": \"on\"}"; // Reemplazar con las órdenes reales
  client.print(commandData);
  
  client.endRequest();
  
  int statusCode = client.responseStatusCode();
  String response = client.responseBody();
  
  if (statusCode == 200) {
    Serial.println("Respuesta del ESP32 de fuerza: " + response);
  } else {
    Serial.println("Error al enviar órdenes al ESP32 de fuerza: " + String(statusCode));
  }
}
void handleRoot(){
  <!DOCTYPE html>
<html>
<head>
    <link rel="stylesheet" href="css.css">
    <title> Control de jardín </title>
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 0;
            padding: 20px;
            background-color: #f0f0f0;
        }
        header {
            text-align: center;
            margin-bottom: 20px;
        }
        .vertical-section {
            display: flex;
            flex-direction: column;
            align-items: center;
            margin-bottom: 20px;
        }
        .boton-redondeado {
            background-color: #008CBA;
            border: none;
            color: white;
            padding: 10px 32px;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 16px;
            margin: 4px 2px;
            cursor: pointer;
            border-radius: 12px;
        }
        .container1 {
            display: flex;
            flex-wrap: wrap;
            justify-content: space-around;
            gap: 20px;
        }
        .rele-container {
            display: flex;
            flex-wrap: wrap;
            justify-content: space-around;
            gap: 20px;
        }
        .espacio {
            background-color: white;
            border-radius: 10px;
            padding: 20px;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
            text-align: center;
            flex: 1 1 calc(25% - 40px);
            box-sizing: border-box;
        }
        @media (max-width: 768px) {
            .espacio {
                flex: 1 1 calc(50% - 20px);
            }
        }
        @media (max-width: 600px) {
            .imagen {
                background-image: url('/logo1.png');
            }
        }
        @media (min-width: 601px) {
            .imagen {
                background-image: url('/logo1.png');
            }
        }
        .status {
            width: 50px;
            height: 50px;
            border-radius: 50%;
            margin: 10px auto;
        }
        .red {
            background-color: red;
        }
        .green {
            background-color: green;
        }
    </style>
</head>
<body>
    <header>
        <h1><img src="/logo1.png" alt="no se ha encontrado imagen" style="width: 50%; height: auto;"></h1>        
        <div class="container1">
            <div class="espacio">
                <h2> Control de luces </h2>
                <button class="boton-redondeado" onclick="sendData('luces_porche_encender')">Encender luces porche</button>
                <button class="boton-redondeado" onclick="sendData('luces_porche_apagar')">Apagar luces porche</button>
                <button class="boton-redondeado" onclick="sendData('luces_camino_encender')">Encender luces camino</button>
                <button class="boton-redondeado" onclick="sendData('luces_camino_apagar')">Apagar luces camino</button>
                <h4> Control de piscina </h4>
                <button class="boton-redondeado" onclick="sendData('depuradora_encender')">Encender depuradora</button>
                <button class="boton-redondeado" onclick="sendData('depuradora_apagar')">Apagar depuradora</button>
                <h3> Lecturas a tiempo real </h3>
                <p>PH: <span id='ph'>0</span> %</p>
                <p>Cloro: <span id='cloro'>0</span> %</p>           
            </div>
            <div class="espacio">
            <h2> Control de riego </h2>
            <h4> Control de seccion 1 </h4>
            <button class="boton-redondeado" onclick="sendData('seccion1_abrir')">Abrir llave</button>
            <button class="boton-redondeado" onclick="sendData('seccion1_cerrar')">Cerrar llave</button>
            <h4> Control de seccion 2 </h4>
            <button class="boton-redondeado" onclick="sendData('seccion2_abrir')">Abrir llave</button>
            <button class="boton-redondeado" onclick="sendData('seccion2_cerrar')">Cerrar llave</button>
            <h4> Control de seccion 3 </h4>
            <button class="boton-redondeado" onclick="sendData('seccion3_abrir')">Abrir llave</button>
            <button class="boton-redondeado" onclick="sendData('seccion3_cerrar')">Cerrar llave</button>
            <h4> Huerto </h4>
            <button class="boton-redondeado" onclick="sendData('huerto_abrir')">Abrir llave</button>
            <button class="boton-redondeado" onclick="sendData('huerto_cerrar')">Cerrar llave</button>
            <h4> Pozo </h4>
            <button class="boton-redondeado" onclick="sendData('pozo_encender')">Encender Pozo</button>
            <button class="boton-redondeado" onclick="sendData('pozo_apagar')">Apagar Pozo</button>
            <h3> Lecturas a tiempo real </h3>
            <p>Temperatura: <span id='temperatura'>0</span> ºC</p>
            <p>Humedad: <span id='humedad'>0</span> %</p>
        </div>
    </header>
    <div class="rele-container">
        <div class="espacio">
            <h5>Relé 1 porche</h5>
            <div id="rele1-status" class="status red"></div>
        </div>
        <div class="espacio">
            <h5>Relé 2 camino</h5>
            <div id="rele2-status" class="status red"></div>
        </div>
        <div class="espacio">
            <h5>Relé 3 depuradora</h5>
            <div id="rele3-status" class="status red"></div>
        </div>
        <div class="espacio">
            <h5>Relé 4 depuradora automatico</h5>
            <div id="rele4-status" class="status red"></div>
        </div>
        <div class="espacio">
            <h5>Relé 5 seccion 1</h5>
            <div id="rele5-status" class="status red"></div>
        </div>
        <div class="espacio">
            <h5>Relé 6 seccion 1 automatico</h5>
            <div id="rele6-status" class="status red"></div>
        </div>
        <div class="espacio">
            <h5>Relé 7 seccion 2</h5>
            <div id="rele7-status" class="status red"></div>
        </div>
        <div class="espacio">
            <h5>Relé 8 seccion 2 automatico</h5>
            <div id="rele8-status" class="status red"></div>
        </div>
        <div class="espacio">
            <h5>Relé 9 seccion 3</h5>
            <div id="rele9-status" class="status red"></div>
        </div>
        <div class="espacio">
            <h5>Relé 10 seccion 3 automatico</h5>
            <div id="rele10-status" class="status red"></div>
        </div>
        <div class="espacio">
            <h5>Relé 11 Huerto</h5>
            <div id="rele11-status" class="status red"></div>
        </div>
        <div class="espacio">
            <h5>Relé 12 Huerto automatico</h5>
            <div id="rele12-status" class="status red"></div>
        </div>
        <div class="espacio">
            <h5>Relé 13 Pozo</h5>
            <div id="rele13-status" class="status red"></div>
        </div>
        <div class="espacio">
            <h5>Relé 14 Pozo automatico</h5>
            <div id="rele14-status" class="status red"></div>
        </div>
        <div class="espacio">
            <h5>Relé 15</h5>
            <div id="rele15-status" class="status red"></div>
        </div>
    </div>
    <script>
        async function sendData(command) {
            try {
                const response = await fetch(`/action?command=${command}`);
                if (!response.ok) {
                    console.error('Error en la solicitud:', response.status);
                }
            } catch (error) {
                console.error('Error en la solicitud:', error);
            }
        }

        async function handleData() {
            try {
                const response = await fetch('/data');
                if (response.ok) {
                    const datos = await response.json();
                    document.getElementById('humedad').innerText = datos.humedad;
                    document.getElementById('temperatura').innerText = datos.temperatura;
                    document.getElementById('ph').innerText = datos.ph;
                    document.getElementById('cloro').innerText = datos.cloro;

                    actualizarEstadoRele('rele1', datos.rele1);
                    actualizarEstadoRele('rele2', datos.rele2);
                    actualizarEstadoRele('rele3', datos.rele3);
                    actualizarEstadoRele('rele4', datos.rele4);
                    actualizarEstadoRele('rele5', datos.rele5);
                    actualizarEstadoRele('rele6', datos.rele6);
                    actualizarEstadoRele('rele7', datos.rele7);
                    actualizarEstadoRele('rele8', datos.rele8);
                    actualizarEstadoRele('rele9', datos.rele9);
                    actualizarEstadoRele('rele10', datos.rele10);
                    actualizarEstadoRele('rele11', datos.rele11);
                    actualizarEstadoRele('rele12', datos.rele12);
                    actualizarEstadoRele('rele13', datos.rele13);
                    actualizarEstadoRele('rele14', datos.rele14);
                    actualizarEstadoRele('rele15', datos.rele15);
                } else {
                    console.error('Error en la solicitud:', response.status);
                }
            } catch (error) {
                console.error('Error en la solicitud:', error);
            }
        }

        function actualizarEstadoRele(releId, estado) {
            const statusElement = document.getElementById(`${releId}-status`);
            if (estado) {
                statusElement.classList.remove('red');
                statusElement.classList.add('green');
            } else {
                statusElement.classList.remove('green');
                statusElement.classList.add('red');
            }
        }

        document.addEventListener('DOMContentLoaded', () => {
            handleData();
            setInterval(handleData, 5000); // Actualiza los datos cada 5 segundos
        });
    </script>
</body>
</html>
)rawliteral";
}
