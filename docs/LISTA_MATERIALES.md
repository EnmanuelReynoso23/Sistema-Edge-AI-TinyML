# Lista de Materiales — Sistema Edge AI/TinyML

## Componentes principales

| # | Componente | Modelo | Cantidad | Especificaciones |
|---|---|---|---|---|
| 1 | Microcontrolador | ESP32 DevKit C V4 | 1 | WiFi + BT, 240 MHz, 30 pines, 3.3V lógica |
| 2 | Sensor temperatura/humedad | DHT22 (AM2302) | 1 | Temp: −40 a 80°C ±0.5°C, Humedad: 0-100% RH |
| 3 | Acelerómetro/Giroscopio | MPU6050 | 1 | 6 ejes, I2C (0x68), rango ±2g a ±16g |
| 4 | Pantalla OLED | SSD1306 128x64 | 1 | I2C (0x3C), 3.3V, 128x64 píxeles monocromo |
| 5 | Potenciómetro | 10 kΩ | 1 | Simulador de intensidad de uso (0-100%) |
| 6 | LED Verde | 5mm difuso | 1 | Estado NORMAL |
| 7 | LED Naranja | 5mm difuso | 1 | Estado PREVENTIVO |
| 8 | LED Rojo | 5mm difuso | 1 | Estado FALLA PROBABLE |
| 9 | Buzzer | Pasivo 5V | 1 | Alarma sonora, frecuencia 1000 Hz |
| 10 | Resistencia | 1 kΩ | 3 | Limitadoras de corriente para LEDs |

## Conexiones de alimentación

| Componente | Voltaje de operación | Fuente en el circuito |
|---|---|---|
| ESP32 | 3.3V / 5V (USB) | USB o externa |
| DHT22 | 3.3V – 5V | Pin 3V3 del ESP32 |
| MPU6050 | 3.3V | Pin 3V3 del ESP32 |
| OLED SSD1306 | 3.3V | Pin 3V3 del ESP32 |
| Potenciómetro | 3.3V | Pin 3V3 del ESP32 |
| LEDs | 3.3V (con resistencia 1kΩ) | GPIO ESP32 |
| Buzzer | 3.3V | GPIO ESP32 |

## Protocolo de comunicación por componente

| Componente | Protocolo | Velocidad / Dirección |
|---|---|---|
| DHT22 | 1-Wire digital | GPIO 14, entrada |
| MPU6050 | I2C | 400 kHz, SDA=21, SCL=22, dirección 0x68 |
| OLED SSD1306 | I2C | SDA=21, SCL=22, dirección 0x3C |
| Potenciómetro | ADC analógico | GPIO 34, resolución 12 bits (0-4095) |
| LEDs | GPIO digital | GPIOs 2, 4, 16, salida |
| Buzzer | GPIO PWM (tone) | GPIO 17, salida |
