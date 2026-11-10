# Gestor de pedidos de una editorial

Proyecto académico de Rodrigo Blaudy Mayoral para Estructuras de Datos, curso 2025-2026.
Aplicación de consola en C++ que simula la distribución de pedidos entre librerías.

## Funciones
- Altas y bajas de librerías mediante un árbol binario de búsqueda.
- Pedidos asociados a cada librería mediante listas enlazadas.
- Búsqueda, extracción y traslado de pedidos.
- Estadísticas y liberación de memoria dinámica.

## Compilar y ejecutar
Requiere un compilador compatible con C++11, por ejemplo GCC.

```sh
g++ -std=c++11 -Wall -Wextra main.cpp control.cpp -o gestor
./gestor
```

En Windows, el ejecutable es `gestor.exe`. Al comenzar se generan 10 librerías y
30 pedidos. Pulsa Intro cuando lo solicite y elige las opciones del menú; 0 termina.

## Estructura
- `main.cpp`: menú y flujo principal.
- `control.h`: estructuras y operaciones públicas.
- `control.cpp`: árboles, listas y operaciones de gestión.
- `test_borrado.cpp`: regresión para el borrado de nodos con dos hijos.

```sh
g++ -std=c++11 -Wall -Wextra test_borrado.cpp -o test_borrado
./test_borrado
```

## Alcance y preparación del portfolio
Es un simulador académico, sin base de datos ni persistencia entre ejecuciones.
El árbol no se autoequilibra, los identificadores aleatorios de pedidos pueden
repetirse y la interfaz espera entradas numéricas válidas.

Para preparar esta versión de portfolio se unificaron los nombres `control.h`
y `control.cpp`, se normalizó la codificación a UTF-8 y se corrigió la propiedad
de la lista de pedidos al borrar un nodo con dos hijos. Esa corrección y la prueba
de regresión se realizaron con asistencia de IA; no forman parte de la entrega original.
