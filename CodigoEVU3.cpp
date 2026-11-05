#include <iostream>
#include <fstream>
#include <cstring>
#include <limits>
using namespace std;

// ================= VALIDACIÓN Y LECTURA DE DATOS =================

bool entradaValida() {
    if (cin.fail()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return false;
    }
    return true;
}

bool leerEntero(const char mensaje[], int &valor) {
    cout << mensaje;
    cin >> valor;
    if (!entradaValida()) {
        return false;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return true;
}

bool leerTexto(const char mensaje[], char destino[], int tamano) {
    cout << mensaje;
    cin.getline(destino, tamano);
    if (cin.fail()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "(Texto muy largo: se guardaron los primeros "
             << tamano - 1 << " caracteres)\n";
    }
    if (strlen(destino) == 0) {
        cout << "\nEl texto no puede estar vacio.\n";
        return false;
    }
    return true;
}

// ================= ESTRUCTURAS Y ENTIDADES =================

// 1. GESTOR DE PROCESOS (Lista Enlazada Simple)
struct Proceso {
    int idProceso;
    char nombre[30];
    int prioridad; // Nivel de prioridad (ej. 1 a 10)
};

struct NodoLista {
    Proceso dato;
    NodoLista* siguiente;
};

// 2. GESTOR DE MEMORIA RAM (Pila LIFO)
struct BloqueMemoria {
    int idBloque;
    int tamanoMB;
};

struct NodoPila {
    BloqueMemoria dato;
    NodoPila* siguiente;
};

// 3. PLANIFICADOR DE CPU (Cola FIFO)
struct TareaCPU {
    int idTarea;
    int idProceso;
    char descripcion[50];
};

struct NodoCola {
    TareaCPU dato;
    NodoCola* siguiente;
};

struct PlanificadorCPU {
    NodoCola* frente;
    NodoCola* final;
};

// ================= PERSISTENCIA EN ARCHIVOS (<fstream>) =================

void guardarProcesos(NodoLista* cabeza) {
    ofstream archivo("procesos.txt");
    if (!archivo.is_open()) return;
    NodoLista* actual = cabeza;
    while (actual != NULL) {
        archivo << actual->dato.idProceso << "\n"
                << actual->dato.nombre << "\n"
                << actual->dato.prioridad << "\n";
        actual = actual->siguiente;
    }
    archivo.close();
}

void cargarProcesos(NodoLista* &cabeza) {
    ifstream archivo("procesos.txt");
    if (!archivo.is_open()) return;
    Proceso p;
    while (archivo >> p.idProceso) {
        archivo.ignore(numeric_limits<streamsize>::max(), '\n');
        archivo.getline(p.nombre, 30);
        archivo >> p.prioridad;
        archivo.ignore(numeric_limits<streamsize>::max(), '\n');

        NodoLista* nuevo = new NodoLista;
        nuevo->dato = p;
        nuevo->siguiente = NULL;

        if (cabeza == NULL) {
            cabeza = nuevo;
        } else {
            NodoLista* actual = cabeza;
            while (actual->siguiente != NULL) {
                actual = actual->siguiente;
            }
            actual->siguiente = nuevo;
        }
    }
    archivo.close();
}

void guardarMemoria(NodoPila* cima) {
    ofstream archivo("memoria.txt");
    if (!archivo.is_open()) return;
    NodoPila* actual = cima;
    while (actual != NULL) {
        archivo << actual->dato.idBloque << "\n"
                << actual->dato.tamanoMB << "\n";
        actual = actual->siguiente;
    }
    archivo.close();
}

void cargarMemoria(NodoPila* &cima) {
    ifstream archivo("memoria.txt");
    if (!archivo.is_open()) return;
    BloqueMemoria b;
    NodoPila* ultimo = NULL;
    while (archivo >> b.idBloque) {
        archivo >> b.tamanoMB;
        archivo.ignore(numeric_limits<streamsize>::max(), '\n');

        NodoPila* nuevo = new NodoPila;
        nuevo->dato = b;
        nuevo->siguiente = NULL;

        if (cima == NULL) {
            cima = nuevo;
        } else {
            ultimo->siguiente = nuevo;
        }
        ultimo = nuevo;
    }
    archivo.close();
}

void guardarTareas(const PlanificadorCPU& cpu) {
    ofstream archivo("tareas_cpu.txt");
    if (!archivo.is_open()) return;
    NodoCola* actual = cpu.frente;
    while (actual != NULL) {
        archivo << actual->dato.idTarea << "\n"
                << actual->dato.idProceso << "\n"
                << actual->dato.descripcion << "\n";
        actual = actual->siguiente;
    }
    archivo.close();
}

void cargarTareas(PlanificadorCPU& cpu) {
    ifstream archivo("tareas_cpu.txt");
    if (!archivo.is_open()) return;
    TareaCPU t;
    while (archivo >> t.idTarea) {
        archivo >> t.idProceso;
        archivo.ignore(numeric_limits<streamsize>::max(), '\n');
        archivo.getline(t.descripcion, 50);

        NodoCola* nuevo = new NodoCola;
        nuevo->dato = t;
        nuevo->siguiente = NULL;

        if (cpu.frente == NULL) {
            cpu.frente = nuevo;
        } else {
            cpu.final->siguiente = nuevo;
        }
        cpu.final = nuevo;
    }
    archivo.close();
}

// ================= MÓDULO 1: GESTOR DE PROCESOS =================

NodoLista* buscarProceso(NodoLista* cabeza, int idBuscado) {
    NodoLista* actual = cabeza;
    while (actual != NULL) {
        if (actual->dato.idProceso == idBuscado) {
            return actual;
        }
        actual = actual->siguiente;
    }
    return NULL;
}

void registrarProceso(NodoLista* &cabeza) {
    Proceso nuevoDato;
    if (!leerEntero("\nIngrese ID del proceso: ", nuevoDato.idProceso) || nuevoDato.idProceso <= 0) {
        cout << "\nID invalido. Debe ser un numero mayor que 0.\n";
        return;
    }
    if (buscarProceso(cabeza, nuevoDato.idProceso) != NULL) {
        cout << "\nYa existe un proceso con ese ID.\n";
        return;
    }
    if (!leerTexto("Ingrese nombre del proceso (ej. chrome.exe): ", nuevoDato.nombre, 30)) {
        cout << "No se registro el proceso.\n";
        return;
    }
    if (!leerEntero("Ingrese nivel de prioridad (1-10): ", nuevoDato.prioridad) || nuevoDato.prioridad <= 0) {
        cout << "\nPrioridad invalida. Debe ser un numero mayor que 0.\n";
        return;
    }

    NodoLista* nuevo = new NodoLista;
    nuevo->dato = nuevoDato;
    nuevo->siguiente = NULL;

    if (cabeza == NULL) {
        cabeza = nuevo;
    } else {
        NodoLista* actual = cabeza;
        while (actual->siguiente != NULL) {
            actual = actual->siguiente;
        }
        actual->siguiente = nuevo;
    }

    guardarProcesos(cabeza);
    cout << "\nProceso registrado correctamente en la lista.\n";
}

void mostrarProcesos(NodoLista* cabeza) {
    if (cabeza == NULL) {
        cout << "\nNo hay procesos registrados en el sistema.\n";
        return;
    }
    int total = 0;
    NodoLista* actual = cabeza;
    cout << "\n========== GESTOR DE PROCESOS (LISTA ENLAZADA) ==========\n";
    while (actual != NULL) {
        cout << "ID Proceso: " << actual->dato.idProceso << endl;
        cout << "Nombre    : " << actual->dato.nombre << endl;
        cout << "Prioridad : " << actual->dato.prioridad << endl;
        cout << "------------------------------------------\n";
        total++;
        actual = actual->siguiente;
    }
    cout << "Total de procesos: " << total << endl;
}

void consultarProceso(NodoLista* cabeza) {
    int id;
    if (!leerEntero("\nIngrese ID de proceso a buscar: ", id)) {
        cout << "\nID invalido.\n";
        return;
    }
    NodoLista* encontrado = buscarProceso(cabeza, id);
    if (encontrado != NULL) {
        cout << "\nProceso encontrado -> Nombre: " << encontrado->dato.nombre
             << " | Prioridad: " << encontrado->dato.prioridad << endl;
    } else {
        cout << "\nProceso no encontrado.\n";
    }
}

void modificarPrioridadProceso(NodoLista* cabeza) {
    int id;
    if (!leerEntero("\nIngrese ID del proceso a modificar: ", id)) {
        cout << "\nID invalido.\n";
        return;
    }
    NodoLista* encontrado = buscarProceso(cabeza, id);
    if (encontrado == NULL) {
        cout << "\nProceso no encontrado.\n";
        return;
    }
    cout << "Proceso: " << encontrado->dato.nombre << endl;
    cout << "Prioridad actual: " << encontrado->dato.prioridad << endl;

    int nuevaPrioridad;
    if (!leerEntero("Ingrese nueva prioridad: ", nuevaPrioridad) || nuevaPrioridad <= 0) {
        cout << "\nPrioridad invalida. Se mantiene la anterior.\n";
        return;
    }
    encontrado->dato.prioridad = nuevaPrioridad;
    guardarProcesos(cabeza);
    cout << "\nPrioridad actualizada correctamente.\n";
}

void ordenarProcesosPorPrioridad(NodoLista* cabeza) {
    if (cabeza == NULL || cabeza->siguiente == NULL) {
        cout << "\nNo hay suficientes procesos para ordenar.\n";
        return;
    }
    bool intercambio;
    do {
        intercambio = false;
        NodoLista* actual = cabeza;
        while (actual->siguiente != NULL) {
            if (actual->dato.prioridad < actual->siguiente->dato.prioridad) {
                Proceso temporal = actual->dato;
                actual->dato = actual->siguiente->dato;
                actual->siguiente->dato = temporal;
                intercambio = true;
            }
            actual = actual->siguiente;
        }
    } while (intercambio);
    guardarProcesos(cabeza);
    cout << "\nProcesos ordenados por prioridad (mayor a menor).\n";
}

void liberarProcesos(NodoLista* &cabeza) {
    while (cabeza != NULL) {
        NodoLista* aux = cabeza;
        cabeza = cabeza->siguiente;
        delete aux;
    }
}

// ================= MÓDULO 2: GESTOR DE MEMORIA RAM =================

NodoPila* buscarBloqueMemoria(NodoPila* cima, int idBuscado, int &posicion) {
    NodoPila* actual = cima;
    posicion = 1;
    while (actual != NULL) {
        if (actual->dato.idBloque == idBuscado) {
            return actual;
        }
        actual = actual->siguiente;
        posicion++;
    }
    return NULL;
}

void asignarMemoria(NodoPila* &cima) {
    BloqueMemoria nuevoDato;
    int posicion;
    if (!leerEntero("\nIngrese ID del bloque de memoria: ", nuevoDato.idBloque) || nuevoDato.idBloque <= 0) {
        cout << "\nID invalido. Debe ser mayor que 0.\n";
        return;
    }
    if (buscarBloqueMemoria(cima, nuevoDato.idBloque, posicion) != NULL) {
        cout << "\nYa existe un bloque con ese ID en la pila de memoria.\n";
        return;
    }
    if (!leerEntero("Ingrese tamano del bloque (en MB): ", nuevoDato.tamanoMB) || nuevoDato.tamanoMB <= 0) {
        cout << "\nTamano invalido.\n";
        return;
    }

    NodoPila* nuevo = new NodoPila;
    nuevo->dato = nuevoDato;
    nuevo->siguiente = cima;
    cima = nuevo;

    guardarMemoria(cima);
    cout << "\nBloque de memoria asignado en el tope de la RAM (Push).\n";
}

void liberarMemoria(NodoPila* &cima) {
    if (cima == NULL) {
        cout << "\nLa pila de memoria RAM esta vacia.\n";
        return;
    }
    NodoPila* aux = cima;
    cout << "\nBloque de memoria liberado del tope (Pop) -> ID: " << aux->dato.idBloque
         << " | Tamano: " << aux->dato.tamanoMB << " MB\n";

    cima = cima->siguiente;
    delete aux;
    guardarMemoria(cima);
}

void mostrarEstadoMemoria(NodoPila* cima) {
    if (cima == NULL) {
        cout << "\nNo hay bloques asignados en la memoria RAM.\n";
        return;
    }
    int total = 0;
    cout << "\n======= ESTADO DE MEMORIA RAM (PILA LIFO) =======\n";
    NodoPila* actual = cima;
    while (actual != NULL) {
        cout << "ID Bloque: " << actual->dato.idBloque
             << " | Tamano: " << actual->dato.tamanoMB << " MB";
        if (actual == cima) cout << " <- [TOPE RAM]";
        cout << endl;
        total++;
        actual = actual->siguiente;
    }
    cout << "Total de bloques asignados: " << total << endl;
}

void consultarBloqueMemoria(NodoPila* cima) {
    int id, posicion;
    if (!leerEntero("\nIngrese ID de bloque a buscar: ", id)) {
        cout << "\nID invalido.\n";
        return;
    }
    NodoPila* encontrado = buscarBloqueMemoria(cima, id, posicion);
    if (encontrado != NULL) {
        cout << "\nBloque encontrado -> ID: " << encontrado->dato.idBloque
             << " | Tamano: " << encontrado->dato.tamanoMB << " MB\n";
        cout << "Posicion desde el tope: " << posicion
             << " (debe liberar " << posicion - 1 << " bloque(s) antes)\n";
    } else {
        cout << "\nBloque no encontrado en la memoria RAM.\n";
    }
}

void liberarPilaMemoria(NodoPila* &cima) {
    while (cima != NULL) {
        NodoPila* aux = cima;
        cima = cima->siguiente;
        delete aux;
    }
}

// ================= MÓDULO 3: PLANIFICADOR DE CPU =================

NodoCola* buscarTarea(const PlanificadorCPU& cpu, int idBuscado) {
    NodoCola* actual = cpu.frente;
    while (actual != NULL) {
        if (actual->dato.idTarea == idBuscado) {
            return actual;
        }
        actual = actual->siguiente;
    }
    return NULL;
}

void encolarTarea(PlanificadorCPU& cpu) {
    TareaCPU nuevoDato;
    if (!leerEntero("\nIngrese ID de la tarea CPU: ", nuevoDato.idTarea) || nuevoDato.idTarea <= 0) {
        cout << "\nID de tarea invalido. Debe ser mayor que 0.\n";
        return;
    }
    if (buscarTarea(cpu, nuevoDato.idTarea) != NULL) {
        cout << "\nYa existe una tarea con ese ID en la cola.\n";
        return;
    }
    if (!leerEntero("Ingrese ID del proceso asociado: ", nuevoDato.idProceso) || nuevoDato.idProceso <= 0) {
        cout << "\nID de proceso invalido.\n";
        return;
    }
    if (!leerTexto("Ingrese descripcion de la tarea: ", nuevoDato.descripcion, 50)) {
        cout << "No se registro la tarea.\n";
        return;
    }

    NodoCola* nuevo = new NodoCola;
    nuevo->dato = nuevoDato;
    nuevo->siguiente = NULL;

    if (cpu.frente == NULL) {
        cpu.frente = nuevo;
    } else {
        cpu.final->siguiente = nuevo;
    }
    cpu.final = nuevo;

    guardarTareas(cpu);
    cout << "\nTarea registrada en la cola del Planificador de CPU (Encolar).\n";
}

void ejecutarTarea(PlanificadorCPU& cpu) {
    if (cpu.frente == NULL) {
        cout << "\nNo hay tareas pendientes en la CPU.\n";
        return;
    }
    NodoCola* aux = cpu.frente;
    cout << "\nTarea ejecutada y despachada por el procesador:\n";
    cout << "ID Tarea: " << aux->dato.idTarea
         << " | ID Proceso: " << aux->dato.idProceso
         << " | Descripcion: " << aux->dato.descripcion << endl;

    cpu.frente = cpu.frente->siguiente;
    if (cpu.frente == NULL) {
        cpu.final = NULL;
    }
    delete aux;
    guardarTareas(cpu);
}

void mostrarColaCPU(const PlanificadorCPU& cpu) {
    if (cpu.frente == NULL) {
        cout << "\nNo hay tareas pendientes en la cola de CPU.\n";
        return;
    }
    int total = 0;
    cout << "\n========== PLANIFICADOR DE CPU (COLA FIFO) ==========\n";
    NodoCola* actual = cpu.frente;
    while (actual != NULL) {
        cout << "ID Tarea: " << actual->dato.idTarea
             << " | ID Proceso: " << actual->dato.idProceso
             << " | Descripcion: " << actual->dato.descripcion;
        if (actual == cpu.frente) cout << " <- [SIGUIENTE EN CPU]";
        cout << endl;
        total++;
        actual = actual->siguiente;
    }
    cout << "Total de tareas pendientes: " << total << endl;
}

void modificarTarea(PlanificadorCPU& cpu) {
    if (cpu.frente == NULL) {
        cout << "\nNo hay tareas en la cola de CPU.\n";
        return;
    }
    int idProc;
    if (!leerEntero("\nIngrese el ID de proceso a modificar: ", idProc)) {
        cout << "\nID invalido.\n";
        return;
    }
    NodoCola* actual = cpu.frente;
    while (actual != NULL) {
        if (actual->dato.idProceso == idProc) {
            cout << "Tarea Nro. " << actual->dato.idTarea
                 << " - Descripcion actual: " << actual->dato.descripcion << endl;

            char nuevaDesc[50];
            if (!leerTexto("Ingrese la nueva descripcion de la tarea: ", nuevaDesc, 50)) {
                cout << "Se mantiene la descripcion anterior.\n";
                return;
            }
            strcpy(actual->dato.descripcion, nuevaDesc);
            guardarTareas(cpu);
            cout << "\nTarea actualizada correctamente.\n";
            return;
        }
        actual = actual->siguiente;
    }
    cout << "\nProceso no encontrado o sin tareas en la cola.\n";
}

void consultarTarea(const PlanificadorCPU& cpu) {
    int id;
    if (!leerEntero("\nIngrese ID de tarea a buscar: ", id)) {
        cout << "\nID invalido.\n";
        return;
    }
    int delante = 0;
    NodoCola* actual = cpu.frente;
    while (actual != NULL) {
        if (actual->dato.idTarea == id) {
            cout << "\nTarea encontrada -> ID Proceso: " << actual->dato.idProceso
                 << " | Descripcion: " << actual->dato.descripcion << endl;
            cout << "Tareas por ejecutar antes de esta: " << delante << endl;
            return;
        }
        delante++;
        actual = actual->siguiente;
    }
    cout << "\nTarea no encontrada en la cola de CPU.\n";
}

void liberarColaCPU(PlanificadorCPU& cpu) {
    while (cpu.frente != NULL) {
        NodoCola* aux = cpu.frente;
        cpu.frente = cpu.frente->siguiente;
        delete aux;
    }
    cpu.final = NULL;
}

// ================= MENÚS DE NAVEGACIÓN =================

void menuProcesos(NodoLista* &procesos) {
    int opcion;
    do {
        cout << "\n============================================\n";
        cout << "   GESTOR DE PROCESOS (LISTA ENLAZADA)\n";
        cout << "============================================\n";
        cout << "1. Registrar proceso\n";
        cout << "2. Mostrar procesos\n";
        cout << "3. Buscar proceso por ID\n";
        cout << "4. Modificar prioridad de proceso\n";
        cout << "5. Ordenar procesos por prioridad (mayor a menor)\n";
        cout << "6. Volver al menu principal\n";

        if (!leerEntero("Seleccione una opcion: ", opcion)) {
            opcion = 0;
        }
        switch (opcion) {
            case 1: registrarProceso(procesos); break;
            case 2: mostrarProcesos(procesos); break;
            case 3: consultarProceso(procesos); break;
            case 4: modificarPrioridadProceso(procesos); break;
            case 5: ordenarProcesosPorPrioridad(procesos); break;
            case 6: break;
            default: cout << "\nOpcion no valida. Intente de nuevo.\n";
        }
    } while (opcion != 6);
}

void menuMemoria(NodoPila* &memoria) {
    int opcion;
    do {
        cout << "\n============================================\n";
        cout << "   GESTOR DE MEMORIA RAM (PILA LIFO)\n";
        cout << "============================================\n";
        cout << "1. Asignar bloque de memoria (Push)\n";
        cout << "2. Liberar bloque de memoria (Pop)\n";
        cout << "3. Mostrar estado de la memoria RAM\n";
        cout << "4. Buscar bloque de memoria por ID\n";
        cout << "5. Volver al menu principal\n";

        if (!leerEntero("Seleccione una opcion: ", opcion)) {
            opcion = 0;
        }
        switch (opcion) {
            case 1: asignarMemoria(memoria); break;
            case 2: liberarMemoria(memoria); break;
            case 3: mostrarEstadoMemoria(memoria); break;
            case 4: consultarBloqueMemoria(memoria); break;
            case 5: break;
            default: cout << "\nOpcion no valida. Intente de nuevo.\n";
        }
    } while (opcion != 5);
}

void menuCPU(PlanificadorCPU& cpu) {
    int opcion;
    do {
        cout << "\n============================================\n";
        cout << "   PLANIFICADOR DE CPU (COLA FIFO)\n";
        cout << "============================================\n";
        cout << "1. Encolar tarea en CPU\n";
        cout << "2. Ejecutar tarea en CPU (Desencolar)\n";
        cout << "3. Mostrar cola de tareas de CPU\n";
        cout << "4. Modificar tarea por ID de proceso\n";
        cout << "5. Buscar tarea por ID\n";
        cout << "6. Volver al menu principal\n";

        if (!leerEntero("Seleccione una opcion: ", opcion)) {
            opcion = 0;
        }
        switch (opcion) {
            case 1: encolarTarea(cpu); break;
            case 2: ejecutarTarea(cpu); break;
            case 3: mostrarColaCPU(cpu); break;
            case 4: modificarTarea(cpu); break;
            case 5: consultarTarea(cpu); break;
            case 6: break;
            default: cout << "\nOpcion no valida. Intente de nuevo.\n";
        }
    } while (opcion != 6);
}

// ================= FUNCIÓN PRINCIPAL =================

int main() {
    NodoLista* procesos = NULL;
    NodoPila* memoria = NULL;
    PlanificadorCPU cpu = {NULL, NULL};

    // Carga de la persistencia guardada previamente
    cargarProcesos(procesos);
    cargarMemoria(memoria);
    cargarTareas(cpu);

    int opcion;

    do {
        cout << "\n============================================\n";
        cout << "  SISTEMA DE GESTION DE PROCESOS - SO\n";
        cout << "============================================\n";
        cout << "1. Gestor de Procesos (Lista Enlazada)\n";
        cout << "2. Gestor de Memoria RAM (Pila LIFO)\n";
        cout << "3. Planificador de CPU (Cola FIFO)\n";
        cout << "4. Salir del programa\n";

        if (!leerEntero("Seleccione una opcion: ", opcion)) {
            opcion = 0;
        }

        switch (opcion) {
            case 1: menuProcesos(procesos); break;
            case 2: menuMemoria(memoria); break;
            case 3: menuCPU(cpu); break;
            case 4:
                guardarProcesos(procesos);
                guardarMemoria(memoria);
                guardarTareas(cpu);
                liberarProcesos(procesos);
                liberarPilaMemoria(memoria);
                liberarColaCPU(cpu);
                cout << "\nDatos guardados en disco y memoria liberada correctamente.\n";
                cout << "Programa finalizado.\n";
                break;
            default:
                cout << "\nOpcion no valida. Intente de nuevo.\n";
        }
    } while (opcion != 4);

    return 0;
}
