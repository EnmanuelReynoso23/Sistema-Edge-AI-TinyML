#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

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

  // Inicializar Pantalla OLED
  // El 0x3C es la dirección I2C estándar para estas pantallas
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Fallo al iniciar la pantalla OLED"));
    for(;;); // Detener el código si no encuentra la pantalla
  }
  
  // Limpiar pantalla al inicio
  display.clearDisplay();
  display.display();
}

void loop() {
  int lectura = analogRead(PIN_POT);
  float porcentaje = (lectura / 4095.0) * 100.0;
  
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

  // --- VARIABLES SIMULADAS PARA LA PANTALLA ---
  // Como tus compañeros aún no programan el DHT22 y el MPU6050, 
  // pondremos valores de prueba que ellos luego cambiarán por las variables reales.
  float tempPrueba = 35.5; 
  float vibPrueba = 0.5;
  int riesgoPrueba = porcentaje * 0.8; // Cálculo simple simulado

  // Llamamos a la función que dibuja en la pantalla
  actualizarPantalla(estadoTexto, porcentaje, tempPrueba, vibPrueba, riesgoPrueba);

  delay(50); // Pequeño retraso para que la pantalla no parpadee demasiado (flickering)
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