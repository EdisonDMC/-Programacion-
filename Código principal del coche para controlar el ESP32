#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include "index.h"

// Definición de comandos con enumeración para mayor claridad
enum Command {
  CMD_STOP = 0,
  CMD_FORWARD = 1,
  CMD_BACKWARD = 2,
  CMD_LEFT = 8,
  CMD_RIGHT = 4,
  CMD_UNKNOWN = 255
};

// Configuración de pines del L298N
constexpr uint8_t ENA_PIN = 14;
constexpr uint8_t IN1_PIN = 25;
constexpr uint8_t IN2_PIN = 33;
constexpr uint8_t IN3_PIN = 27;
constexpr uint8_t IN4_PIN = 26;
constexpr uint8_t ENB_PIN = 32;

// Configuración WiFi
const char* ssid = "Departamento1";
const char* password = "Alejandro123";

// Variables globales
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
bool isMoving = false;
Command lastCommand = CMD_STOP;

// Prototipos de funciones
void initMotorPins();
void connectToWiFi();
void handleWebSocketEvent(AsyncWebSocket*, AsyncWebSocketClient*, AwsEventType, void*, uint8_t*, size_t);
void processCommand(Command cmd);
void setMotorSpeed(uint8_t speed = 255); // Velocidad por defecto al máximo

// Funciones de control del coche
void CAR_moveForward();
void CAR_moveBackward();
void CAR_turnLeft();
void CAR_turnRight();
void CAR_stop();

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); } // Esperar a que se inicialice el puerto serial

  initMotorPins();
  setMotorSpeed(); // Inicializar motores a velocidad máxima

  connectToWiFi();

  // Iniciar mDNS
  if (!MDNS.begin("esp32-car")) {
    Serial.println("Error setting up MDNS responder!");
  } else {
    Serial.println("mDNS responder started. Access via esp32-car.local");
  }

  // Configuración del servidor web
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", HTML_CONTENT);
  });

  // Configuración WebSocket
  ws.onEvent(handleWebSocketEvent);
  server.addHandler(&ws);

  // Manejo de errores 404
  server.onNotFound([](AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
  });

  server.begin();
  Serial.println("Servidor HTTP y WebSocket iniciados");
}

void loop() {
  // Limpieza de clientes WebSocket desconectados
  ws.cleanupClients();

  // Pequeño delay para evitar sobrecarga de CPU
  delay(10);
}

// Implementación de funciones

void initMotorPins() {
  // Configurar pines de dirección
  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);
  pinMode(IN3_PIN, OUTPUT);
  pinMode(IN4_PIN, OUTPUT);
  
  // Configurar pines PWM (modo básico)
  pinMode(ENA_PIN, OUTPUT);
  pinMode(ENB_PIN, OUTPUT);
  
  // Detener motores al inicio
  CAR_stop();
  setMotorSpeed(255); // Velocidad máxima
  
  Serial.println("Motores inicializados (modo PWM básico)");
}

void connectToWiFi() {
  Serial.println("Conectando a WiFi...");
  
  // Configurar IP estática (opcional)
  /*
  IPAddress local_IP(192, 168, 1, 100);
  IPAddress gateway(192, 168, 1, 1);
  IPAddress subnet(255, 255, 255, 0);
  IPAddress primaryDNS(8, 8, 8, 8);
  
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS)) {
    Serial.println("STA Failed to configure");
  }
  */
  
  WiFi.begin(ssid, password);

  unsigned long startAttemptTime = millis();
  
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 20000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nNo se pudo conectar al WiFi. Reiniciando...");
    delay(1000);
    ESP.restart();
  }

  Serial.println("\nConectado a WiFi");
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Señal RSSI: ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");
}

void handleWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
                        AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("Cliente #%u conectado desde %s\n", client->id(), client->remoteIP().toString().c_str());
      client->text("Conectado al servidor WebSocket");
      break;
      
    case WS_EVT_DISCONNECT:
      Serial.printf("Cliente #%u desconectado\n", client->id());
      // Detener el carro si el cliente se desconecta
      if (ws.count() == 0) {
        CAR_stop();
      }
      break;
      
    case WS_EVT_DATA: {
      AwsFrameInfo *info = (AwsFrameInfo*)arg;
      if (info->final && info->index == 0 && info->len == len) {
        // Mensaje completo recibido
        char cmdChar = (char)data[0];
        if (cmdChar >= '0' && cmdChar <= '8') {
          Command cmd = static_cast<Command>(cmdChar - '0');
          Serial.printf("Comando válido recibido: %d\n", cmd);
          processCommand(cmd);
          client->text(String("ACK:") + String(cmd));
        } else {
          Serial.println("Comando no válido recibido");
          client->text("ERR:Comando no válido");
        }
      }
      break;
    }
    
    case WS_EVT_ERROR:
      Serial.printf("Error en WebSocket: %s\n", (char*)arg);
      break;
      
    case WS_EVT_PONG:
      // Manejo de keep-alive
      break;
  }
}

void processCommand(Command cmd) {
  // Solo procesar si es un comando nuevo
  if (cmd == lastCommand) return;
  
  lastCommand = cmd;
  
  switch (cmd) {
    case CMD_STOP:
      Serial.println("Deteniendo...");
      CAR_stop();
      isMoving = false;
      break;
      
    case CMD_FORWARD:
      Serial.println("Avanzando...");
      CAR_moveForward();
      isMoving = true;
      break;
      
    case CMD_BACKWARD:
      Serial.println("Retrocediendo...");
      CAR_moveBackward();
      isMoving = true;
      break;
      
    case CMD_LEFT:
      Serial.println("Girando izquierda...");
      CAR_turnLeft();
      isMoving = true;
      break;
      
    case CMD_RIGHT:
      Serial.println("Girando derecha...");
      CAR_turnRight();
      isMoving = true;
      break;
      
    default:
      Serial.println("Comando desconocido ignorado");
      break;
  }
}

// Funciones de control de motores mejoradas
void CAR_moveForward() {
  digitalWrite(IN1_PIN, HIGH);
  digitalWrite(IN2_PIN, LOW);
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, HIGH);
}

void CAR_moveBackward() {
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, HIGH);
  digitalWrite(IN3_PIN, HIGH);
  digitalWrite(IN4_PIN, LOW);
}

void CAR_turnLeft() {
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, HIGH);
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, HIGH); // Motor derecho en reversa para giro más pronunciado
}

void CAR_turnRight() {
  digitalWrite(IN1_PIN, HIGH);
  digitalWrite(IN2_PIN, LOW);
  digitalWrite(IN3_PIN, HIGH);
  digitalWrite(IN4_PIN, LOW); // Motor izquierdo en reversa para giro más pronunciado
}

void CAR_stop() {
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, LOW);
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, LOW);
}

void setMotorSpeed(uint8_t speed) {
  analogWrite(ENA_PIN, speed);
  analogWrite(ENB_PIN, speed);
  Serial.printf("Velocidad: %d/255\n", speed);
}
