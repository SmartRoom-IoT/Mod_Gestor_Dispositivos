#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <MQUnifiedsensor.h>

// WiFi
//const char* ssid = "sumothings";
//const char* password = "sum0th1ngs@manzab";
const char* ssid = "Redmi 10";
const char* password = "freider123";

// MQTT
const char* mqtt_server = "192.168.75.107";
const int mqtt_port = 1883;
const char* mqtt_topic = "casa/temperatura";
const char* client_id = "ESP8266_Rele_MQ2";

// Pines
#define RELAY_PIN D6
#define MQ2_PIN A0

// MQ2 Config
#define Board "ESP8266"
#define Voltage_Resolution 3.3
#define ADC_Bit_Resolution 10
#define RatioMQ2CleanAir 9.83
MQUnifiedsensor mq2(Board, Voltage_Resolution, ADC_Bit_Resolution, MQ2_PIN, "MQ-2");

// Umbral de gases
const float PPM_UMBRAL = 25.0;

// Estado del ventilador
bool ventiladorActivoPorGas = false;
bool ventiladorActivoPorMQTT = false;

WiFiClient espClient;
PubSubClient client(espClient);

// Mensaje MQTT recibido
void callback(char* topic, byte* payload, unsigned int length) {
  String mensaje = "";
  for (unsigned int i = 0; i < length; i++) {
    mensaje += (char)payload[i];
  }

  Serial.print("Mensaje MQTT: ");
  Serial.println(mensaje);

  if (mensaje == "ENCENDER") {
    ventiladorActivoPorMQTT = true;
  } else if (mensaje == "APAGAR") {
    ventiladorActivoPorMQTT = false;
  }

  actualizarVentilador();
}

// Encender o apagar ventilador según lógicas combinadas
void actualizarVentilador() {
  if (ventiladorActivoPorMQTT || ventiladorActivoPorGas) {
    digitalWrite(RELAY_PIN, HIGH);
    Serial.println("Ventilador ENCENDIDO");
  } else {
    digitalWrite(RELAY_PIN, LOW);
    Serial.println("Ventilador APAGADO");
  }
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando a WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado");
  Serial.print("IP local: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando al broker MQTT...");
    if (client.connect(client_id)) {
      Serial.println(" ¡Conectado!");
      client.subscribe(mqtt_topic);
    } else {
      Serial.print("Error: ");
      Serial.print(client.state());
      Serial.println(" - Reintentando en 5s");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  // Inicializar MQ2
  mq2.setRegressionMethod(1); 
  mq2.setA(574.25); mq2.setB(-2.222); 
  mq2.init();

  Serial.println("Calibrando MQ2... Mantén el sensor en aire limpio.");
  float calcR0 = 0;
  for (int i = 0; i < 10; i++) {
    mq2.update();
    calcR0 += mq2.calibrate(RatioMQ2CleanAir);
    delay(500);
  }
  mq2.setR0(calcR0 / 10.0);

  if (isnan(mq2.getR0())) {
    Serial.println("Error de calibración MQ2");
    while (1);
  }

  Serial.print("R0 calibrado: ");
  Serial.println(mq2.getR0());
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Leer sensor MQ2 cada 3 segundos
  static unsigned long lastRead = 0;
  unsigned long now = millis();
  if (now - lastRead > 3000) {
    lastRead = now;

    mq2.update();
    float ppm = mq2.readSensor();

    Serial.print("MQ2 PPM: ");
    Serial.println(ppm);

    ventiladorActivoPorGas = ppm > PPM_UMBRAL;
    actualizarVentilador();
  }
}
