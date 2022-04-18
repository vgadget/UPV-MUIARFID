
"""
Dada la cadena:
"El/DT perro/N come/V carne/N de/P la/DT carnicería/N y/C de/P la/DT nevera/N y/C canta/V el/DT la/N la/N la/N ./Fp"
"""

cadena = "El/DT perro/N come/V carne/N de/P la/DT carnicería/N y/C de/P la/DT nevera/N y/C canta/V el/DT la/N la/N la/N ./Fp"

elementos = cadena.split()
palabras = [ x.split("/")[0].lower() for x in elementos]
categorias = [ x.split("/")[1] for x in elementos]

"""
1) Obtener un diccionario, que para cada categoría, muestre su frecuencia. Ordenar el resultado 
alfabéticamente por categoría.
"""
dic_categorias = {}

for c in categorias:
    if (c in dic_categorias.keys()):
        dic_categorias[c]+=1
    else:
        dic_categorias[c] = 1

print(f"\n-------- EJERCICIO 1 --------\n\n {dic_categorias}\n")
        
"""
2) Obtener un diccionario, que para cada palabra, muestre su frecuencia, y una lista de sus categorías morfosintácticas 
con su respectiva frecuencia. Mostrar el resultado ordenado alfabéticamente por palabra.
"""

dic_palabras_cat = {}

for e in elementos:
    palabra = e.split("/")[0].lower()
    categoria = e.split("/")[1]
    
    if (palabra not in dic_palabras_cat.keys()):
        dic_palabras_cat[palabra] = [0, {}]
    
    dic_palabras_cat[palabra][0]+=1
    
    if (categoria not in dic_palabras_cat[palabra][1].keys()):
        dic_palabras_cat[palabra][1][categoria] = 0
    
    dic_palabras_cat[palabra][1][categoria]+=1
 
print(f"\n-------- EJERCICIO 2 --------\n\n {dic_palabras_cat}\n")
 
"""
 3) Calcular la frecuencia de los todos los bigramas de la cadena, considerar un símbolo inicial <S> y un símbolo 
 final </S> para la cadena.
"""
    
categoriasEj3 = ['<S>']
categoriasEj3.extend(categorias)
categoriasEj3.append('</S>')

frecuenciaBigramas = {}

for i in range(len(categoriasEj3)-1):
    
    element = (categoriasEj3[i], categoriasEj3[i+1])
    
    if (element not in frecuenciaBigramas.keys()):
        frecuenciaBigramas[element] = 0
    
    frecuenciaBigramas[element] += 1

print(f"\n-------- EJERCICIO 3 --------\n\n {frecuenciaBigramas}\n")
    
"""
4) Construir una función que devuelva las probabilidades léxicas P(C|w) y de emisión P(w|C) para una palabra dada (w) 
para todas sus categorías (C) que aparecen en el diccionario construido anteriormente. Si la palabra no existe en el 
diccionario debe decir que la palabra es desconocida.

P(A|B) = P(B|A)*P(A) / P(B)

"""

def ejercicio4(palabra, dicc_palabras, dicc_categorias):
    
    total_palabras = sum([p[0] for p in dicc_palabras.values()])
    
    probabilidad_palabra = dicc_palabras[palabra][0]/total_palabras
    
    res = {}
    
    for categorias in dicc_palabras[palabra][1:]:
        for c in categorias.keys():
            res[f"P({c}|{palabra})"] = categorias[c] / sum(categorias.values())
            
            res[f"P({palabra}|{c})"] = ( res[f"P({c}|{palabra})"] * probabilidad_palabra ) / ( dic_categorias[c] / total_palabras )
        
        
    return res


palabra = "la"
print(f"\n-------- EJERCICIO 4 -------- Ejemplo con la palabra '{palabra}': \n\n {ejercicio4(palabra, dic_palabras_cat, dic_categorias)}\n")
