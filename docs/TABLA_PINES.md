# Tabla de Pines — ESP32 DevKit C V4

## Modulo 1: Sensores de entrada

| Componente | Pin del componente | GPIO ESP32 | Tipo de señal | Color de cable |
|---|---|---|---|---|
| DHT22 | VCC | 3V3 | Alimentación | Rojo |
| DHT22 | SDA (datos) | GPIO 14 | Digital entrada | Verde |
| DHT22 | GND | GND | Tierra | Negro |
| MPU6050 | VCC | 3V3 | Alimentación | Rojo |
| MPU6050 | SDA | GPIO 21 | I2C datos | Verde |
| MPU6050 | SCL | GPIO 22 | I2C reloj | Verde |
| MPU6050 | GND | GND | Tierra | Negro |
| Potenciómetro | VCC | 3V3 | Alimentación | Rojo |
| Potenciómetro | SIG (señal) | GPIO 34 | Analógica entrada | Verde |
| Potenciómetro | GND | GND | Tierra | Negro |

## Modulo 3: Actuadores de salida

| Componente | Pin del componente | GPIO ESP32 | Tipo de señal | Color de cable |
|---|---|---|---|---|
| LED Verde | Ánodo (+) | GPIO 2 | Digital salida | Verde |
| LED Verde | Cátodo (−) | GND (via R1) | Tierra | Verde |
| LED Naranja | Ánodo (+) | GPIO 4 | Digital salida | Verde |
| LED Naranja | Cátodo (−) | GND (via R2) | Tierra | Verde |
| LED Rojo | Ánodo (+) | GPIO 16 | Digital salida | Verde |
| LED Rojo | Cátodo (−) | GND (via R3) | Tierra | Verde |
| Buzzer | Pin + | GPIO 17 | Digital salida | Rojo |
| Buzzer | Pin − | GND | Tierra | Negro |
| OLED SSD1306 | VCC | 3V3 | Alimentación | Rojo |
| OLED SSD1306 | GND | GND | Tierra | Negro |
| OLED SSD1306 | SDA | GPIO 21 | I2C datos | Azul |
| OLED SSD1306 | SCL | GPIO 22 | I2C reloj | Dorado |

## Resumen de pines usados

| GPIO | Función | Dirección |
|---|---|---|
| GPIO 2 | LED Verde | Salida |
| GPIO 4 | LED Naranja | Salida |
| GPIO 14 | DHT22 datos | Entrada |
| GPIO 16 | LED Rojo | Salida |
| GPIO 17 | Buzzer | Salida |
| GPIO 21 | I2C SDA (OLED + MPU6050) | I2C |
| GPIO 22 | I2C SCL (OLED + MPU6050) | I2C |
| GPIO 34 | Potenciómetro señal | Entrada analógica |

> **Nota:** GPIO 21 y GPIO 22 son el bus I2C compartido entre la pantalla OLED (dirección 0x3C) y el MPU6050 (dirección 0x68).
