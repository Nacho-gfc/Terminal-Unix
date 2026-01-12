// Proyecto 2 - Terminal Unix
// Sistema de archivos simulado
// agregue rm y rmdir para eliminar

#include <iostream>
#include <string>
#include <cstdio>
#include <cstring>

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

void guardarNodoRecursivo(FILE* archivo, NodoABB* nodo, const string& rutaBase) {
    if (nodo == NULL) return;
    string rutaCompleta = rutaBase;
    if (rutaBase != "/") rutaCompleta += "/";
    rutaCompleta += nodo->nombre;
    if (nodo->esDirectorio) {
        fprintf(archivo, "DIR|%s|\n", rutaCompleta.c_str());
        guardarNodoRecursivo(archivo, nodo->hijoRaiz, rutaCompleta);
    } else {
        fprintf(archivo, "FILE|%s|%s\n", rutaCompleta.c_str(), nodo->contenido.c_str());
    }
    guardarNodoRecursivo(archivo, nodo->izquierdo, rutaBase);
    guardarNodoRecursivo(archivo, nodo->derecho, rutaBase);
}

void guardarEnArchivo(SistemaArchivos& sistema) {
    FILE* archivo = fopen(sistema.archivoPersistencia.c_str(), "w");
    if (archivo == NULL) {
        cout << "error al guardar" << endl;
        return;
    }
    if (sistema.raiz->hijoRaiz != NULL) {
        guardarNodoRecursivo(archivo, sistema.raiz->hijoRaiz, "/");
    }
    fclose(archivo);
}

void cargarDesdeArchivo(SistemaArchivos& sistema) {
    FILE* archivo = fopen(sistema.archivoPersistencia.c_str(), "r");
    if (archivo == NULL) {
        return;
    }
    char linea[1000];
    while (fgets(linea, sizeof(linea), archivo)) {
        linea[strcspn(linea, "\n")] = 0;
        if (strlen(linea) == 0) continue;
        char* tipo = strtok(linea, "|");
        char* ruta = strtok(NULL, "|");
        char* contenido = strtok(NULL, "|");
        if (tipo == NULL || ruta == NULL) continue;
        string rutaStr(ruta);
        string contenidoStr = (contenido != NULL) ? string(contenido) : "";
        bool esDir = (strcmp(tipo, "DIR") == 0);
        size_t ultimaBarra = rutaStr.find_last_of('/');
        if (ultimaBarra == string::npos) continue;
        string rutaPadre = rutaStr.substr(0, ultimaBarra);
        string nombreFinal = rutaStr.substr(ultimaBarra + 1);
        if (rutaPadre.length() == 0) rutaPadre = "/";
        NodoABB* padre = navegarRuta(sistema, rutaPadre);
        if (padre == NULL) continue;
        if (buscarEnABB(padre->hijoRaiz, nombreFinal) == NULL) {
            NodoABB* nuevo = crearNodo(nombreFinal, esDir, contenidoStr);
            nuevo->padre = padre;
            padre->hijoRaiz = insertarEnABB(padre->hijoRaiz, nuevo);
        }
    }
    fclose(archivo);
}

void inicializarSistema(SistemaArchivos& sistema, const string& archivoConfig) {
    sistema.archivoPersistencia = archivoConfig;
    sistema.raiz = crearNodo("/", true);
    sistema.actual = sistema.raiz;
}

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

void comandoCd(SistemaArchivos& sistema, const string& ruta) {
    NodoABB* destino = navegarRuta(sistema, ruta);
    if (destino == NULL) {
        cout << "cd: no se encontro " << ruta << endl;
        return;
    }
    sistema.actual = destino;
}

void comandoMkdir(SistemaArchivos& sistema, const string& ruta) {
    string rutaPadre, nombreCarpeta;
    separarRuta(ruta, rutaPadre, nombreCarpeta);
    if (nombreCarpeta.length() == 0) {
        cout << "mkdir: falta el nombre" << endl;
        return;
    }
    NodoABB* dirPadre;
    if (rutaPadre.length() == 0) {
        dirPadre = sistema.actual;
    } else {
        dirPadre = navegarRuta(sistema, rutaPadre);
    }
    if (dirPadre == NULL) {
        cout << "mkdir: ruta no existe" << endl;
        return;
    }
    if (buscarEnABB(dirPadre->hijoRaiz, nombreCarpeta) != NULL) {
        cout << "mkdir: ya existe " << nombreCarpeta << endl;
        return;
    }
    NodoABB* nuevaCarpeta = crearNodo(nombreCarpeta, true);
    nuevaCarpeta->padre = dirPadre;
    dirPadre->hijoRaiz = insertarEnABB(dirPadre->hijoRaiz, nuevaCarpeta);
}

void comandoTouch(SistemaArchivos& sistema, const string& ruta) {
    string rutaPadre, nombreArchivo;
    separarRuta(ruta, rutaPadre, nombreArchivo);
    if (nombreArchivo.length() == 0) {
        cout << "touch: falta el nombre" << endl;
        return;
    }
    NodoABB* dirPadre;
    if (rutaPadre.length() == 0) {
        dirPadre = sistema.actual;
    } else {
        dirPadre = navegarRuta(sistema, rutaPadre);
    }
    if (dirPadre == NULL) {
        cout << "touch: ruta no existe" << endl;
        return;
    }
    if (buscarEnABB(dirPadre->hijoRaiz, nombreArchivo) != NULL) {
        cout << "touch: ya existe " << nombreArchivo << endl;
        return;
    }
    NodoABB* nuevoArchivo = crearNodo(nombreArchivo, false);
    nuevoArchivo->padre = dirPadre;
    dirPadre->hijoRaiz = insertarEnABB(dirPadre->hijoRaiz, nuevoArchivo);
}

void comandoMv(SistemaArchivos& sistema, const string& origen, const string& destino) {
    NodoABB* nodoOrigen = buscarPorRuta(sistema, origen);
    if (nodoOrigen == NULL) {
        cout << "mv: no existe " << origen << endl;
        return;
    }
    NodoABB* padreOrigen = nodoOrigen->padre;
    string rutaPadreDestino, nombreDestino;
    separarRuta(destino, rutaPadreDestino, nombreDestino);
    NodoABB* padreDestino;
    if (rutaPadreDestino.length() == 0) {
        padreDestino = sistema.actual;
    } else {
        padreDestino = navegarRuta(sistema, rutaPadreDestino);
    }
    if (padreDestino == NULL) {
        cout << "mv: ruta destino no existe" << endl;
        return;
    }
    if (buscarEnABB(padreDestino->hijoRaiz, nombreDestino) != NULL) {
        cout << "mv: ya existe " << nombreDestino << endl;
        return;
    }
    if (padreOrigen == padreDestino) {
        nodoOrigen->nombre = nombreDestino;
    } else {
        string nombreViejo = nodoOrigen->nombre;
        NodoABB* nodoMovido = NULL;
        padreOrigen->hijoRaiz = quitarDeABB(padreOrigen->hijoRaiz, nombreViejo, nodoMovido);
        if (nodoMovido != NULL) {
            nodoMovido->nombre = nombreDestino;
            nodoMovido->padre = padreDestino;
            padreDestino->hijoRaiz = insertarEnABB(padreDestino->hijoRaiz, nodoMovido);
        }
    }
}

// comando rm para eliminar archivos
void comandoRm(SistemaArchivos& sistema, const string& ruta) {
    string rutaPadre, nombreArchivo;
    separarRuta(ruta, rutaPadre, nombreArchivo);
    
    NodoABB* dirPadre;
    if (rutaPadre.length() == 0) {
        dirPadre = sistema.actual;
    } else {
        dirPadre = navegarRuta(sistema, rutaPadre);
    }
    
    if (dirPadre == NULL) {
        cout << "rm: ruta no existe" << endl;
        return;
    }
    
    NodoABB* nodo = buscarEnABB(dirPadre->hijoRaiz, nombreArchivo);
    
    if (nodo == NULL) {
        cout << "rm: no existe " << nombreArchivo << endl;
        return;
    }
    
    if (nodo->esDirectorio) {
        cout << "rm: es una carpeta " << nombreArchivo << endl;
        return;
    }
    
    dirPadre->hijoRaiz = eliminarDeABB(dirPadre->hijoRaiz, nombreArchivo);
}

// comando rmdir para eliminar carpetas vacias
void comandoRmdir(SistemaArchivos& sistema, const string& ruta) {
    string rutaPadre, nombreCarpeta;
    separarRuta(ruta, rutaPadre, nombreCarpeta);
    
    NodoABB* dirPadre;
    if (rutaPadre.length() == 0) {
        dirPadre = sistema.actual;
    } else {
        dirPadre = navegarRuta(sistema, rutaPadre);
    }
    
    if (dirPadre == NULL) {
        cout << "rmdir: ruta no existe" << endl;
        return;
    }
    
    NodoABB* nodo = buscarEnABB(dirPadre->hijoRaiz, nombreCarpeta);
    
    if (nodo == NULL) {
        cout << "rmdir: no existe " << nombreCarpeta << endl;
        return;
    }
    
    if (!nodo->esDirectorio) {
        cout << "rmdir: no es carpeta " << nombreCarpeta << endl;
        return;
    }
    
    if (nodo->hijoRaiz != NULL) {
        cout << "rmdir: carpeta no vacia " << nombreCarpeta << endl;
        return;
    }
    
    dirPadre->hijoRaiz = eliminarDeABB(dirPadre->hijoRaiz, nombreCarpeta);
}

int main() {
    SistemaArchivos sistema;
    inicializarSistema(sistema, "filesystem.txt");
    cargarDesdeArchivo(sistema);
    
    string comando, parametro1, parametro2;
    
    while (true) {
        cout << obtenerRutaCompleta(sistema.actual) << "$ ";
        cin >> comando;
        
        if (comando == "exit") {
            guardarEnArchivo(sistema);
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
        else if (comando == "mkdir") {
            cin >> parametro1;
            comandoMkdir(sistema, parametro1);
        }
        else if (comando == "touch") {
            cin >> parametro1;
            comandoTouch(sistema, parametro1);
        }
        else if (comando == "mv") {
            cin >> parametro1 >> parametro2;
            comandoMv(sistema, parametro1, parametro2);
        }
        else if (comando == "rm") {
            cin >> parametro1;
            comandoRm(sistema, parametro1);
        }
        else if (comando == "rmdir") {
            cin >> parametro1;
            comandoRmdir(sistema, parametro1);
        }
        else {
            cout << "comando no encontrado" << endl;
        }
    }
    
    return 0;
}
