#ifndef CCONTROL_H
#define CCONTROL_H

#include <string>

// Constantes de la simulación
#define N_LIBRERIAS 10
#define N_PEDIDOS   30

// Declaración de estructuras
struct Pedido {
    int idLibreria;
    std::string idPedido;
    std::string codLibro;
    std::string materia;
    int unidades;
    std::string fechaEnvio;
};

struct NodoPedido {
    Pedido info;
    NodoPedido* sig;
};

struct Libreria {
    int idLibreria;
    std::string localidad;
    NodoPedido* pedidos;
};

struct NodoABB {
    Libreria info;
    NodoABB* izq;
    NodoABB* der;
};

extern const char* LOCALIDADES[];
extern const int NUM_LOCALIDADES;
extern const char* MATERIAS[];
extern const int NUM_MATERIAS;

void inicializarAleatorio();
void generarLibreriasIniciales(NodoABB*& raiz, int n);
void generarYRepartirPedidos(NodoABB* raiz);
void mostrarResumenLibrerias(NodoABB* raiz);
void mostrarMenu();
void insertarLibreriaManual(NodoABB*& raiz);
void borrarLibreriaPorId(NodoABB*& raiz);
void mostrarPedidosDeLibreria(NodoABB* raiz);
void buscarPedidoPorId(NodoABB* raiz);
void extraerPedidoPorId(NodoABB* raiz);
void moverPedidoEntreLibrerias(NodoABB* raiz);
void mostrarEstadisticas(NodoABB* raiz);
void liberarMemoria(NodoABB*& raiz);

#endif // CCONTROL_H
