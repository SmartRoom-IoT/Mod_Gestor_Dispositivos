# 🔥 Sistema de Ventilación Automática con MQ-2 y ESP8266

Este proyecto implementa un sistema de monitoreo y control automático de gases usando el sensor MQ-2, conectado a una placa ESP8266 y controlado a través del protocolo MQTT. Un ventilador es activado si se detecta una concentración de gases superior al umbral definido o por comando remoto vía MQTT. Ideal para espacios cerrados donde se necesita una ventilación automatizada por seguridad.

---

## 🧰 Requisitos de Hardware

- Placa ESP8266 (ej. NodeMCU)
- Sensor de gas MQ-2
- Módulo de relé
- Ventilador o extractor (cualquier carga controlable por relé)
- Fuente de alimentación 5V para relé/ventilador (según necesidad)
- Protoboard y cables de conexión
- Red WiFi disponible

---

## 📚 Librerías Necesarias
Asegúrate de tener instaladas las siguientes librerías desde el **Administrador de Bibliotecas** del Arduino IDE:

- **ESP8266WiFi**
- **PubSubClient**  
  [Repositorio](https://github.com/knolleary/pubsubclient)
- **MQUnifiedsensor**  
  [Repositorio](https://github.com/miguel5612/MQSensorsLib)

---

## ⚙️ Conexiones del Hardware

| Componente  | ESP8266 (NodeMCU) |
|-------------|------------------|
| MQ-2 AOUT   | A0               |
| Relé IN     | D6               |
| VCC (MQ-2 y Relé) | 3.3V / 5V (según componente) |
| GND         | GND              |

---

## 🛠️ Configuración del Código

Antes de subir el sketch, ajusta estos valores:

### 🔐 WiFi
```cpp
const char* ssid = "Redmi 10";
const char* password = "freider123";
```
---

## 📡 MQTT

```cpp
const char* mqtt_server = "192.168.75.107";
const int mqtt_port = 1883;
const char* mqtt_topic = "casa/temperatura";
const char* client_id = "ESP8266_Rele_MQ2";
````

## 🧠 Lógica del Sistema

* El sistema enciende el ventilador si:

  * El sensor MQ-2 detecta gases por encima del umbral, o

  * Se recibe el comando "ENCENDER" por MQTT.

* Se apaga si ambos criterios están desactivados.

* Se calibra automáticamente el sensor al iniciar en aire limpio.
