#include <iostream>
using namespace std;

struct Proceso {
    int id;
    int tiempoRafaga;
    int tiempoLlegada;
    int tiempoEspera;
    int tiempoFinalizacion;
    int tiempoRetorno;
    int inicio; // Tiempo en el que comenzó a ejecutarse
    int tiempoRafagaRestante; // Para algoritmos expropiativos
};

// Función para reiniciar los valores de los procesos
void reiniciarProcesos(Proceso procesos[], int n) {
    for (int i = 0; i < n; i++) {
        procesos[i].tiempoEspera = 0;
        procesos[i].tiempoFinalizacion = 0;
        procesos[i].tiempoRetorno = 0;
        procesos[i].inicio = 0;
        procesos[i].tiempoRafagaRestante = procesos[i].tiempoRafaga;
    }
}

// Función para copiar un arreglo de procesos
void copiarProcesos(Proceso destino[], Proceso fuente[], int n) {
    for (int i = 0; i < n; i++) {
        destino[i] = fuente[i];
    }
}

// Función para calcular el tiempo de espera y finalización en FCFS
void FCFS(Proceso procesos[], int n) {
    int tiempoActual = 0;
    for (int i = 0; i < n; i++) {
        if (tiempoActual < procesos[i].tiempoLlegada) {
            tiempoActual = procesos[i].tiempoLlegada;
        }
        procesos[i].inicio = tiempoActual;
        procesos[i].tiempoEspera = tiempoActual - procesos[i].tiempoLlegada;
        procesos[i].tiempoFinalizacion = tiempoActual + procesos[i].tiempoRafaga;
        procesos[i].tiempoRetorno = procesos[i].tiempoFinalizacion - procesos[i].tiempoLlegada;
        tiempoActual = procesos[i].tiempoFinalizacion;
    }
}

// Función para calcular el tiempo de espera y finalización en SPF (No expropiativo)
void SPF(Proceso procesos[], int n) {
    // Ordenar por tiempo de llegada (burbuja)
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (procesos[j].tiempoLlegada > procesos[j + 1].tiempoLlegada) {
                // Intercambiar procesos[j] y procesos[j + 1]
                Proceso temp = procesos[j];
                procesos[j] = procesos[j + 1];
                procesos[j + 1] = temp;
            }
        }
    }

    int tiempoActual = 0;
    for (int i = 0; i < n; i++) {
        int idx = -1;
        int menorRafaga = 999999; // Valor grande para encontrar el mínimo
        for (int j = i; j < n; j++) {
            if (procesos[j].tiempoLlegada <= tiempoActual && procesos[j].tiempoRafaga < menorRafaga) {
                menorRafaga = procesos[j].tiempoRafaga;
                idx = j;
            }
        }
        if (idx != -1) {
            // Intercambiar procesos[i] y procesos[idx]
            Proceso temp = procesos[i];
            procesos[i] = procesos[idx];
            procesos[idx] = temp;

            // Calcular tiempos
            procesos[i].inicio = tiempoActual;
            procesos[i].tiempoEspera = tiempoActual - procesos[i].tiempoLlegada;
            procesos[i].tiempoFinalizacion = tiempoActual + procesos[i].tiempoRafaga;
            procesos[i].tiempoRetorno = procesos[i].tiempoFinalizacion - procesos[i].tiempoLlegada;
            tiempoActual = procesos[i].tiempoFinalizacion;
        } else {
            // Si no hay procesos listos, avanzar el tiempo al siguiente proceso
            tiempoActual = procesos[i].tiempoLlegada;
            procesos[i].inicio = tiempoActual;
            procesos[i].tiempoEspera = 0;
            procesos[i].tiempoFinalizacion = tiempoActual + procesos[i].tiempoRafaga;
            procesos[i].tiempoRetorno = procesos[i].tiempoFinalizacion - procesos[i].tiempoLlegada;
            tiempoActual = procesos[i].tiempoFinalizacion;
        }
    }
}

// Función para calcular el tiempo de espera y finalización en HRRN
void HRRN(Proceso procesos[], int n) {
    int tiempoActual = 0;
    for (int i = 0; i < n; i++) {
        int idx = -1;
        float maxRatio = -1;
        for (int j = 0; j < n; j++) {
            if (procesos[j].tiempoLlegada <= tiempoActual && procesos[j].tiempoFinalizacion == 0) {
                float ratio = (float)(tiempoActual - procesos[j].tiempoLlegada + procesos[j].tiempoRafaga) / procesos[j].tiempoRafaga;
                if (ratio > maxRatio) {
                    maxRatio = ratio;
                    idx = j;
                }
            }
        }
        if (idx != -1) {
            procesos[idx].inicio = tiempoActual;
            procesos[idx].tiempoEspera = tiempoActual - procesos[idx].tiempoLlegada;
            procesos[idx].tiempoFinalizacion = tiempoActual + procesos[idx].tiempoRafaga;
            procesos[idx].tiempoRetorno = procesos[idx].tiempoFinalizacion - procesos[idx].tiempoLlegada;
            tiempoActual = procesos[idx].tiempoFinalizacion;
        } else {
            tiempoActual++;
            i--;
        }
    }
}

// Función para calcular el tiempo de espera y finalización en Round Robin
void RoundRobin(Proceso procesos[], int n) {
    int tiempoActual = 0;
    int completados = 0;
    int tiemposRafagaRestantes[n];
    bool procesoIniciado[n]; // Para registrar si el proceso ya ha comenzado
    for (int i = 0; i < n; i++) {
        tiemposRafagaRestantes[i] = procesos[i].tiempoRafaga;
        procesoIniciado[i] = false; // Inicialmente, ningún proceso ha comenzado
    }

    while (completados < n) {
        bool procesoEjecutado = false;
        for (int i = 0; i < n; i++) {
            if (procesos[i].tiempoLlegada <= tiempoActual && tiemposRafagaRestantes[i] > 0) {
                procesoEjecutado = true;

                // Registrar el inicio si es la primera vez que el proceso se ejecuta
                if (!procesoIniciado[i]) {
                    procesos[i].inicio = tiempoActual;
                    procesoIniciado[i] = true;
                }

                // Ejecutar el proceso por un quantum de 1
                int tiempoEjecucion = min(1, tiemposRafagaRestantes[i]);
                tiemposRafagaRestantes[i] -= tiempoEjecucion;
                tiempoActual += tiempoEjecucion;

                if (tiemposRafagaRestantes[i] == 0) {
                    completados++;
                    procesos[i].tiempoFinalizacion = tiempoActual;
                    procesos[i].tiempoEspera = procesos[i].tiempoFinalizacion - procesos[i].tiempoLlegada - procesos[i].tiempoRafaga;
                    procesos[i].tiempoRetorno = procesos[i].tiempoFinalizacion - procesos[i].tiempoLlegada;
                }
            }
        }
        if (!procesoEjecutado) {
            tiempoActual++;
        }
    }
}

// Función para calcular el tiempo de espera y finalización en SPN (Shortest Process Next)
void SPN(Proceso procesos[], int n) {
    int tiempoActual = 0;
    int completados = 0;

    while (completados < n) {
        int idx = -1;
        int menorRafaga = 999999;

        // Buscar el proceso con la menor ráfaga que esté listo para ejecutarse
        for (int i = 0; i < n; i++) {
            if (procesos[i].tiempoLlegada <= tiempoActual && procesos[i].tiempoRafagaRestante > 0) {
                if (procesos[i].tiempoRafagaRestante < menorRafaga) {
                    menorRafaga = procesos[i].tiempoRafagaRestante;
                    idx = i;
                }
            }
        }

        if (idx != -1) {
            // Registrar el tiempo de inicio justo antes de ejecutar el proceso
            if (procesos[idx].inicio == 0) {
                procesos[idx].inicio = tiempoActual + 1; // Corrección aquí
            }

            // Ejecutar el proceso seleccionado por 1 unidad de tiempo
            procesos[idx].tiempoRafagaRestante--;
            tiempoActual++;

            // Si el proceso ha terminado, calcular sus tiempos
            if (procesos[idx].tiempoRafagaRestante == 0) {
                procesos[idx].tiempoFinalizacion = tiempoActual;
                procesos[idx].tiempoEspera = procesos[idx].tiempoFinalizacion - procesos[idx].tiempoLlegada - procesos[idx].tiempoRafaga;
                procesos[idx].tiempoRetorno = procesos[idx].tiempoFinalizacion - procesos[idx].tiempoLlegada;
                completados++;
            }
        } else {
            // Si no hay procesos listos, avanzar el tiempo
            tiempoActual++;
        }
    }
}

// Función para calcular el tiempo de espera y finalización en Retroalimentación (Multilevel Feedback Queue)
void Retroalimentacion(Proceso procesos[], int n) {
    int tiempoActual = 0;
    int completados = 0;
    int tiemposRafagaRestantes[n];
    bool procesoIniciado[n]; // Para registrar si el proceso ya ha comenzado
    for (int i = 0; i < n; i++) {
        tiemposRafagaRestantes[i] = procesos[i].tiempoRafaga;
        procesoIniciado[i] = false; // Inicialmente, ningún proceso ha comenzado
    }

    while (completados < n) {
        bool procesoEjecutado = false;
        for (int i = 0; i < n; i++) {
            if (procesos[i].tiempoLlegada <= tiempoActual && tiemposRafagaRestantes[i] > 0) {
                procesoEjecutado = true;

                // Registrar el inicio si es la primera vez que el proceso se ejecuta
                if (!procesoIniciado[i]) {
                    procesos[i].inicio = tiempoActual;
                    procesoIniciado[i] = true;
                }

                // Ejecutar el proceso por un quantum de 1
                int tiempoEjecucion = min(1, tiemposRafagaRestantes[i]);
                tiemposRafagaRestantes[i] -= tiempoEjecucion;
                tiempoActual += tiempoEjecucion;

                if (tiemposRafagaRestantes[i] == 0) {
                    completados++;
                    procesos[i].tiempoFinalizacion = tiempoActual;
                    procesos[i].tiempoEspera = procesos[i].tiempoFinalizacion - procesos[i].tiempoLlegada - procesos[i].tiempoRafaga;
                    procesos[i].tiempoRetorno = procesos[i].tiempoFinalizacion - procesos[i].tiempoLlegada;
                }
            }
        }
        if (!procesoEjecutado) {
            tiempoActual++;
        }
    }
}

// Función para mostrar los resultados
void mostrarProcesos(Proceso procesos[], int n) {
    cout << "Proceso\tInicio\tTiempo de espera\tFinalizacion\tTiempo de retorno\tTiempo de llegada" << endl;
    for (int i = 0; i < n; i++) {
        cout << procesos[i].id << "\t" << procesos[i].inicio << "\t\t" << procesos[i].tiempoEspera << "\t\t" 
             << procesos[i].tiempoFinalizacion << "\t\t" << procesos[i].tiempoRetorno << "\t\t" << procesos[i].tiempoLlegada << endl;
    }
}

int main() {
    int n;
    Proceso procesos[100]; // Máximo 100 procesos
    Proceso procesosOriginal[100]; // Copia de los procesos originales
    bool continuar = true;

    while (continuar) {
        cout << "Ingrese el numero de procesos: ";
        cin >> n;

        for (int i = 0; i < n; i++) {
            procesos[i].id = i + 1;
            cout << "Ingrese el tiempo de rafaga del proceso " << i + 1 << ": ";
            cin >> procesos[i].tiempoRafaga;
            cout << "Ingrese el tiempo de llegada del proceso " << i + 1 << ": ";
            cin >> procesos[i].tiempoLlegada;
            procesos[i].tiempoEspera = 0;
            procesos[i].tiempoFinalizacion = 0;
            procesos[i].tiempoRetorno = 0;
            procesos[i].inicio = 0;
            procesos[i].tiempoRafagaRestante = procesos[i].tiempoRafaga;
        }

        // Guardar una copia de los procesos originales
        copiarProcesos(procesosOriginal, procesos, n);

        bool elegirOtroAlgoritmo = true;
        while (elegirOtroAlgoritmo) {
            // Restaurar los procesos originales antes de aplicar un nuevo algoritmo
            copiarProcesos(procesos, procesosOriginal, n);
            reiniciarProcesos(procesos, n); // Reiniciar valores antes de aplicar un nuevo algoritmo

            int opcion;
            cout << "Seleccione el algoritmo de planificacion:\n";
            cout << "1. FCFS (No Expropiativo)\n";
            cout << "2. SPF (No Expropiativo)\n";
            cout << "3. HRRN (No Expropiativo)\n";
            cout << "4. Round Robin (Expropiativo)\n";
            cout << "5. SPN (No Expropiativo)\n";
            cout << "6. Retroalimentacion (Expropiativo)\n";
            cin >> opcion;

            switch (opcion) {
                case 1:
                    FCFS(procesos, n);
                    break;
                case 2:
                    SPF(procesos, n);
                    break;
                case 3:
                    HRRN(procesos, n);
                    break;
                case 4:
                    RoundRobin(procesos, n);
                    break;
                case 5:
                    SPN(procesos, n);
                    break;
                case 6:
                    Retroalimentacion(procesos, n);
                    break;
                default:
                    cout << "Opcion no valida.\n";
                    continue;
            }

            mostrarProcesos(procesos, n);

            int decision;
            cout << "\n¿Qué desea hacer?\n";
            cout << "1. Finalizar el programa\n";
            cout << "2. Ingresar una nueva lista de procesos\n";
            cout << "3. Elegir otro algoritmo con los procesos actuales\n";
            cin >> decision;

            if (decision == 1) {
                continuar = false;
                elegirOtroAlgoritmo = false;
            } else if (decision == 2) {
                elegirOtroAlgoritmo = false;
            } else if (decision == 3) {
                // Continuar en el bucle para elegir otro algoritmo
            } else {
                cout << "Opcion no valida. Saliendo del programa.\n";
                continuar = false;
                elegirOtroAlgoritmo = false;
            }
        }
    }

    cout << "Programa finalizado.\n";
    return 0;
}