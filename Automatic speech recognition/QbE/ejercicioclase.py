# -*- coding: utf-8 -*-

import numpy as np
from sys import *
from distances import get_distance_matrix
from matplotlib.pyplot import matshow, show, cm, plot

def lee_fichero(fichero):
	matriz=[]
	fichero=open(fichero,"r")
	lineas=fichero.readlines()
	matriz=[linea.split() for linea in lineas] 
	fichero.close()
	return np.array(matriz).astype(np.float)

# Implementar a partir de aqui
# sintaxis get_distance_matrix:
# distancias=get_distance_matrix(npmatriz1,npmatriz2,'cos')
# donde npmatriz1 y npmatriz2 son los vectores de características de los dos audios

LONGITUD = 0
COSTE = 1
ORIGEN = 2


npmatriz1 = lee_fichero("SEG-0062.mfc.raw") 
npmatriz2 = lee_fichero("largo250000.mfc.raw")

distancias = get_distance_matrix(npmatriz1, npmatriz2,'cos')

T = np.zeros((distancias.shape[0], distancias.shape[1], 3))

I = T.shape[0]
J = T.shape[1]

for j in range(0, J):
	T[0][j][ORIGEN] = j
	T[0][j][LONGITUD] = 1
	T[0][j][COSTE] = distancias[0, j]

for i in range(1, I):
	T[i][0][COSTE] = T[i-1][0][COSTE]  + distancias[i][0] 
	T[i][0][ORIGEN] = 0
	T[0][j][LONGITUD] = i

for i in range(1, I):
	for j in range(1, J):
		
		checks = [
			((T[i-1, j][COSTE] + distancias[i][j]) / (1+(T[i-1, j][LONGITUD]))),
			((T[i, j-1][COSTE] + distancias[i][j]) / (1+(T[i, j-1][LONGITUD]))),
			((T[i-1, j-1][COSTE] + distancias[i][j]) / (1+(T[i-1, j-1][LONGITUD]))),
		]
		
		minimo = min(checks)
		pos_minim = checks.index(minimo)

		if pos_minim == 0:
			imax, jmax = (i-1, j)
		elif pos_minim == 1:
			imax, jmax = (i, j-1)
		elif pos_minim == 2:
			imax, jmax = (i-1, j-1)
                
		T[i][j][COSTE] = minimo + distancias[i][j]  
		T[i][j][LONGITUD] = i
		T[i][j][ORIGEN] = T[imax][jmax][ORIGEN]


distancias_finales = [T[I-1][j][COSTE]  for j in range(0, J)]

minimo = min(distancias_finales)
pos_fin_minimo = distancias_finales.index(minimo)
origen = T[I-1][pos_fin_minimo][ORIGEN]

print(f"origen: {origen}, fin {pos_fin_minimo}, valor minimo {minimo}")

limite_minimos = 10
distancia_entre_pronunciaciones = 5	
lista_minimos = []

for i in range(len(distancias_finales)):
    if abs(pos_fin_minimo - i) > distancia_entre_pronunciaciones:
        lista_minimos.append( (T[I-1][i][ORIGEN], i, distancias_finales[i]))
        

lista_minimos.sort(key= lambda e : e[2])
lista_minimos = lista_minimos[0:limite_minimos]

for minimo in lista_minimos:
    origen, fin, distancia = minimo
    print(f"origen: {origen}, fin {fin}, valor minimo {distancia}")
    

