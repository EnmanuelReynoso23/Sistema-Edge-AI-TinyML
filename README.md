# Sistema Edge AI/TinyML para Mantenimiento Predictivo de Equipos

## Descripcion del Problema

En laboratorios y areas tecnologicas, los equipos pueden presentar fallas por vibracion, temperatura elevada o uso inadecuado. Este proyecto propone un prototipo que detecte patrones de riesgo y clasifique el estado del equipo antes de una falla critica, utilizando tecnicas de Edge AI y TinyML sobre un microcontrolador ESP32.

## Objetivo General

Desarrollar un sistema embebido inteligente capaz de monitorear variables fisicas de un equipo (temperatura, vibracion, intensidad de uso), clasificar su estado en tiempo real y generar alertas preventivas para evitar fallas criticas.

## Objetivos Especificos

- Adquirir datos de sensores de temperatura, vibracion e intensidad de uso en tiempo real.
- Implementar un clasificador local (Edge AI) que determine el estado del equipo en tres niveles: NORMAL, PREVENTIVO y FALLA PROBABLE.
- Mostrar el estado del equipo en una pantalla OLED/LCD con indicadores visuales (LEDs) y sonoros (buzzer).
- Transmitir telemetria por puerto serial para monitoreo externo y registro de alertas.
- Documentar el flujo de decision y justificar los umbrales utilizados con base en estandares industriales.

## Arquitectura del Sistema

El sistema se divide en 4 modulos:

```
+----------------------------+     +----------------------------+
|  MODULO 1: HARDWARE/IoT    |     |  MODULO 2: EDGE AI/TinyML  |
|                            |     |                            |
|  Sensores (DHT22,          |---->|  Clasificador de estado    |
|  MPU6050, Potenciometro)   |     |  Reglas logicas + score    |
|  Filtrado de senales       |     |  Umbrales justificados     |
+----------------------------+     +----------------------------+
            |                                |
            v                                v
+----------------------------+     +----------------------------+
|  MODULO 3: INTERFAZ/UI     |     |  MODULO 4: TELEMETRIA      |
|                            |     |                            |
|  Pantalla OLED/LCD         |     |  Salida serial CSV         |
|  LEDs indicadores          |     |  Log de alertas            |
|  Buzzer de alarma          |     |  Estadisticas y dashboard  |
+----------------------------+     +----------------------------+
```

### Modulo 1: Adquisicion de Datos y Sistemas Embebidos (Hardware/IoT)

Responsable del circuito fisico en Wokwi, la conexion de sensores, lectura de datos crudos y filtrado de senales.

### Modulo 2: Procesamiento Inteligente Local (Edge AI / TinyML)

Responsable de la logica de clasificacion del estado del equipo mediante reglas logicas y un sistema de puntaje ponderado (risk score).

### Modulo 3: Interfaz de Usuario y Gestion del Mantenimiento

Responsable de la visualizacion del estado en pantalla OLED, control de LEDs indicadores y buzzer de alarma.

### Modulo 4: Telemetria Serial y Sistema de Alertas (Outputs/Dashboard)

Responsable de la transmision de datos por puerto serial, registro de eventos y propuesta de dashboard de monitoreo.

## Lista de Materiales (Wokwi)

| Componente | Funcion |
|---|---|
| ESP32 DevKit V1 | Microcontrolador principal |
| Sensor DHT22 | Lectura de temperatura y humedad |
| Sensor MPU6050 | Acelerometro/giroscopio para vibracion |
| Potenciometro | Simulacion de intensidad de uso |
| Pantalla OLED SSD1306 128x64 | Visualizacion de estado y datos |
| LED Verde | Indicador de estado NORMAL |
| LED Amarillo | Indicador de estado PREVENTIVO |
| LED Rojo | Indicador de estado FALLA PROBABLE |
| Buzzer | Alarma sonora de mantenimiento |
| Resistencias 220 ohm (x3) | Proteccion para LEDs |
| Resistencia 10K ohm | Pull-up para DHT22 |

## Estados del Equipo

| Estado | Condicion | Indicador Visual | Indicador Sonoro |
|---|---|---|---|
| NORMAL | Todos los parametros dentro de rango seguro | LED verde encendido | Sin sonido |
| PREVENTIVO | Al menos un parametro en rango de alerta | LED amarillo encendido | Beep intermitente (opcional) |
| FALLA PROBABLE | Al menos un parametro en rango critico | LED rojo parpadeando | Alarma continua |

## Herramientas Utilizadas

- Wokwi (simulador de circuitos)
- Arduino IDE o PlatformIO
- ESP32 DevKit V1
- Edge Impulse (opcion avanzada para entrenamiento de modelos)
- Monitor serial para telemetria

## Estructura del Repositorio

```
Sistema-Edge-AI-TinyML/
|-- src/                    # Codigo fuente del firmware
|   |-- main.ino            # Sketch principal
|   |-- config.h             # Configuracion de pines y umbrales
|   |-- classifier.h         # Clasificador Edge AI
|   |-- classifier.cpp       # Implementacion del clasificador
|-- wokwi/                  # Archivos de simulacion Wokwi
|   |-- diagram.json         # Circuito del simulador
|-- docs/                   # Documentacion del proyecto
|   |-- FLUJO_DECISION.md    # Diagrama y justificacion de umbrales
|   |-- ARQUITECTURA.md      # Arquitectura detallada por modulos
|   |-- PROPUESTA_VALOR.md   # Propuesta de valor del sistema
|-- DIVISION_TAREAS.md      # Division de tareas por modulos
|-- README.md               # Este archivo
|-- LICENSE                 # Licencia del proyecto
```

## Como Ejecutar

1. Abrir el proyecto en Wokwi (https://wokwi.com)
2. Cargar el archivo `wokwi/diagram.json` para el circuito
3. Cargar el codigo fuente desde `src/main.ino`
4. Iniciar la simulacion
5. Abrir el monitor serial (115200 baud) para ver la telemetria
6. Ajustar el potenciometro y observar los cambios de estado

## Licencia

MIT License - Ver archivo LICENSE para mas detalles.
