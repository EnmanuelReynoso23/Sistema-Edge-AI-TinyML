# Flujo de Decision y Justificacion de Umbrales

## Sistema de Clasificacion IA - Mantenimiento Predictivo

Este documento detalla la logica utilizada por el clasificador inteligente (Modulo 2)
para determinar el estado del equipo en tiempo real.

---

## 1. Umbrales del Sistema

### Temperatura (Sensor DHT22) — Norma: IEC 60085

| Estado | Rango | Justificacion |
|--------|-------|---------------|
| NORMAL | < 50 C | Operacion segura dentro de clase de aislamiento A (105 C max). Un equipo con temperatura ambiente/superficie por debajo de 50 C opera dentro de margenes seguros. |
| PREVENTIVO | 50 - 75 C | Zona de alerta. Se acerca al limite de clase B (130 C). Indica posible falta de ventilacion, sobrecarga termica o necesidad de mantenimiento del sistema de refrigeracion. |
| FALLA PROBABLE | > 75 C | Riesgo de degradacion del aislamiento electrico. Posible dano termico a componentes internos si no se interviene. |

### Vibracion (Sensor MPU6050) — Norma: ISO 10816

| Estado | Rango | Justificacion |
|--------|-------|---------------|
| NORMAL | < 2.0 g | Vibracion aceptable para maquinas rotativas clase I-II. Operacion estable sin desgaste acelerado. |
| PREVENTIVO | 2.0 - 5.0 g | Vibracion elevada. Indica posible desbalance, desalineacion o desgaste de rodamientos. Requiere inspeccion y posible lubricacion. |
| FALLA PROBABLE | > 5.0 g | Vibracion severa. Riesgo de dano mecanico inminente: rotura de rodamientos, grietas en ejes, solturas estructurales. |

### Intensidad de Uso (Potenciometro) — Practicas de Ingenieria Industrial

| Estado | Rango | Justificacion |
|--------|-------|---------------|
| NORMAL | < 60% | Carga dentro de la capacidad nominal del equipo. Operacion sostenible a largo plazo. |
| PREVENTIVO | 60 - 85% | Sobrecarga moderada. Acorta la vida util de componentes mecanicos y electricos. Se recomienda reducir carga o programar mantenimiento. |
| FALLA PROBABLE | > 85% | Sobrecarga severa. El equipo opera por encima de su capacidad disenada. Alto riesgo de falla por fatiga mecanica o termica. |

---

## 2. Risk Score Ponderado

El clasificador calcula un puntaje de riesgo combinado usando los tres sensores:

```
risk_score = (temp_normalizada x 0.40) + (vib_normalizada x 0.35) + (uso_normalizado x 0.25)
```

### Pesos y Justificacion

| Variable | Peso | Razon |
|----------|------|-------|
| Temperatura | 0.40 | Factor mas critico: el dano termico es irreversible y afecta aislamiento electrico (IEC 60085) |
| Vibracion | 0.35 | Segundo factor: indica desgaste mecanico progresivo que puede escalar rapidamente (ISO 10816) |
| Intensidad de uso | 0.25 | Factor contribuyente: acelera el desgaste pero no causa falla inmediata por si solo |

### Umbrales del Score

| Risk Score | Estado | Accion |
|------------|--------|--------|
| < 0.45 | NORMAL | Sin intervencion necesaria |
| 0.45 - 0.74 | PREVENTIVO | Inspeccion visual, programar mantenimiento al final del turno |
| >= 0.75 | FALLA PROBABLE | Parar la maquina con urgencia |

### Normalizacion

Cada lectura se normaliza al rango [0.0, 1.0] usando min-max scaling:

```
valor_normalizado = (valor - minimo) / (maximo - minimo)

Rangos de normalizacion:
  Temperatura: 20 C (min) a 100 C (max)
  Vibracion:   0.0 g (min) a 10.0 g (max)
  Uso:         0% (min) a 100% (max)
```

---

## 3. Diagrama de Flujo del Clasificador

```
                    LECTURA DE SENSORES
                          |
                +---------+---------+
                |         |         |
            Temperatura Vibracion   Uso
                |         |         |
                +----+----+----+----+
                     |         |
              Normalizar   Verificar umbrales
              (0.0 - 1.0)  individuales
                     |         |
              Calcular     Algun sensor
              Risk Score   en zona FALLA?
                     |         |
                     |    Si --+--> FALLA PROBABLE
                     |         |
                     |    Algun sensor
                     |    en zona PREV?
                     |         |
                     |    Si --+--> PREVENTIVO
                     |         |
              Score >= 0.75? --+--> FALLA PROBABLE
                     |
              Score >= 0.45? --+--> PREVENTIVO
                     |
                  NORMAL
```

---

## 4. Diagrama de Transicion de Estados

```
  +----------+     temp>50 OR vib>2 OR uso>60     +-------------+
  |          |  OR score >= 0.45                   |             |
  |  NORMAL  | ----------------------------------> | PREVENTIVO  |
  |  (LED    |                                     | (LED        |
  |  verde)  | <---------------------------------- | amarillo)   |
  |          |   todos los parametros normales      |             |
  +----------+   Y score < 0.45                    +-------------+
       ^                                                 |
       |                                                 |
       |     todos normales       temp>75 OR vib>5 OR uso>85
       |     Y score < 0.45      OR score >= 0.75
       |                                                 |
       |                                                 v
       |                                          +--------------+
       +----------------------------------------- | FALLA        |
                                                   | PROBABLE     |
                                                   | (LED rojo)   |
                                                   +--------------+
```

---

## 5. Protocolo de Mantenimiento Predictivo

### Protocolo por Sonido y LEDs

| Estado | LED | Sonido | Accion Requerida |
|--------|-----|--------|-----------------|
| NORMAL | Verde encendido | Sin sonido | Ninguna intervencion necesaria |
| PREVENTIVO | Amarillo encendido | Beep de 500ms cada 60 segundos | Inspeccion visual inmediata. Al terminar turno: revisar ventilacion, lubricacion de piezas |
| FALLA PROBABLE | Rojo parpadeando (200ms) | Alarma continua alternante | Parar la maquina con urgencia para evitar dano irreparable |

### Registro de Intervenciones

1. Cuando el sistema detecta cambio a PREVENTIVO o FALLA PROBABLE:
   - Se genera automaticamente un registro/ticket
   - El tecnico documenta la intervencion realizada a detalle
   - Una vez solucionado, se marca como "Completado"
   - El hardware vuelve a leer parametros en estado NORMAL

### Priorizacion de Equipos (si hay multiples alertas)

| Prioridad | Criterio |
|-----------|----------|
| 1 (Maxima) | Equipos en FALLA PROBABLE — riesgo de dano inminente |
| 2 | Equipos en PREVENTIVO con Risk Score mas alto o acercandose al umbral de falla |
| 3 | Equipos en PREVENTIVO recien activados |

---

## 6. Tabla de Ejemplos

| Temp (C) | Vib (g) | Uso (%) | Risk Score | Estado | Explicacion |
|----------|---------|---------|------------|--------|-------------|
| 30.0 | 0.5 | 30.0 | 0.1425 | NORMAL | Todos los parametros en zona segura |
| 45.0 | 1.0 | 50.0 | 0.2850 | NORMAL | Cercano al limite pero aun seguro |
| 55.0 | 3.0 | 70.0 | 0.4550 | PREVENTIVO | Temperatura y vibracion elevadas |
| 60.0 | 4.0 | 75.0 | 0.5275 | PREVENTIVO | Multiples parametros en zona de alerta |
| 80.0 | 6.0 | 90.0 | 0.7350 | FALLA PROBABLE | Todos los sensores en zona critica |
| 76.0 | 1.0 | 40.0 | 0.3150 | FALLA PROBABLE | Temperatura supera umbral critico (>75 C) |
| 40.0 | 6.0 | 30.0 | 0.3850 | FALLA PROBABLE | Vibracion supera umbral critico (>5 g) |

---

## 7. Modelo de IA Utilizado

| Parametro | Valor |
|-----------|-------|
| Algoritmo | DecisionTreeClassifier (scikit-learn) |
| Profundidad maxima | 6 niveles |
| Muestras minimas por hoja | 20 |
| Dataset de entrenamiento | 15,000 ejemplos sinteticos |
| Division train/test | 80% / 20% |
| Accuracy en test | 99.97% |
| Exportacion | Reglas if/else en C++ (clasificador.h) |

El DecisionTree es ideal para TinyML porque:
- Las reglas se exportan directamente a if/else en C++ puro
- No requiere bibliotecas externas en el microcontrolador
- Ejecucion en microsegundos en el ESP32
- El modelo ocupa menos de 1 KB de memoria Flash
