#include <iostream>
#include "control.h"

using namespace std;

int main() {
    NodoABB* raiz = NULL;
    inicializarAleatorio();
    // 1) Generar librerias iniciales
    generarLibreriasIniciales(raiz, N_LIBRERIAS);
    // 2) Generar primer lote de pedidos, mostrarlos, repartirlos y mostrar resumen
    generarYRepartirPedidos(raiz);
    int opcion;
    do {
        mostrarMenu();
        cin >> opcion;
        cin.ignore(1000, '\n'); // limpiar resto de la linea
        switch (opcion) {
            case 1:
                insertarLibreriaManual(raiz);
                break;
            case 2:
                borrarLibreriaPorId(raiz);
                break;
            case 3:
                mostrarPedidosDeLibreria(raiz);
                break;
            case 4:
                buscarPedidoPorId(raiz);
                break;
            case 5:
                extraerPedidoPorId(raiz);
                break;
            case 6:
                moverPedidoEntreLibrerias(raiz);
                break;
            case 7:
                mostrarEstadisticas(raiz);
                break;
            case 8:
                generarYRepartirPedidos(raiz);
                break;
            case 0:
                cout << "Saliendo del programa..." << endl;
                break;
            default:
                cout << "Opcion no valida. Intente de nuevo." << endl;
                break;
        }
    } while (opcion != 0);
    liberarMemoria(raiz);
    return 0;
}
