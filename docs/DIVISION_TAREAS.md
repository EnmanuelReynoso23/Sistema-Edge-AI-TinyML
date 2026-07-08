# Division de Tareas — Proyecto 6
## Sistema Edge AI/TinyML para Mantenimiento Predictivo de Equipos

---

## Modulo 1: Adquisicion de Datos y Sistemas Embebidos (Hardware/IoT)

> Responsable de todo lo fisico: sensores, conexiones, lectura de datos crudos.

### Tareas:

- [ ] Disenar el circuito completo en Wokwi (ESP32, sensores, actuadores)
- [ ] Conectar y configurar el sensor DHT22 para lectura de temperatura y humedad
- [ ] Conectar y configurar el sensor MPU6050 (acelerometro I2C) para lectura de vibracion
- [ ] Conectar el potenciometro como simulador de intensidad de uso
- [ ] Cablear los 3 LEDs (verde, amarillo, rojo) con sus resistencias
- [ ] Cablear el buzzer para alarma sonora
- [ ] Cablear la pantalla OLED/LCD I2C para visualizacion
- [ ] Escribir las funciones de lectura de cada sensor (leerTemperatura(), leerVibracion(), leerIntensidad())
- [ ] Implementar filtro de suavizado (media movil de las ultimas N lecturas) para estabilizar datos
- [ ] Crear el archivo diagram.json de Wokwi con todas las conexiones
- [ ] Documentar la tabla de pines (que pin va a que componente)
- [ ] Documentar la lista de materiales con especificaciones

### Entregables:

| Entregable | Descripcion |
|---|---|
| diagram.json | Circuito completo en Wokwi |
| Funciones de lectura | Codigo que retorna valores limpios de cada sensor |
| Tabla de pines | Documento con todas las conexiones |
| Lista de materiales | Componentes y especificaciones |

---

## Modulo 2: Procesamiento Inteligente Local (Edge AI / TinyML)

> Responsable de la logica de clasificacion: tomar datos crudos y decidir el estado del equipo.

### Tareas:

- [ ] Definir los 3 estados del equipo: NORMAL, PREVENTIVO, FALLA_PROBABLE
- [ ] Establecer y justificar los umbrales de cada sensor:
  - Temperatura: a partir de cuantos grados C es preventivo y a partir de cuantos es falla
  - Vibracion: a partir de cuantos g es preventivo y a partir de cuantos es falla
  - Intensidad de uso: a partir de que porcentaje es preventivo y a partir de cual es falla
- [ ] Implementar clasificacion por reglas logicas (if/else basado en umbrales)
- [ ] Implementar clasificacion por puntaje ponderado (risk score):
  - Normalizar cada lectura al rango 0.0 a 1.0
  - Asignar pesos a cada variable (ej: temperatura 0.4, vibracion 0.35, intensidad 0.25)
  - Calcular risk_score como suma ponderada
  - Clasificar segun rangos del score
- [ ] Justificar los umbrales elegidos citando normas o estandares industriales (ej: ISO 10816 para vibracion, IEC 60085 para temperatura)
- [ ] Crear el diagrama de flujo de decision (flowchart) que muestre toda la logica
- [ ] Crear el diagrama de transicion de estados (state machine)
- [ ] Documentar ejemplos concretos con valores de sensores y el resultado de la clasificacion

### Entregables:

| Entregable | Descripcion |
|---|---|
| Codigo del clasificador | Funcion/clase que recibe datos y retorna el estado |
| Documento de umbrales | Justificacion tecnica de cada umbral |
| Diagrama de flujo | Flowchart de toda la logica de decision |
| Diagrama de estados | State machine NORMAL, PREVENTIVO, FALLA |
| Tabla de ejemplos | Valores de entrada y resultado esperado |

---

## Modulo 3: Interfaz de Usuario y Gestion del Mantenimiento

> Responsable de lo que el usuario ve y la experiencia: pantalla OLED, LEDs, buzzer.

### Tareas:

- [ ] Programar la pantalla OLED/LCD I2C para mostrar:
  - Estado actual del equipo (NORMAL / PREVENTIVO / FALLA PROBABLE)
  - Temperatura actual en grados C
  - Nivel de vibracion en g
  - Intensidad de uso en porcentaje
  - Puntaje de riesgo (risk score)
- [ ] Disenar el layout de la pantalla (que informacion va en que parte de la pantalla 128x64)
- [ ] Programar los LEDs indicadores:
  - Verde encendido = NORMAL
  - Amarillo encendido = PREVENTIVO
  - Rojo encendido + parpadeo = FALLA PROBABLE
- [ ] Programar el buzzer de alarma:
  - Sin sonido en NORMAL
  - Beep corto intermitente en PREVENTIVO (opcional)
  - Alarma continua/rapida en FALLA PROBABLE
- [ ] Implementar animaciones/transiciones en pantalla al cambiar de estado
- [ ] Proponer como se gestionaria el mantenimiento con este sistema:
  - Que haria un tecnico al ver cada alerta
  - Como se registrarian las intervenciones
  - Como se priorizan los equipos

### Entregables:

| Entregable | Descripcion |
|---|---|
| Codigo de pantalla | Funciones para mostrar info en OLED/LCD |
| Codigo de LEDs y buzzer | Control de indicadores segun estado |
| Mockup de pantalla | Diseno del layout de la pantalla OLED |
| Propuesta de gestion | Documento de como se usaria en la practica |

---

## Modulo 4: Telemetria Serial y Sistema de Alertas (Outputs/Dashboard)

> Responsable de la salida de datos por serial, registro de eventos y visualizacion externa.

### Tareas:

- [ ] Implementar la salida serial (telemetria) en formato CSV:
  - Formato: timestamp,temperatura,vibracion,intensidad,estado,risk_score
  - Baudrate: 115200
  - Enviar cada N segundos
- [ ] Implementar un sistema de log de alertas por serial:
  - Registrar cada cambio de estado con timestamp
  - Registrar cuanto tiempo lleva en cada estado
  - Registrar los picos maximos de cada sensor
- [ ] Crear un protocolo de comunicacion serial documentado (que formato tienen los datos, como parsearlos)
- [ ] Disenar/proponer un dashboard para visualizar los datos:
  - Puede ser: monitor serial, Excel, herramienta externa, o sketch de como seria
  - Graficas de temperatura, vibracion e intensidad en el tiempo
  - Indicador de estado actual
  - Historial de alertas
- [ ] Implementar contadores y estadisticas:
  - Total de lecturas realizadas
  - Cantidad de alertas generadas
  - Tiempo en cada estado
  - Promedio del risk score
- [ ] Documentar como leer e interpretar la salida serial

### Entregables:

| Entregable | Descripcion |
|---|---|
| Codigo de telemetria | Funciones de salida serial formateada |
| Protocolo serial | Documento del formato de datos |
| Log de alertas | Sistema de registro de eventos |
| Propuesta de dashboard | Diseno o prototipo de visualizacion |
| Estadisticas | Contadores y metricas del sistema |

---

## Resumen Visual

```
+----------------------------+     +----------------------------+
|  MODULO 1: HARDWARE/IoT    |     |  MODULO 2: EDGE AI/TinyML  |
|                            |     |                            |
|  - Circuito Wokwi          |---->|  - Clasificador            |
|  - Sensores (DHT22,        |     |  - Umbrales + justif.      |
|    MPU6050, Potenciometro)  |     |  - Diagrama de flujo       |
|  - Filtro de datos         |     |  - Risk score ponderado    |
|  - Lista de materiales     |     |  - Diagrama de estados     |
+----------------------------+     +----------------------------+
            |                                |
            v                                v
+----------------------------+     +----------------------------+
|  MODULO 3: INTERFAZ/UI     |     |  MODULO 4: TELEMETRIA      |
|                            |     |                            |
|  - Pantalla OLED/LCD       |     |  - Salida serial CSV       |
|  - LEDs (verde, amarillo,  |     |  - Log de alertas          |
|    rojo)                   |     |  - Protocolo serial        |
|  - Buzzer de alarma        |     |  - Propuesta dashboard     |
|  - Propuesta de gestion    |     |  - Estadisticas            |
+----------------------------+     +----------------------------+
```

---

## Dependencias entre Modulos

| Modulo | Depende de | Razon |
|---|---|---|
| Modulo 2 | Modulo 1 | Necesita los datos de sensores para clasificar |
| Modulo 3 | Modulo 2 | Necesita el estado clasificado para mostrar en pantalla/LEDs |
| Modulo 4 | Modulo 1 + 2 | Necesita datos crudos Y el estado para la telemetria |
| Modulo 1 | Ninguno | Es independiente, se puede empezar primero |

### Orden sugerido de trabajo:

1. Modulo 1 primero (sin sensores no hay nada)
2. Modulo 2 segundo (clasificar los datos)
3. Modulos 3 y 4 en paralelo (ambos consumen el resultado del clasificador)

---

## Integracion Final

Todos los modulos se unen en un solo archivo main.ino donde:

1. El Modulo 1 lee los sensores
2. El Modulo 2 clasifica el estado
3. El Modulo 3 muestra el resultado en pantalla/LEDs/buzzer
4. El Modulo 4 envia todo por serial y registra alertas
