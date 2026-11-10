#include <cassert>
#include "control.cpp"

int main() {
    NodoABB* raiz = NULL;
    bool ok = false;
    for (int id : {20, 10, 30, 25, 40}) {
        Libreria lib = {id, "Prueba", NULL};
        raiz = insertarNodoLibreria(raiz, lib, ok);
        assert(ok);
    }
    Pedido pedido = {25, "P00001", "123A45", "Historia", 2, "01-01-2025"};
    insertarPedidoEnLista(buscarNodoLibreria(raiz, 25)->info.pedidos, pedido);
    raiz = borrarNodoLibreria(raiz, 20, ok);
    assert(ok && buscarNodoLibreria(raiz, 20) == NULL);
    NodoABB* sucesor = buscarNodoLibreria(raiz, 25);
    assert(sucesor && sucesor->info.pedidos);
    assert(sucesor->info.pedidos->info.idPedido == "P00001");
    liberarMemoria(raiz);
    assert(raiz == NULL);
    std::cout << "OK: borrado con dos hijos conserva los pedidos y permite liberar memoria.\n";
}
