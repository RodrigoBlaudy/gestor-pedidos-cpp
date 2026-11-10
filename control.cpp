#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include "control.h"

using namespace std;

// Definición de arrays de datos fijos
const char* LOCALIDADES[] = {
    "Mostoles", "Alcala", "Leganes", "Fuenlabrada", "Getafe",
    "Alcorcon", "Torrejon", "Parla", "Alcobendas", "Coslada",
    "Pozuelo", "Rivas", "Valdemoro", "Majadahonda", "Aranjuez",
    "Arganda", "Boadilla", "Pinto", "Colmenar", "Tres Cantos"
};
const int NUM_LOCALIDADES = 20;

const char* MATERIAS[] = {
    "Matematicas", "Fisica", "Tecnologia", "Musica", "Historia", "Lengua"
};
const int NUM_MATERIAS = 6;

// ---------- Funciones auxiliares internas (solo en este fichero) ----------

static int numeroAleatorio(int min, int max) {
    return min + rand() % (max - min + 1);
}

static string generarIdPedidoAleatorio() {
    char buffer[16];
    int num = numeroAleatorio(0, 99999);
    sprintf(buffer, "P%05d", num);
    return string(buffer);
}

static string generarCodLibroAleatorio() {
    char buffer[16];
    int d1 = numeroAleatorio(0, 9);
    int d2 = numeroAleatorio(0, 9);
    int d3 = numeroAleatorio(0, 9);
    char letra = 'A' + numeroAleatorio(0, 25);
    int d4 = numeroAleatorio(0, 9);
    int d5 = numeroAleatorio(0, 9);
    sprintf(buffer, "%d%d%d%c%d%d", d1, d2, d3, letra, d4, d5);
    return string(buffer);
}

static string generarMateriaAleatoria() {
    int idx = numeroAleatorio(0, NUM_MATERIAS - 1);
    return string(MATERIAS[idx]);
}

static string generarFechaAleatoria() {
    char buffer[16];
    int dia = numeroAleatorio(1, 30);
    int mes = numeroAleatorio(1, 9);  // por ejemplo, meses 1-9
    sprintf(buffer, "%02d-%02d-2025", dia, mes);
    return string(buffer);
}

// Lista enlazada de pedidos: operaciones básicas
static int contarPedidosLista(NodoPedido* lista) {
    int cont = 0;
    NodoPedido* aux = lista;
    while (aux != NULL) {
        cont++;
        aux = aux->sig;
    }
    return cont;
}

static void insertarPedidoEnLista(NodoPedido*& lista, const Pedido& p) {
    NodoPedido* nuevo = new NodoPedido;
    nuevo->info = p;
    nuevo->sig = NULL;

    if (lista == NULL) {
        lista = nuevo;
    } else {
        NodoPedido* aux = lista;
        while (aux->sig != NULL) {
            aux = aux->sig;
        }
        aux->sig = nuevo;
    }
}

static bool eliminarPedidoDeLista(NodoPedido*& lista, const string& idPedido, Pedido& eliminado) {
    NodoPedido* actual = lista;
    NodoPedido* anterior = NULL;

    while (actual != NULL && actual->info.idPedido != idPedido) {
        anterior = actual;
        actual = actual->sig;
    }

    if (actual == NULL) {
        return false;
    }

    eliminado = actual->info;

    if (anterior == NULL) {
        // eliminar cabeza
        lista = actual->sig;
    } else {
        anterior->sig = actual->sig;
    }

    delete actual;
    return true;
}

static void liberarListaPedidos(NodoPedido*& lista) {
    NodoPedido* aux;
    while (lista != NULL) {
        aux = lista;
        lista = lista->sig;
        delete aux;
    }
}

// ABB de librerías: operaciones básicas
static NodoABB* crearNodoLibreria(const Libreria& lib) {
    NodoABB* nodo = new NodoABB;
    nodo->info = lib;
    nodo->izq = nodo->der = NULL;
    return nodo;
}

static NodoABB* buscarNodoLibreria(NodoABB* raiz, int id) {
    if (raiz == NULL) return NULL;
    if (id == raiz->info.idLibreria) return raiz;
    if (id < raiz->info.idLibreria)
        return buscarNodoLibreria(raiz->izq, id);
    else
        return buscarNodoLibreria(raiz->der, id);
}

static NodoABB* insertarNodoLibreria(NodoABB* raiz, const Libreria& lib, bool& insertado) {
    if (raiz == NULL) {
        insertado = true;
        return crearNodoLibreria(lib);
    }

    if (lib.idLibreria < raiz->info.idLibreria) {
        raiz->izq = insertarNodoLibreria(raiz->izq, lib, insertado);
    } else if (lib.idLibreria > raiz->info.idLibreria) {
        raiz->der = insertarNodoLibreria(raiz->der, lib, insertado);
    } else {
        // id repetido
        insertado = false;
    }
    return raiz;
}

static NodoABB* buscarMin(NodoABB* raiz) {
    NodoABB* actual = raiz;
    while (actual != NULL && actual->izq != NULL) {
        actual = actual->izq;
    }
    return actual;
}

static NodoABB* borrarNodoLibreria(NodoABB* raiz, int id, bool& borrado) {
    if (raiz == NULL) {
        borrado = false;
        return NULL;
    }

    if (id < raiz->info.idLibreria) {
        raiz->izq = borrarNodoLibreria(raiz->izq, id, borrado);
    } else if (id > raiz->info.idLibreria) {
        raiz->der = borrarNodoLibreria(raiz->der, id, borrado);
    } else {
        // Encontrado
        borrado = true;
        // liberar lista de pedidos de esta librería
        liberarListaPedidos(raiz->info.pedidos);

        if (raiz->izq == NULL && raiz->der == NULL) {
            delete raiz;
            return NULL;
        } else if (raiz->izq == NULL) {
            NodoABB* temp = raiz->der;
            delete raiz;
            return temp;
        } else if (raiz->der == NULL) {
            NodoABB* temp = raiz->izq;
            delete raiz;
            return temp;
        } else {
            // dos hijos: sustituir por sucesor in-order
            NodoABB* sucesor = buscarMin(raiz->der);
            raiz->info = sucesor->info;
            sucesor->info.pedidos = NULL; // La lista pertenece ahora al nodo destino.
            // Ojo: al copiar info también copiamos el puntero a la lista de pedidos
            // Borramos el sucesor sin liberar su lista (ya la "hemos movido")
            raiz->der = borrarNodoLibreria(raiz->der, sucesor->info.idLibreria, borrado);
        }
    }
    return raiz;
}

// Rellenar array con IDs de las librerías (recorrido en-orden)
static void rellenarIds(NodoABB* raiz, int ids[], int maxIds, int& contador) {
    if (raiz == NULL || contador >= maxIds) return;
    rellenarIds(raiz->izq, ids, maxIds, contador);
    if (contador < maxIds) {
        ids[contador] = raiz->info.idLibreria;
        contador++;
    }
    rellenarIds(raiz->der, ids, maxIds, contador);
}

int obtenerIdsLibrerias(NodoABB* raiz, int ids[], int maxIds) {
    int contador = 0;
    rellenarIds(raiz, ids, maxIds, contador);
    return contador;
}

// Recorridos para mostrar información
static void mostrarResumenInOrden(NodoABB* raiz) {
    if (raiz == NULL) return;
    mostrarResumenInOrden(raiz->izq);
    cout << "Libreria " << raiz->info.idLibreria
         << " (" << raiz->info.localidad << ") -> "
         << "Pedidos: " << contarPedidosLista(raiz->info.pedidos) << endl;
    mostrarResumenInOrden(raiz->der);
}

static void mostrarPedidosLista(NodoPedido* lista) {
    NodoPedido* aux = lista;
    while (aux != NULL) {
        const Pedido& p = aux->info;
        cout << "  ID Pedido: " << p.idPedido
             << " | Cod libro: " << p.codLibro
             << " | Materia: " << p.materia
             << " | Unidades: " << p.unidades
             << " | Fecha envio: " << p.fechaEnvio
             << " | Id libreria: " << p.idLibreria
             << endl;
        aux = aux->sig;
    }
}

static bool buscarPedidoEnNodoLista(NodoPedido* lista, const string& idPedido, Pedido& encontrado) {
    NodoPedido* aux = lista;
    while (aux != NULL) {
        if (aux->info.idPedido == idPedido) {
            encontrado = aux->info;
            return true;
        }
        aux = aux->sig;
    }
    return false;
}

static bool buscarPedidoEnArbolRec(NodoABB* raiz, const string& idPedido, Pedido& encontrado, int& idLibreria) {
    if (raiz == NULL) return false;

    if (buscarPedidoEnNodoLista(raiz->info.pedidos, idPedido, encontrado)) {
        idLibreria = raiz->info.idLibreria;
        return true;
    }

    if (buscarPedidoEnArbolRec(raiz->izq, idPedido, encontrado, idLibreria)) return true;
    if (buscarPedidoEnArbolRec(raiz->der, idPedido, encontrado, idLibreria)) return true;

    return false;
}

static bool extraerPedidoEnArbolRec(NodoABB* raiz, const string& idPedido, Pedido& extraido, int& idLibreria) {
    if (raiz == NULL) return false;

    if (eliminarPedidoDeLista(raiz->info.pedidos, idPedido, extraido)) {
        idLibreria = raiz->info.idLibreria;
        return true;
    }

    if (extraerPedidoEnArbolRec(raiz->izq, idPedido, extraido, idLibreria)) return true;
    if (extraerPedidoEnArbolRec(raiz->der, idPedido, extraido, idLibreria)) return true;

    return false;
}

// Estadísticas: total librerías, total pedidos, librería con más pedidos
static void acumularEstadisticas(NodoABB* raiz, int& totalLibrerias, int& totalPedidos,
                                 int& idMaxPedidos, int& maxPedidos,
                                 int& totalUnidades) {
    if (raiz == NULL) return;

    acumularEstadisticas(raiz->izq, totalLibrerias, totalPedidos, idMaxPedidos, maxPedidos, totalUnidades);

    totalLibrerias++;
    int numPedidos = 0;
    int unidadesLib = 0;

    NodoPedido* aux = raiz->info.pedidos;
    while (aux != NULL) {
        numPedidos++;
        totalPedidos++;
        unidadesLib += aux->info.unidades;
        totalUnidades += aux->info.unidades;
        aux = aux->sig;
    }

    if (numPedidos > maxPedidos) {
        maxPedidos = numPedidos;
        idMaxPedidos = raiz->info.idLibreria;
    }

    acumularEstadisticas(raiz->der, totalLibrerias, totalPedidos, idMaxPedidos, maxPedidos, totalUnidades);
}

// Liberar árbol completo
void liberarMemoria(NodoABB*& raiz) {
    if (raiz == NULL) return;
    liberarMemoria(raiz->izq);
    liberarMemoria(raiz->der);
    liberarListaPedidos(raiz->info.pedidos);
    delete raiz;
    raiz = NULL;
}

// ---------- Funciones del "interface" declarado en el .h ----------

void inicializarAleatorio() {
    srand((unsigned int) time(NULL));
}

void generarLibreriasIniciales(NodoABB*& raiz, int n) {
    raiz = NULL;

    for (int i = 0; i < n; ++i) {
        Libreria lib;
        bool insertado = false;

        // Generar id de 3 cifras no repetido
        do {
            lib.idLibreria = numeroAleatorio(100, 999);
            lib.localidad = LOCALIDADES[numeroAleatorio(0, NUM_LOCALIDADES - 1)];
            lib.pedidos = NULL;
            raiz = insertarNodoLibreria(raiz, lib, insertado);
        } while (!insertado);
    }

    cout << "Se han generado " << n << " librerias iniciales." << endl;
}

void generarYRepartirPedidos(NodoABB* raiz) {
    if (raiz == NULL) {
        cout << "No hay librerias en el arbol. No se pueden generar pedidos." << endl;
        return;
    }

    int ids[200];
    int numIds = obtenerIdsLibrerias(raiz, ids, 200);

    if (numIds == 0) {
        cout << "No hay librerias en el arbol. No se pueden generar pedidos." << endl;
        return;
    }

    Pedido pedidos[N_PEDIDOS];

    cout << "\nGenerando " << N_PEDIDOS << " pedidos aleatorios..." << endl;
    for (int i = 0; i < N_PEDIDOS; ++i) {
        int indiceId = numeroAleatorio(0, numIds - 1);
        int idLib = ids[indiceId];

        pedidos[i].idLibreria = idLib;
        pedidos[i].idPedido = generarIdPedidoAleatorio();
        pedidos[i].codLibro = generarCodLibroAleatorio();
        pedidos[i].materia = generarMateriaAleatoria();
        pedidos[i].unidades = numeroAleatorio(1, 30);
        pedidos[i].fechaEnvio = generarFechaAleatoria();
    }

    cout << "\nPedidos generados (antes de repartir):" << endl;
    for (int i = 0; i < N_PEDIDOS; ++i) {
        const Pedido& p = pedidos[i];
        cout << i + 1 << ") "
             << "IdLibreria: " << p.idLibreria
             << " | IdPedido: " << p.idPedido
             << " | CodLibro: " << p.codLibro
             << " | Materia: " << p.materia
             << " | Unidades: " << p.unidades
             << " | Fecha: " << p.fechaEnvio
             << endl;
    }

    cout << "\nPulse Enter para repartir los pedidos entre las librerias...";
    cin.ignore(1000, '\n');

    // Repartir pedidos
    for (int i = 0; i < N_PEDIDOS; ++i) {
        NodoABB* nodoLib = buscarNodoLibreria(raiz, pedidos[i].idLibreria);
        if (nodoLib != NULL) {
            insertarPedidoEnLista(nodoLib->info.pedidos, pedidos[i]);
        }
    }

    cout << "\nPedidos repartidos. Resumen de librerias:" << endl;
    mostrarResumenLibrerias(raiz);
}

void mostrarResumenLibrerias(NodoABB* raiz) {
    if (raiz == NULL) {
        cout << "No hay librerias en el arbol." << endl;
        return;
    }
    mostrarResumenInOrden(raiz);
}

void mostrarMenu() {
    cout << "\n========== MENU PRINCIPAL ==========\n";
    cout << "1 - Insertar una libreria de forma manual\n";
    cout << "2 - Borrar una libreria del arbol\n";
    cout << "3 - Mostrar los datos de los pedidos de una libreria dada\n";
    cout << "4 - Buscar un pedido concreto por su ID\n";
    cout << "5 - Extraer un pedido concreto\n";
    cout << "6 - Llevar un pedido concreto de una libreria a otra\n";
    cout << "7 - Mostrar una estadistica de las librerias\n";
    cout << "8 - Continuar con la distribucion de pedidos\n";
    cout << "0 - Salir del programa\n";
    cout << "Seleccione una opcion: ";
}

// Opciones del menú

void insertarLibreriaManual(NodoABB*& raiz) {
    Libreria lib;
    cout << "Introduce el ID de la nueva libreria (entero de 3 cifras): ";
    cin >> lib.idLibreria;
    cin.ignore(1000, '\n');

    cout << "Introduce la localidad de la libreria: ";
    getline(cin, lib.localidad);

    lib.pedidos = NULL;
    bool insertado = false;
    raiz = insertarNodoLibreria(raiz, lib, insertado);

    if (insertado) {
        cout << "Libreria insertada correctamente." << endl;
    } else {
        cout << "No se ha podido insertar: ya existe una libreria con ese ID." << endl;
    }
}

void borrarLibreriaPorId(NodoABB*& raiz) {
    if (raiz == NULL) {
        cout << "No hay librerias en el arbol." << endl;
        return;
    }
    int id;
    cout << "Introduce el ID de la libreria a borrar: ";
    cin >> id;
    cin.ignore(1000, '\n');

    bool borrado = false;
    raiz = borrarNodoLibreria(raiz, id, borrado);

    if (borrado) {
        cout << "Libreria borrada correctamente." << endl;
    } else {
        cout << "No se ha encontrado ninguna libreria con ese ID." << endl;
    }
}

void mostrarPedidosDeLibreria(NodoABB* raiz) {
    if (raiz == NULL) {
        cout << "No hay librerias en el arbol." << endl;
        return;
    }

    int id;
    cout << "Introduce el ID de la libreria: ";
    cin >> id;
    cin.ignore(1000, '\n');

    NodoABB* nodo = buscarNodoLibreria(raiz, id);
    if (nodo == NULL) {
        cout << "No se ha encontrado ninguna libreria con ese ID." << endl;
        return;
    }

    cout << "Libreria " << nodo->info.idLibreria
         << " (" << nodo->info.localidad << ")\n";
    cout << "Pedidos:\n";
    if (nodo->info.pedidos == NULL) {
        cout << "  (sin pedidos)\n";
    } else {
        mostrarPedidosLista(nodo->info.pedidos);
    }
}

void buscarPedidoPorId(NodoABB* raiz) {
    if (raiz == NULL) {
        cout << "No hay librerias en el arbol." << endl;
        return;
    }

    string idPedido;
    cout << "Introduce el ID del pedido a buscar (ej. P12345): ";
    cin >> idPedido;
    cin.ignore(1000, '\n');

    Pedido encontrado;
    int idLibreria = -1;
    bool ok = buscarPedidoEnArbolRec(raiz, idPedido, encontrado, idLibreria);

    if (ok) {
        cout << "Pedido encontrado en la libreria " << idLibreria << ":\n";
        cout << "  IdPedido: " << encontrado.idPedido
             << " | CodLibro: " << encontrado.codLibro
             << " | Materia: " << encontrado.materia
             << " | Unidades: " << encontrado.unidades
             << " | Fecha: " << encontrado.fechaEnvio << endl;
    } else {
        cout << "No se ha encontrado ningun pedido con ese ID." << endl;
    }
}

void extraerPedidoPorId(NodoABB* raiz) {
    if (raiz == NULL) {
        cout << "No hay librerias en el arbol." << endl;
        return;
    }

    string idPedido;
    cout << "Introduce el ID del pedido a extraer (se borrara): ";
    cin >> idPedido;
    cin.ignore(1000, '\n');

    Pedido extraido;
    int idLibreria = -1;
    bool ok = extraerPedidoEnArbolRec(raiz, idPedido, extraido, idLibreria);

    if (ok) {
        cout << "Pedido extraido de la libreria " << idLibreria << ":\n";
        cout << "  IdPedido: " << extraido.idPedido
             << " | CodLibro: " << extraido.codLibro
             << " | Materia: " << extraido.materia
             << " | Unidades: " << extraido.unidades
             << " | Fecha: " << extraido.fechaEnvio << endl;
    } else {
        cout << "No se ha encontrado ningun pedido con ese ID." << endl;
    }
}

void moverPedidoEntreLibrerias(NodoABB* raiz) {
    if (raiz == NULL) {
        cout << "No hay librerias en el arbol." << endl;
        return;
    }

    string idPedido;
    cout << "Introduce el ID del pedido a mover: ";
    cin >> idPedido;

    int idDestino;
    cout << "Introduce el ID de la libreria destino: ";
    cin >> idDestino;
    cin.ignore(1000, '\n');

    NodoABB* nodoDestino = buscarNodoLibreria(raiz, idDestino);
    if (nodoDestino == NULL) {
        cout << "No existe la libreria destino." << endl;
        return;
    }

    Pedido extraido;
    int idOrigen = -1;
    bool ok = extraerPedidoEnArbolRec(raiz, idPedido, extraido, idOrigen);

    if (!ok) {
        cout << "No se ha encontrado el pedido con ese ID." << endl;
        return;
    }

    extraido.idLibreria = idDestino;  // actualizar id_libreria en el pedido
    insertarPedidoEnLista(nodoDestino->info.pedidos, extraido);

    cout << "Pedido " << idPedido << " movido de la libreria "
         << idOrigen << " a la libreria " << idDestino << "." << endl;
}

void mostrarEstadisticas(NodoABB* raiz) {
    if (raiz == NULL) {
        cout << "No hay librerias en el arbol." << endl;
        return;
    }

    int totalLibrerias = 0;
    int totalPedidos = 0;
    int idMaxPedidos = -1;
    int maxPedidos = -1;
    int totalUnidades = 0;

    acumularEstadisticas(raiz, totalLibrerias, totalPedidos,
                         idMaxPedidos, maxPedidos, totalUnidades);

    cout << "\n===== ESTADISTICAS DE LAS LIBRERIAS =====\n";
    cout << "Total de librerias: " << totalLibrerias << endl;
    cout << "Total de pedidos: " << totalPedidos << endl;
    cout << "Total de unidades solicitadas: " << totalUnidades << endl;

    if (idMaxPedidos != -1) {
        cout << "Libreria con mayor numero de pedidos: " << idMaxPedidos
             << " (" << maxPedidos << " pedidos)" << endl;
    } else {
        cout << "No hay pedidos registrados." << endl;
    }
}
