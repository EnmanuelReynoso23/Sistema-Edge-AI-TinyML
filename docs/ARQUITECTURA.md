# Arquitectura del Sistema

El sistema se compone de los siguientes módulos, que interactúan entre sí dentro de la plataforma ESP32:

## 1. Hardware/IoT
Encargado de la adquisición de señales de los sensores (DHT22, MPU6050, Potenciómetro).

## 2. Edge AI / TinyML
Procesa los datos adquiridos y calcula el "Risk Score" para asignar uno de los 3 estados: NORMAL, PREVENTIVO, FALLA_PROBABLE.

## 3. UI e Interfaz
Recibe el estado calculado y actualiza la pantalla OLED, activa los LEDs correspondientes y acciona el buzzer si es necesario.

## 4. Telemetría
Recopila los datos y el estado y los envía a través de UART para monitoreo externo.
