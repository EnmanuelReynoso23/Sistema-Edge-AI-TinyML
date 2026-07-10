#ifndef TELEMETRIA_H
#define TELEMETRIA_H

#include <Arduino.h>

// CONFIGURACION 
// Cada cuanto se envia una linea DATA con las lecturas actuales (ajustable)
const unsigned long INTERVALO_TELEMETRIA_MS = 5000; // 5 segundos

// CONTROL INTERNO 
static unsigned long telemUltimoEnvioMs = 0;

//Hay que llamarlo una vez en setup(), despues de inicializarEstadisticas().
// Manda la cabecera CSV una sola vez, para que el que lea el puerto serial 
//sepa que representa cada columna de las lineas DATA.
void inicializarTelemetria() {
  Serial.println(F("HDR,timestamp,temperatura_c,vibracion_g,intensidad_pct,estado,risk_score"));
  telemUltimoEnvioMs = millis();
}

// Imprime una linea de datos CSV con la lectura actual
static void enviarLineaDatos(float temp, float vib, float uso, int estado, float riskScore) {
  Serial.print(F("DATA,"));
  Serial.print(statFormatearTimestamp(millis()));
  Serial.print(",");
  Serial.print(temp, 2);
  Serial.print(",");
  Serial.print(vib, 2);
  Serial.print(",");
  Serial.print(uso, 1);
  Serial.print(",");
  Serial.print(statNombreEstado(estado));
  Serial.print(",");
  Serial.println(riskScore, 3);
}

// Hay que llamarlo en cada loop(), despues de calcular estadoIA y riskScore.
// Solo manda una linea DATA cuando se cumple el intervalo configurado,
// para no poner full el puerto serial con una linea cada ~50 ms.
void actualizarTelemetria(float temp, float vib, float uso, int estado, float riskScore) {
  unsigned long ahora = millis();
  if (ahora - telemUltimoEnvioMs >= INTERVALO_TELEMETRIA_MS) {
    telemUltimoEnvioMs = ahora;
    enviarLineaDatos(temp, vib, uso, estado, riskScore);
  }
}

#endif
