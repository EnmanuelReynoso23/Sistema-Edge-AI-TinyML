#ifndef ESTADISTICAS_H
#define ESTADISTICAS_H

#include <Arduino.h>

// se imprime el bloque de estadisticas
const unsigned long INTERVALO_ESTADISTICAS_MS = 30000; // 30 segundos

//  CONTADORES  ACUMULADORES
static unsigned long statTotalLecturas          = 0;
static unsigned long statTotalAlertas           = 0;
static unsigned long statTiempoNormalMs         = 0;
static unsigned long statTiempoPreventivoMs     = 0;
static unsigned long statTiempoFallaMs          = 0;
static double        statSumaRiskScore          = 0.0;

// CONTROL
static unsigned long statUltimaActualizacionMs  = 0;
static unsigned long statUltimoReporteMs        = 0;
static int           statEstadoAnterior         = -1; // -1 = aun no hay lectura previa

// Convierte milisegundos desde el arranque a formato HH:MM:SS
static String statFormatearTimestamp(unsigned long ms) {
  unsigned long totalSeg = ms / 1000UL;
  unsigned int horas    = totalSeg / 3600UL;
  unsigned int minutos  = (totalSeg % 3600UL) / 60UL;
  unsigned int segundos = totalSeg % 60UL;
  char buf[12];
  sprintf(buf, "%02u:%02u:%02u", horas, minutos, segundos);
  return String(buf);
}

// Nombre legible del estado (reutiliza los enum de clasificador.h)
static const char* statNombreEstado(int estado) {
  switch (estado) {
    case ESTADO_NORMAL:     return "NORMAL";
    case ESTADO_PREVENTIVO: return "PREVENTIVO";
    default:                return "FALLA PROBABLE";
  }
}

// hay que llamarlo una vez en setup(), despues de Serial.begin().
void inicializarEstadisticas() {
  unsigned long ahora = millis();
  statUltimaActualizacionMs = ahora;
  statUltimoReporteMs       = ahora;
}

// Imprime el bloque de estadisticas por Serial.
static void imprimirBloqueEstadisticas() {
  float riskPromedio = (statTotalLecturas > 0)
                          ? (float)(statSumaRiskScore / statTotalLecturas)
                          : 0.0f;

  Serial.println(F("--- ESTADISTICAS DEL SISTEMA ---"));
  Serial.print(F("Uptime                   : "));
  Serial.println(statFormatearTimestamp(millis()));
  Serial.print(F("Lecturas totales         : "));
  Serial.println(statTotalLecturas);
  Serial.print(F("Alertas generadas        : "));
  Serial.println(statTotalAlertas);
  Serial.print(F("Tiempo en NORMAL         : "));
  Serial.print(statTiempoNormalMs / 1000);
  Serial.println(F("s"));
  Serial.print(F("Tiempo en PREVENTIVO     : "));
  Serial.print(statTiempoPreventivoMs / 1000);
  Serial.println(F("s"));
  Serial.print(F("Tiempo en FALLA PROBABLE : "));
  Serial.print(statTiempoFallaMs / 1000);
  Serial.println(F("s"));
  Serial.print(F("Risk score promedio      : "));
  Serial.println(riskPromedio, 3);
  Serial.println(F("---------------------------------"));
}

// hay que llmamarlo en cada loop(), justo despues de calcular estadoIA y riskScore.
void actualizarEstadisticas(int estado, float riskScore) {
  unsigned long ahora = millis();

  // acumular tiempo transcurrido en el estado actual 
  unsigned long deltaMs = ahora - statUltimaActualizacionMs;
  statUltimaActualizacionMs = ahora;

  if (estado == ESTADO_NORMAL) {
    statTiempoNormalMs += deltaMs;
  } else if (estado == ESTADO_PREVENTIVO) {
    statTiempoPreventivoMs += deltaMs;
  } else {
    statTiempoFallaMs += deltaMs;
  }

  // contadores generales 
  statTotalLecturas++;
  statSumaRiskScore += riskScore;

  // deteccion de alertas: transicion hacia PREVENTIVO o FALLA 
  if (statEstadoAnterior == -1) {
    statEstadoAnterior = estado; // primera lectura, no cuenta como alerta
  } else if (estado != statEstadoAnterior) {
    if (estado == ESTADO_PREVENTIVO || estado == ESTADO_FALLA) {
      statTotalAlertas++;
    }
    statEstadoAnterior = estado;
  }

  // reporte periodico 
  if (ahora - statUltimoReporteMs >= INTERVALO_ESTADISTICAS_MS) {
    statUltimoReporteMs = ahora;
    imprimirBloqueEstadisticas();
  }
}

#endif
