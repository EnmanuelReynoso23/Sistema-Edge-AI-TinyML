"""
ML.py - Clasificador de estado para mantenimiento predictivo
Genera un dataset sintetico, entrena un DecisionTree y evalua su precision.
"""

import random
import csv
from sklearn.tree import DecisionTreeClassifier
from sklearn.model_selection import train_test_split
from sklearn.metrics import classification_report, accuracy_score

# umbrales basados en normas industriales (IEC 60085 para temp, ISO 10816 para vibracion)
TEMP_NORMAL_MAX     = 50.0   # por debajo de 50 C es normal
TEMP_PREVENTIVO_MAX = 75.0   # entre 50 y 75 es preventivo, arriba de 75 es falla
VIB_NORMAL_MAX      = 2.0    # por debajo de 2g es normal
VIB_PREVENTIVO_MAX  = 5.0    # entre 2 y 5g es preventivo, arriba de 5 es falla
USO_NORMAL_MAX      = 60.0   # por debajo de 60% es normal
USO_PREVENTIVO_MAX  = 85.0   # entre 60 y 85 es preventivo, arriba de 85 es falla

# pesos del risk score (que tanto importa cada variable)
PESO_TEMP = 0.40
PESO_VIB  = 0.35
PESO_USO  = 0.25

ESTADOS = {0: "NORMAL", 1: "PREVENTIVO", 2: "FALLA_PROBABLE"}


def normalizar(valor, minimo, maximo):
    """pone un valor en el rango de 0 a 1"""
    if maximo == minimo:
        return 0.0
    n = (valor - minimo) / (maximo - minimo)
    return max(0.0, min(1.0, n))


def calcular_risk_score(temperatura, vibracion, uso):
    """calcula el puntaje de riesgo combinando los 3 sensores con pesos"""
    t = normalizar(temperatura, 20.0, 100.0)
    v = normalizar(vibracion,    0.0,  10.0)
    u = normalizar(uso,          0.0, 100.0)
    score = (t * PESO_TEMP) + (v * PESO_VIB) + (u * PESO_USO)
    return max(0.0, min(1.0, score))


def clasificar_estado(temperatura, vibracion, uso):
    """
    decide el estado del equipo segun los sensores y el risk score.
    si algun sensor pasa del limite critico -> falla directo
    si pasa del limite preventivo -> preventivo
    si no, se decide por el risk score combinado
    """
    score = calcular_risk_score(temperatura, vibracion, uso)

    # si cualquier sensor esta en zona de falla
    if temperatura > TEMP_PREVENTIVO_MAX or vibracion > VIB_PREVENTIVO_MAX or uso > USO_PREVENTIVO_MAX:
        return 2  # falla probable

    if score >= 0.75:
        return 2

    # si cualquier sensor esta en zona preventiva
    if temperatura > TEMP_NORMAL_MAX or vibracion > VIB_NORMAL_MAX or uso > USO_NORMAL_MAX:
        return 1  # preventivo

    if score >= 0.45:
        return 1

    return 0  # normal


def generar_dataset(n=15000):
    """genera ejemplos aleatorios con distribucion realista"""
    X = []
    y = []

    n_normal = int(n * 0.40)
    n_prev   = int(n * 0.35)
    n_falla  = n - n_normal - n_prev

    random.seed(42)

    # ejemplos normales
    for _ in range(n_normal):
        t = random.uniform(20.0, 52.0)
        v = random.uniform(0.0, 2.5)
        u = random.uniform(10.0, 65.0)
        X.append([t, v, u])
        y.append(clasificar_estado(t, v, u))

    # ejemplos preventivos
    for _ in range(n_prev):
        t = random.uniform(40.0, 78.0)
        v = random.uniform(1.0, 6.0)
        u = random.uniform(45.0, 88.0)
        X.append([t, v, u])
        y.append(clasificar_estado(t, v, u))

    # ejemplos de falla
    for _ in range(n_falla):
        t = random.uniform(55.0, 100.0)
        v = random.uniform(3.0, 10.0)
        u = random.uniform(60.0, 100.0)
        X.append([t, v, u])
        y.append(clasificar_estado(t, v, u))

    return X, y


# --- programa principal ---
if __name__ == "__main__":
    print("Generando 15,000 ejemplos de sensores...")
    X, y = generar_dataset(15000)

    conteo = {0: 0, 1: 0, 2: 0}
    for e in y:
        conteo[e] += 1
    print(f"  NORMAL: {conteo[0]} | PREVENTIVO: {conteo[1]} | FALLA: {conteo[2]}")

    # entrenar el modelo con 80% train y 20% test
    print("\nEntrenando DecisionTree...")
    X_train, X_test, y_train, y_test = train_test_split(
        X, y, test_size=0.20, random_state=42, stratify=y
    )

    modelo = DecisionTreeClassifier(max_depth=6, min_samples_leaf=20, random_state=42)
    modelo.fit(X_train, y_train)

    y_pred = modelo.predict(X_test)
    acc = accuracy_score(y_test, y_pred)
    print(f"  Accuracy: {acc * 100:.2f}%")

    nombres = [ESTADOS[i] for i in sorted(ESTADOS.keys())]
    print("\n" + classification_report(y_test, y_pred, target_names=nombres))

    # probar con algunos valores
    print("Pruebas rapidas:")
    pruebas = [
        (30.0, 0.5, 30.0, "equipo normal"),
        (55.0, 3.0, 70.0, "temp y uso elevado"),
        (80.0, 6.0, 90.0, "todo critico"),
        (45.0, 1.0, 50.0, "en el limite"),
        (60.0, 4.0, 75.0, "zona preventiva alta"),
    ]

    for temp, vib, uso, desc in pruebas:
        score = calcular_risk_score(temp, vib, uso)
        estado = clasificar_estado(temp, vib, uso)
        pred = modelo.predict([[temp, vib, uso]])[0]
        print(f"  T={temp:.0f} V={vib:.1f} U={uso:.0f} -> score={score:.3f} estado={ESTADOS[estado]}  ({desc})")

    # guardar el dataset en csv
    print("\nGuardando dataset en CSV...")
    with open("datos_sensores.csv", "w", newline="", encoding="utf-8") as f:
        w = csv.writer(f)
        w.writerow(["Temperatura", "Vibracion", "Uso", "RiskScore", "Estado_Codigo", "Estado_Nombre"])
        for medicion, cod in zip(X, y):
            rs = calcular_risk_score(medicion[0], medicion[1], medicion[2])
            w.writerow([
                round(medicion[0], 2), round(medicion[1], 2), round(medicion[2], 2),
                round(rs, 4), cod, ESTADOS[cod]
            ])
    print("Listo, archivo datos_sensores.csv creado")
