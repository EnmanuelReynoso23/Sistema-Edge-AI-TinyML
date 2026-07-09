import random 
from sklearn.tree import DecisionTreeClassifier
import csv 

# funcion de clasificar
def clasificador_estado(temperatura, vibracion, uso):
    if temperatura > 75 or vibracion > 5 or uso > 85:
        return "Posibilidad de falla en el equipo"
    elif (50 < temperatura <= 75) or (2 <= vibracion <= 5) or (60 <= uso <= 85):
        return "Posibilidad de sobre calentamiento en el equipo"
    else: 
        return "Todo está en orden"

#variable
modelo_ia = DecisionTreeClassifier(max_depth=4)
x_datos = []
y_etiquetas = []
sensore_nuevos = [[40.0, 1.5, 50.0]]

print ("Generar 10000 ejemplos de sensores")

for i in range(10000):
    t = random.uniform (20.0, 100.0)   ## Temperatura 
    v = random.uniform (0.0, 10.0)     ## Vibracion
    u = random.uniform (10.0, 100.0)   ## Uso

    # preguntamo el estado
    estado = clasificador_estado(t, v, u)

    # guardamo eso dato       
    x_datos.append([t, v, u])
    y_etiquetas.append(estado)

print ("Entrenando el modelo de IA")
modelo_ia.fit(x_datos, y_etiquetas)

prediccion_ia = modelo_ia.predict(sensore_nuevos)
print(f"para los sensores {sensore_nuevos} el estado es: {prediccion_ia[0]}")
                   
print ("guardando los 10,000 ejemplos en un archivo csv")

with open("datos_sensores.csv","w", newline="", encoding="utf-8") as archivo_csv:
     escritor = csv.writer(archivo_csv)
     escritor.writerow(["Temperatura","Vibracion","Uso","Etiqueta_IA"])
     for medicion, resultado in zip(x_datos, y_etiquetas):
         escritor.writerow([medicion[0], medicion[1], medicion[2], resultado])

print("Archivo creado y guardado con exito" )
