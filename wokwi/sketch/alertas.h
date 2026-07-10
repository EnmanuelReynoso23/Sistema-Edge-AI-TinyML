#ifndef ALERTAS_H
#define ALERTAS_H

#include <Arduino.h>

// ESTADO INTERNO DEL LOG 
static int           alertEstadoAnterior  = -1; // -1 = aun no hay lectura previa
static unsigned long alertEntradaEstadoMs = 0;
static float alertPicoTemp = -1000.0f;
static float alertPicoVib  = -1000.0f;
static float alertPicoUso  = -1000.0f;

// Hay que llamarlo una vez en setup(), despues de inicializarEstadisticas().
void inicializarAlertas() {
  alertEstadoAnterior  = -1;
  alertEntradaEstadoMs = millis();
  alertPicoTemp = alertPicoVib = alertPicoUso = -1000.0f;
}

// Imprime la linea de alerta con la transicion, duracion y picos del
// segmento de estado que acaba de terminar.
static void enviarLineaAlerta(int estadoAnterior, int estadoNuevo, unsigned long duracionMs,
                               float pTemp, float pVib, float pUso, float riskScore) {
  Serial.print(F("ALERT,"));
  Serial.print(statFormatearTimestamp(millis()));
  Serial.print(",");
  Serial.print(statNombreEstado(estadoAnterior));
  Serial.print("->");
  Serial.print(statNombreEstado(estadoNuevo));
  Serial.print(",duracion_seg=");
  Serial.print(duracionMs / 1000.0, 1);
  Serial.print(",pico_temp=");
  Serial.print(pTemp, 2);
  Serial.print(",pico_vib=");
  Serial.print(pVib, 2);
  Serial.print(",pico_uso=");
  Serial.print(pUso, 1);
  Serial.print(",risk_score=");
  Serial.println(riskScore, 3);
}

// Hay que llamarlo en cada loop(), despues de calcular estadoIA y riskScore.
// Va acumulando los picos de cada sensor mientras el equipo permanece en
// el mismo estado; en cuanto detecta un cambio, cierra ese segmento,
// imprime la alerta y reinicia los picos para el nuevo estado.
void actualizarAlertas(float temp, float vib, float uso, int estado, float riskScore) {
  unsigned long ahora = millis();

  // primera lectura: solo arranca el seguimiento, no genera alerta
  if (alertEstadoAnterior == -1) {
    alertEstadoAnterior  = estado;
    alertEntradaEstadoMs = ahora;
    alertPicoTemp = temp;
    alertPicoVib  = vib;
    alertPicoUso  = uso;
    return;
  }

  // actualizar picos del segmento de estado actual
  if (temp > alertPicoTemp) alertPicoTemp = temp;
  if (vib  > alertPicoVib)  alertPicoVib  = vib;
  if (uso  > alertPicoUso)  alertPicoUso  = uso;

  // cambio de estado: cerrar el segmento anterior y loguear la alerta
  if (estado != alertEstadoAnterior) {
    unsigned long duracionMs = ahora - alertEntradaEstadoMs;
    enviarLineaAlerta(alertEstadoAnterior, estado, duracionMs,
                       alertPicoTemp, alertPicoVib, alertPicoUso, riskScore);

    alertEstadoAnterior  = estado;
    alertEntradaEstadoMs = ahora;
    alertPicoTemp = temp;
    alertPicoVib  = vib;
    alertPicoUso  = uso;
  }
}

#endif
