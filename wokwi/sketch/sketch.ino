#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <math.h>

// --- DEFINICIÓN DE PANTALLA OLED ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
// Declaración del objeto display (conectado por I2C a los pines SDA=21, SCL=22)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// --- DEFINICIÓN DE PINES ---
const int LED_VERDE = 2;
const int LED_AMARILLO = 4;
const int LED_ROJO = 16;
const int PIN_BUZZER = 17;
const int PIN_POT = 34;
// DHT22 — pin de datos
const int PIN_DHT = 14;

// --- OBJETOS DE SENSORES ---
DHT dht(PIN_DHT, DHT22);
Adafruit_MPU6050 mpu;
bool mpuDisponible = false;

// --- FILTRO DE MEDIA MÓVIL ---
// Ventana circular de 10 muestras para estabilizar lecturas ruidosas
#define FILTER_SIZE 10
float bufTemp[FILTER_SIZE]; int idxTemp = 0, cntTemp = 0;
float bufVib [FILTER_SIZE]; int idxVib  = 0, cntVib  = 0;
float bufUso [FILTER_SIZE]; int idxUso  = 0, cntUso  = 0;

// --- VARIABLES PARA EL ESTADO: FALLA PROBABLE ---
unsigned long tiempoAnteriorFalla = 0;
const long intervaloFalla = 200;
bool estadoAlarmaFalla = false;

// --- VARIABLES PARA EL ESTADO: PREVENTIVO ---
unsigned long tiempoAnteriorPreventivo = 0;
bool estadoBuzzerPreventivo = false;
const long tiempoSilencioPreventivo = 60000; // 60 segundos en silencio
const long tiempoBeepPreventivo = 500;      // Un "beep" de 500 ms

void setup() {
  Serial.begin(115200);

  // Inicializar pines
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_AMARILLO, OUTPUT);
  pinMode(LED_ROJO, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);

  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_AMARILLO, LOW);
  digitalWrite(LED_ROJO, LOW);
  noTone(PIN_BUZZER);

  // Inicializar bus I2C (SDA=21, SCL=22) antes de cualquier dispositivo I2C
  Wire.begin(21, 22);

  // Inicializar Pantalla OLED
  // El 0x3C es la dirección I2C estándar para estas pantallas
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Fallo al iniciar la pantalla OLED"));
    for (;;); // Detener el código si no encuentra la pantalla
  }

  // Limpiar pantalla al inicio
  display.clearDisplay();
  display.display();

  // Inicializar DHT22
  dht.begin();

  // Inicializar MPU6050 (dirección por defecto 0x68)
  mpuDisponible = mpu.begin();
  if (mpuDisponible) {
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  } else {
    Serial.println(F("MPU6050 no encontrado — vibracion en 0"));
  }

  // Pre-cargar buffers con valores seguros para que la media no arranque distorsionada
  for (int i = 0; i < FILTER_SIZE; i++) {
    bufTemp[i] = 25.0;
    bufVib[i]  = 0.0;
    bufUso[i]  = 0.0;
  }
  cntTemp = cntVib = cntUso = FILTER_SIZE;
}

void loop() {
  float porcentaje = leerIntensidad();

  unsigned long tiempoActual = millis();
  String estadoTexto = ""; // Variable para enviarle el texto a la pantalla

  // --- LÓGICA DE ESTADOS ---
  if (porcentaje < 45.0) {
    estadoTexto = "NORMAL";
    digitalWrite(LED_VERDE, HIGH);
    digitalWrite(LED_AMARILLO, LOW);
    digitalWrite(LED_ROJO, LOW);
    noTone(PIN_BUZZER);
    estadoBuzzerPreventivo = false;

  } else if (porcentaje >= 45.0 && porcentaje < 75.0) {
    estadoTexto = "PREVENTIVO";
    digitalWrite(LED_VERDE, LOW);
    digitalWrite(LED_AMARILLO, HIGH);
    digitalWrite(LED_ROJO, LOW);

    if (estadoBuzzerPreventivo) {
      if (tiempoActual - tiempoAnteriorPreventivo >= tiempoBeepPreventivo) {
        tiempoAnteriorPreventivo = tiempoActual;
        estadoBuzzerPreventivo = false;
        noTone(PIN_BUZZER);
      }
    } else {
      if (tiempoActual - tiempoAnteriorPreventivo >= tiempoSilencioPreventivo) {
        tiempoAnteriorPreventivo = tiempoActual;
        estadoBuzzerPreventivo = true;
        tone(PIN_BUZZER, 1000);
      }
    }

  } else {
    estadoTexto = "FALLA PROBABLE";
    digitalWrite(LED_VERDE, LOW);
    digitalWrite(LED_AMARILLO, LOW);

    if (tiempoActual - tiempoAnteriorFalla >= intervaloFalla) {
      tiempoAnteriorFalla = tiempoActual;
      estadoAlarmaFalla = !estadoAlarmaFalla;
      digitalWrite(LED_ROJO, estadoAlarmaFalla ? HIGH : LOW);

      if (estadoAlarmaFalla) {
        tone(PIN_BUZZER, 1000);
      } else {
        noTone(PIN_BUZZER);
      }
    }
  }

  // --- ecturas reales de sensores ---
  float tempPrueba = leerTemperatura();
  float vibPrueba  = leerVibracion();
  int riesgoPrueba = porcentaje * 0.8; // Cálculo simple simulado

  // Llama a la función que dibuja en la pantalla
  actualizarPantalla(estadoTexto, porcentaje, tempPrueba, vibPrueba, riesgoPrueba);

  delay(50); // Pequeño retraso para que la pantalla no parpadee demasiado
}

// =========================================================================
// FUNCIONES DE LECTURA DE SENSORES
// =========================================================================

// Inserta valor en el buffer circular y devuelve el promedio de las últimas N lecturas.
float agregarFiltro(float* buf, int& idx, int& cnt, float valor) {
  buf[idx] = valor;
  idx = (idx + 1) % FILTER_SIZE;
  if (cnt < FILTER_SIZE) cnt++;
  float suma = 0.0;
  for (int i = 0; i < cnt; i++) suma += buf[i];
  return suma / cnt;
}

// Lee temperatura del DHT22, devuelve grados Celsius filtrados.
float leerTemperatura() {
  float temp = dht.readTemperature();
  if (isnan(temp) || temp < -40.0 || temp > 125.0) {
    // Lectura inválida: reutiliza el último valor del buffer
    int ultimoIdx = (idxTemp - 1 + FILTER_SIZE) % FILTER_SIZE;
    temp = (cntTemp > 0) ? bufTemp[ultimoIdx] : 25.0;
  }
  return agregarFiltro(bufTemp, idxTemp, cntTemp, temp);
}

// Lee vibración del MPU6050 como magnitud del vector de aceleración en g,
// restando la componente estática de la gravedad (1 g).
float leerVibracion() {
  if (!mpuDisponible) return 0.0;
  sensors_event_t a, g, t;
  mpu.getEvent(&a, &g, &t);
  float ax = a.acceleration.x / 9.81;
  float ay = a.acceleration.y / 9.81;
  float az = a.acceleration.z / 9.81;
  float magnitud  = sqrt(ax * ax + ay * ay + az * az);
  float vibracion = abs(magnitud - 1.0);
  return agregarFiltro(bufVib, idxVib, cntVib, vibracion);
}

// Lee el potenciómetro como simulador de intensidad de uso. Retorna 0–100 %.
float leerIntensidad() {
  int   raw        = analogRead(PIN_POT);
  float porcentaje = (raw / 4095.0) * 100.0;
  return agregarFiltro(bufUso, idxUso, cntUso, porcentaje);
}

// =========================================================================
// FUNCION INDEPENDIENTE PARA DIBUJAR EN LA PANTALLA
// =========================================================================
void actualizarPantalla(String estado, float uso, float temp, float vib, int riesgo) {
  display.clearDisplay(); // Borra lo que había antes

  // 1. TÍTULO / ESTADO CENTRAL
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("ESTADO: ");
  display.print(estado);

  // Línea separadora
  display.drawLine(0, 12, 128, 12, SSD1306_WHITE);

  // 2. COLUMNA IZQUIERDA (Temp y Vibración)
  display.setCursor(0, 20);
  display.print("Temp: ");
  display.print(temp, 1); // El ", 1" es para mostrar solo un decimal
  display.print(" C");

  display.setCursor(0, 40);
  display.print("Vib: ");
  display.print(vib, 1);
  display.print(" g");

  // 3. COLUMNA DERECHA (Uso y Riesgo)
  display.setCursor(70, 20);
  display.print("Uso: ");
  display.print(uso, 0);
  display.print("%");

  display.setCursor(70, 40);
  display.print("Riesgo:");
  display.print(riesgo);

  // Mostrar en la pantalla física
  display.display();
}