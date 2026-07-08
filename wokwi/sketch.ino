// --- DEFINICIÓN DE PINES ---
const int LED_VERDE = 2;
const int LED_AMARILLO = 4;
const int LED_ROJO = 16;
const int PIN_BUZZER = 17;
const int PIN_POT = 34; // Pin del potenciómetro

// --- VARIABLES PARA EL ESTADO: FALLA PROBABLE ---
unsigned long tiempoAnteriorFalla = 0;
const long intervaloFalla = 200; // 200 ms intermitente constante
bool estadoAlarmaFalla = false;

// --- VARIABLES PARA EL ESTADO: PREVENTIVO ---
unsigned long tiempoAnteriorPreventivo = 0;
bool estadoBuzzerPreventivo = false;
const long tiempoSilencioPreventivo = 60000; // 60 segundos en silencio
const long tiempoBeepPreventivo = 500;      // Un "beep" de 500 ms

void setup() {
  Serial.begin(115200);

  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_AMARILLO, OUTPUT);
  pinMode(LED_ROJO, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_AMARILLO, LOW);
  digitalWrite(LED_ROJO, LOW);
  noTone(PIN_BUZZER);
}

void loop() {
  // 1. Leer el potenciómetro (0 a 4095) y convertirlo a porcentaje (0 a 100%)
  int lectura = analogRead(PIN_POT);
  float porcentaje = (lectura / 4095.0) * 100.0;
  
  // Capturar el tiempo actual para las alarmas asíncronas
  unsigned long tiempoActual = millis();

  // 2. LÓGICA DE ESTADOS Y ACTUADORES
  if (porcentaje < 45.0) {
    // --- ESTADO: NORMAL ---
    digitalWrite(LED_VERDE, HIGH);
    digitalWrite(LED_AMARILLO, LOW);
    digitalWrite(LED_ROJO, LOW);
    noTone(PIN_BUZZER); 
    
    // Reiniciamos la variable por si venía sonando del estado anterior
    estadoBuzzerPreventivo = false; 
    
  } else if (porcentaje >= 45.0 && porcentaje < 75.0) {
    // --- ESTADO: PREVENTIVO (Buzzer esporádico) ---
    digitalWrite(LED_VERDE, LOW);
    digitalWrite(LED_AMARILLO, HIGH);
    digitalWrite(LED_ROJO, LOW);
    
    // Lógica asimétrica: Comprueba si está sonando o en silencio
    if (estadoBuzzerPreventivo) {
      // Si está sonando, espera 200ms y lo apaga
      if (tiempoActual - tiempoAnteriorPreventivo >= tiempoBeepPreventivo) {
        tiempoAnteriorPreventivo = tiempoActual;
        estadoBuzzerPreventivo = false;
        noTone(PIN_BUZZER);
      }
    } else {
      // Si está en silencio, espera 3000ms (3 segundos) y hace el pitido
      if (tiempoActual - tiempoAnteriorPreventivo >= tiempoSilencioPreventivo) {
        tiempoAnteriorPreventivo = tiempoActual;
        estadoBuzzerPreventivo = true;
        tone(PIN_BUZZER, 1000); 
      }
    }
    
  } else {
    // --- ESTADO: FALLA PROBABLE (Alarma continua) ---
    digitalWrite(LED_VERDE, LOW);
    digitalWrite(LED_AMARILLO, LOW);
    
    // Alarma intermitente rápida (simétrica 200ms ON / 200ms OFF)
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

  // Pequeño retraso de estabilidad general
  delay(10); 
}