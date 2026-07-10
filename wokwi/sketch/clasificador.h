/*
 * clasificador.h
 * Logica del clasificador para mantenimiento predictivo
 * Decide si el equipo esta NORMAL, PREVENTIVO o en FALLA PROBABLE
 * usando temperatura, vibracion e intensidad de uso
 */

#ifndef CLASIFICADOR_H
#define CLASIFICADOR_H

#include <Arduino.h>

// estados del equipo
enum EstadoEquipo {
    ESTADO_NORMAL      = 0,
    ESTADO_PREVENTIVO  = 1,
    ESTADO_FALLA       = 2
};

// umbrales de cada sensor (IEC 60085 para temp, ISO 10816 para vibracion)
const float TEMP_UMBRAL_NORMAL     = 50.0f;
const float TEMP_UMBRAL_PREVENTIVO = 75.0f;
const float VIB_UMBRAL_NORMAL      = 2.0f;
const float VIB_UMBRAL_PREVENTIVO  = 5.0f;
const float USO_UMBRAL_NORMAL      = 60.0f;
const float USO_UMBRAL_PREVENTIVO  = 85.0f;

// pesos para el risk score
const float PESO_TEMPERATURA = 0.4f;
const float PESO_VIBRACION   = 0.35f;
const float PESO_USO_MOTOR   = 0.25f;

// umbrales del score final
const float SCORE_UMBRAL_NORMAL     = 0.45f;
const float SCORE_UMBRAL_PREVENTIVO = 0.75f;


// pone un valor en el rango de 0 a 1
float normalizarValor(float valor, float minimo, float maximo) {
    if (maximo <= minimo) return 0.0f;
    float norm = (valor - minimo) / (maximo - minimo);
    if (norm < 0.0f) norm = 0.0f;
    if (norm > 1.0f) norm = 1.0f;
    return norm;
}

// calcula el risk score ponderado, retorna de 0.0 a 1.0
float calcularRiskScore(float temperatura, float vibracion, float uso) {
    float tNorm = normalizarValor(temperatura, 20.0f, 100.0f);
    float vNorm = normalizarValor(vibracion,    0.0f,  10.0f);
    float uNorm = normalizarValor(uso,          0.0f, 100.0f);

    float score = (tNorm * PESO_TEMPERATURA)
                + (vNorm * PESO_VIBRACION)
                + (uNorm * PESO_USO_MOTOR);

    if (score < 0.0f) score = 0.0f;
    if (score > 1.0f) score = 1.0f;
    return score;
}

// arbol de decision entrenado con sklearn y pasado a if/else
// basicamente replica lo que aprendio el DecisionTree con 15000 ejemplos
int clasificarEstadoArbol(float temperatura, float vibracion, float uso) {
    if (vibracion <= 4.9985f) {
        if (vibracion <= 2.0000f) {
            if (uso <= 59.9981f) {
                if (temperatura <= 50.0001f) {
                    return 0;  // NORMAL
                } else {
                    if (temperatura <= 75.0012f) {
                        return 1;  // PREVENTIVO
                    } else {
                        return 2;  // FALLA
                    }
                }
            } else {
                if (uso <= 85.0315f) {
                    if (temperatura <= 74.9564f) {
                        return 1;  // PREVENTIVO
                    } else {
                        return 2;  // FALLA
                    }
                } else {
                    return 2;  // FALLA
                }
            }
        } else {
            if (temperatura <= 74.9766f) {
                if (uso <= 85.0065f) {
                    return 1;  // PREVENTIVO
                } else {
                    return 2;  // FALLA
                }
            } else {
                return 2;  // FALLA
            }
        }
    } else {
        // vibracion mayor a 5g, siempre es falla
        return 2;
    }
}

// funcion principal que combina el arbol con el risk score
// siempre elige el estado mas peligroso entre los dos para no dejar pasar nada
int clasificarEstado(float temperatura, float vibracion, float uso) {
    float riskScore = calcularRiskScore(temperatura, vibracion, uso);
    int estadoArbol = clasificarEstadoArbol(temperatura, vibracion, uso);

    // clasificar tambien por el score
    int estadoScore;
    if (riskScore >= SCORE_UMBRAL_PREVENTIVO) {
        estadoScore = ESTADO_FALLA;
    } else if (riskScore >= SCORE_UMBRAL_NORMAL) {
        estadoScore = ESTADO_PREVENTIVO;
    } else {
        estadoScore = ESTADO_NORMAL;
    }

    // el que sea mas alto (mas peligroso) gana
    return (estadoArbol > estadoScore) ? estadoArbol : estadoScore;
}

#endif
