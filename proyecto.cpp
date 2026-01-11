// Proyecto 2 - Terminal Unix
// Sistema de archivos simulado
// agregue comandos ls y cd

#include <iostream>
#include <string>

using namespace std;

struct NodoABB {
    string nombre;
    bool esDirectorio;
    string contenido;
    NodoABB* izquierdo;
    NodoABB* derecho;
    NodoABB* padre;
    NodoABB* hijoRaiz;
};

struct SistemaArchivos {
    NodoABB* raiz;
    NodoABB* actual;
    string archivoPersistencia;
};

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

NodoABB* quitarDeABB(NodoABB* raiz, const string& nombre, NodoABB*& nodoQuitado) {
    if (raiz == NULL) {
        nodoQuitado = NULL;
        return raiz;
    }
    if (nombre < raiz->nombre) {
        raiz->izquierdo = quitarDeABB(raiz->izquierdo, nombre, nodoQuitado);
    } else if (nombre > raiz->nombre) {
        raiz->derecho = quitarDeABB(raiz->derecho, nombre, nodoQuitado);
    } else {
        nodoQuitado = raiz;
        if (raiz->izquierdo == NULL) {
            return raiz->derecho;
        } else if (raiz->derecho == NULL) {
            return raiz->izquierdo;
        }
        NodoABB* sucesor = raiz->derecho;
        NodoABB* padreSucesor = raiz;
        while (sucesor->izquierdo != NULL) {
            padreSucesor = sucesor;
            sucesor = sucesor->izquierdo;
        }
        if (padreSucesor != raiz) {
            padreSucesor->izquierdo = sucesor->derecho;
            sucesor->derecho = raiz->derecho;
        }
        sucesor->izquierdo = raiz->izquierdo;
        nodoQuitado->izquierdo = NULL;
        nodoQuitado->derecho = NULL;
        return sucesor;
    }
    return raiz;
}

NodoABB* eliminarDeABB(NodoABB* raiz, const string& nombre) {
    NodoABB* nodoQuitado = NULL;
    NodoABB* resultado = quitarDeABB(raiz, nombre, nodoQuitado);
    if (nodoQuitado != NULL) {
        delete nodoQuitado;
    }
    return resultado;
}

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

// navegar por una ruta
NodoABB* navegarRuta(SistemaArchivos& sistema, const string& ruta) {
    if (ruta == "/") {
        return sistema.raiz;
    }
    
    NodoABB* actual = (ruta[0] == '/') ? sistema.raiz : sistema.actual;
    
    string rutaTrabajo = ruta;
    if (rutaTrabajo[0] == '/') {
        rutaTrabajo = rutaTrabajo.substr(1);
    }
    
    size_t inicio = 0;
    size_t fin = 0;
    
    while (fin != string::npos) {
        fin = rutaTrabajo.find('/', inicio);
        string componente = rutaTrabajo.substr(inicio, fin - inicio);
        
        if (componente.length() > 0) {
            if (componente == "..") {
                if (actual->padre != NULL) {
                    actual = actual->padre;
                }
            } else {
                NodoABB* encontrado = buscarEnABB(actual->hijoRaiz, componente);
                if (encontrado == NULL || !encontrado->esDirectorio) {
                    return NULL;
                }
                actual = encontrado;
            }
        }
        inicio = fin + 1;
    }
    return actual;
}

// separar ruta en padre y nombre
void separarRuta(const string& ruta, string& rutaPadre, string& nombreFinal) {
    size_t pos = ruta.find_last_of('/');
    if (pos == string::npos) {
        rutaPadre = "";
        nombreFinal = ruta;
    } else if (pos == 0) {
        rutaPadre = "/";
        nombreFinal = ruta.substr(1);
    } else {
        rutaPadre = ruta.substr(0, pos);
        nombreFinal = ruta.substr(pos + 1);
    }
}

// buscar nodo por ruta completa
NodoABB* buscarPorRuta(SistemaArchivos& sistema, const string& ruta) {
    string rutaPadre, nombreFinal;
    separarRuta(ruta, rutaPadre, nombreFinal);
    
    NodoABB* dirPadre;
    if (rutaPadre.length() == 0) {
        dirPadre = sistema.actual;
    } else {
        dirPadre = navegarRuta(sistema, rutaPadre);
    }
    
    if (dirPadre == NULL) {
        return NULL;
    }
    return buscarEnABB(dirPadre->hijoRaiz, nombreFinal);
}

void inicializarSistema(SistemaArchivos& sistema, const string& archivoConfig) {
    sistema.archivoPersistencia = archivoConfig;
    sistema.raiz = crearNodo("/", true);
    sistema.actual = sistema.raiz;
}

// comando ls
void comandoLs(SistemaArchivos& sistema, const string& ruta) {
    NodoABB* dir;
    
    if (ruta.length() == 0) {
        dir = sistema.actual;
    } else {
        dir = navegarRuta(sistema, ruta);
        if (dir == NULL) {
            NodoABB* nodo = buscarPorRuta(sistema, ruta);
            if (nodo != NULL && !nodo->esDirectorio) {
                cout << nodo->nombre << endl;
                return;
            }
            cout << "ls: no existe " << ruta << endl;
            return;
        }
    }
    
    if (dir->hijoRaiz == NULL) {
        return;
    }
    listarInorden(dir->hijoRaiz);
}

// comando cd
void comandoCd(SistemaArchivos& sistema, const string& ruta) {
    NodoABB* destino = navegarRuta(sistema, ruta);
    
    if (destino == NULL) {
        cout << "cd: no se encontro " << ruta << endl;
        return;
    }
    sistema.actual = destino;
}

int main() {
    SistemaArchivos sistema;
    inicializarSistema(sistema, "filesystem.txt");
    
    string comando, parametro1;
    
    while (true) {
        cout << obtenerRutaCompleta(sistema.actual) << "$ ";
        cin >> comando;
        
        if (comando == "exit") {
            break;
        }
        else if (comando == "ls") {
            char siguiente = cin.peek();
            if (siguiente == '\n' || siguiente == EOF) {
                comandoLs(sistema, "");
            } else {
                cin >> parametro1;
                comandoLs(sistema, parametro1);
            }
        }
        else if (comando == "cd") {
            cin >> parametro1;
            comandoCd(sistema, parametro1);
        }
        else {
            cout << "comando no encontrado" << endl;
        }
    }
    
    return 0;
}
