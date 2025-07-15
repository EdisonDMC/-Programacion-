def convertir_a_robot(frase):
    traduccion = frase.lower()
    traduccion = traduccion.replace('a', '4')
    traduccion = traduccion.replace('e', '3')
    traduccion = traduccion.replace('i', '1')
    traduccion = traduccion.replace('o', '0')
    traduccion = traduccion.replace('u', 'µ')
    return traduccion

# Prueba del programa
frase_original = input("Escribe una frase para convertirla en idioma robot: ")
frase_robot = convertir_a_robot(frase_original)
print("Tu frase en idioma robot es:", frase_robot)