// Proyecto 2 - Terminal Unix
// Sistema de archivos simulado

#include <iostream>
#include <string>

using namespace std;

// estructura del nodo para el arbol
struct NodoABB {
    string nombre;
    bool esDirectorio;
    string contenido;
    NodoABB* izquierdo;
    NodoABB* derecho;
    NodoABB* padre;
    NodoABB* hijoRaiz;
};

// estructura principal del sistema
struct SistemaArchivos {
    NodoABB* raiz;
    NodoABB* actual;
    string archivoPersistencia;
};

// crear un nodo nuevo
NodoABB* crearNodo(const string& nombre, bool esDirectorio, const string& contenido = "") {
    NodoABB* nuevo = new NodoABB;
    nuevo->nombre = nombre;
    nuevo->esDirectorio = esDirectorio;
    nuevo->contenido = contenido;
    nuevo->izquierdo = NULL;
    nuevo->derecho = NULL;
    nuevo->padre = NULL;
    nuevo->hijoRaiz = NULL;
    return nuevo;
}

// insertar en el abb ordenado por nombre
NodoABB* insertarEnABB(NodoABB* raiz, NodoABB* nuevo) {
    if (raiz == NULL) {
        return nuevo;
    }
    
    if (nuevo->nombre < raiz->nombre) {
        raiz->izquierdo = insertarEnABB(raiz->izquierdo, nuevo);
    } else {
        raiz->derecho = insertarEnABB(raiz->derecho, nuevo);
    }
    
    return raiz;
}

// buscar en el abb por nombre
NodoABB* buscarEnABB(NodoABB* raiz, const string& nombre) {
    if (raiz == NULL || raiz->nombre == nombre) {
        return raiz;
    }
    
    if (nombre < raiz->nombre) {
        return buscarEnABB(raiz->izquierdo, nombre);
    } else {
        return buscarEnABB(raiz->derecho, nombre);
    }
}

// listar en orden alfabetico
void listarInorden(NodoABB* nodo) {
    if (nodo != NULL) {
        listarInorden(nodo->izquierdo);
        if (nodo->esDirectorio) {
            cout << nodo->nombre << "/" << endl;
        } else {
            cout << nodo->nombre << endl;
        }
        listarInorden(nodo->derecho);
    }
}

// obtener la ruta completa del nodo actual
string obtenerRutaCompleta(NodoABB* nodo) {
    if (nodo == NULL || nodo->padre == NULL) {
        return "/";
    }
    
    string ruta = obtenerRutaCompleta(nodo->padre);
    if (ruta != "/") {
        ruta += "/";
    }
    ruta += nodo->nombre;
    return ruta;
}

// inicializar el sistema
void inicializarSistema(SistemaArchivos& sistema, const string& archivoConfig) {
    sistema.archivoPersistencia = archivoConfig;
    sistema.raiz = crearNodo("/", true);
    sistema.actual = sistema.raiz;
}

int main() {
    SistemaArchivos sistema;
    inicializarSistema(sistema, "filesystem.txt");
    
    string comando;
    
    while (true) {
        cout << obtenerRutaCompleta(sistema.actual) << "$ ";
        cin >> comando;
        
        if (comando == "exit") {
            break;
        }
        else {
            cout << "comando no encontrado" << endl;
        }
    }
    
    return 0;
}
