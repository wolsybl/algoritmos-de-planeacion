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
    int colaPrioridad[100]; // Arreglo para almacenar las prioridades
    int numPrioridades; // Numero de prioridades almacenadas
    int colaProcesos[100]; // Contador de procesos para validar si se baja la prioridad
};
// Funcion para reiniciar los valores de los procesos
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

void generarDiagramaGantt(Proceso procesos[], int n, int tiempoTotal) {
    // Crear una matriz para almacenar el diagrama de Gantt
    char gantt[100][100]; // Asumimos un máximo de 100 procesos y 100 unidades de tiempo

    // Inicializar la matriz con espacios en blanco
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < tiempoTotal; j++) {
            gantt[i][j] = ' ';
        }
    }

    // Llenar la matriz con los tiempos de ejecucion de cada proceso
    for (int i = 0; i < n; i++) {
        for (int j = procesos[i].inicio; j < procesos[i].tiempoFinalizacion; j++) {
            gantt[i][j] = '#';
        }
    }

    // Mostrar la linea de tiempo
    cout << "Tiempo:  ";
    for (int j = 0; j < tiempoTotal+1; j++) {
        cout << j << "  ";
    }
    cout << endl;

    // Mostrar el diagrama de Gantt para cada proceso
    for (int i = 0; i < n; i++) {
        cout << "Proceso " << procesos[i].id << ": ";
        for (int j = 0; j < tiempoTotal; j++) {
            cout << "|" << gantt[i][j] << "|";
        }
        cout << endl;
    }
}

// Funcion para calcular el tiempo de espera y finalizacion en FCFS
void FCFS(Proceso procesos[], int n) {
    int tiempoActual = 0;

    // Calcular tiempos de inicio, finalizacion, espera y retorno
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

    // Generar el diagrama de Gantt
    generarDiagramaGantt(procesos, n, tiempoActual);
}

// Funcion para calcular el tiempo de espera y finalizacion en SPF (No expropiativo)
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
        int menorRafaga = 999999; // Valor grande para encontrar el minimo
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
    // Generar el diagrama de Gantt
    generarDiagramaGantt(procesos, n, tiempoActual);
}

// Funcion para calcular el tiempo de espera y finalizacion en HRRN
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
    // Generar el diagrama de Gantt
    generarDiagramaGantt(procesos, n, tiempoActual);
}

// Funcion para calcular el tiempo de espera y finalizacion en Round Robin
void RoundRobin(Proceso procesos[], int n) {
    int tiempoActual = 0;
    int completados = 0;
    int tiemposRafagaRestantes[n]; // Tiempo de rafaga restante para cada proceso
    bool procesoIniciado[n];      // Indica si un proceso ya ha comenzado a ejecutarse
    
    // Matriz para almacenar el diagrama de Gantt
    char gantt[100][100]; // Asumimos un maximo de 100 procesos y 100 unidades de tiempo

    // Inicializar la matriz con espacios en blanco
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < 100; j++) {
            gantt[i][j] = ' ';
        }
    }

    // Inicializar los tiempos de rafaga restantes y el estado de inicio de los procesos
    for (int i = 0; i < n; i++) {
        tiemposRafagaRestantes[i] = procesos[i].tiempoRafaga;
        procesoIniciado[i] = false; // Ningun proceso ha comenzado
    }

    // Bucle principal: ejecutar hasta que todos los procesos hayan terminado
    while (completados < n) {
        bool procesoEjecutado = false;

        // Iterar sobre todos los procesos
        for (int i = 0; i < n; i++) {
            // Verificar si el proceso esta listo para ejecutarse
            if (procesos[i].tiempoLlegada <= tiempoActual && tiemposRafagaRestantes[i] > 0) {
                procesoEjecutado = true;

                // Registrar el inicio si es la primera vez que el proceso se ejecuta
                if (!procesoIniciado[i]) {
                	procesos[i].inicio = tiempoActual + 1;
                    procesoIniciado[i] = true;
                }

                // Ejecutar el proceso por un quantum de 1 unidad de tiempo
                tiemposRafagaRestantes[i]--;
                gantt[i][tiempoActual] = '#'; // Marcar el tiempo actual en el diagrama de Gantt
                tiempoActual++;

                // Si el proceso ha terminado, calcular sus tiempos
                if (tiemposRafagaRestantes[i] == 0) {
                    completados++;
                    procesos[i].tiempoFinalizacion = tiempoActual;
                    procesos[i].tiempoRetorno = procesos[i].tiempoFinalizacion - procesos[i].tiempoLlegada;
                    procesos[i].tiempoEspera = procesos[i].tiempoFinalizacion - procesos[i].tiempoLlegada - procesos[i].tiempoRafaga;
                }
            }
        }

        // Si no se ejecuto ningun proceso en este ciclo, avanzar el tiempo
        if (!procesoEjecutado) {
            tiempoActual++;
        }
    }
    // Mostrar la linea de tiempo
    cout << "Tiempo:  ";
    for (int j = 0; j < tiempoActual + 1; j++) {
        cout << j << "  ";
    }
    cout << endl;

    // Mostrar el diagrama de Gantt para cada proceso
    for (int i = 0; i < n; i++) {
        cout << "Proceso " << procesos[i].id << ": ";
        for (int j = 0; j < tiempoActual; j++) {
            cout << "|" << gantt[i][j] << "|";
        }
        cout << endl;
    }
}

// Funcion para calcular el tiempo de espera y finalizacion en SPN (Shortest Process Next)
void SPN(Proceso procesos[], int n) {
    int tiempoActual = 0;
    int completados = 0;

    // Matriz para almacenar el diagrama de Gantt
    char gantt[100][100]; // Asumimos un máximo de 100 procesos y 100 unidades de tiempo

    // Inicializar la matriz con espacios en blanco
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < 100; j++) {
            gantt[i][j] = ' ';
        }
    }

    while (completados < n) {
        int idx = -1;
        int menorRafaga = 999999;

        // Buscar el proceso con la menor rafaga que esta listo para ejecutarse
        for (int i = 0; i < n; i++) {
            if (procesos[i].tiempoLlegada <= tiempoActual && procesos[i].tiempoRafagaRestante > 0) {
                if (procesos[i].tiempoRafagaRestante < menorRafaga) {
                    menorRafaga = procesos[i].tiempoRafagaRestante;
                    idx = i;
                }
            }
        }

        if (idx != -1) {
            // Registrar el tiempo de inicio si es la primera vez que el proceso se ejecuta
            if (procesos[idx].inicio == 0) {
                procesos[idx].inicio = tiempoActual;
            }

            // Ejecutar el proceso seleccionado por 1 unidad de tiempo
            procesos[idx].tiempoRafagaRestante--;
            gantt[idx][tiempoActual] = '#'; // Marcar el tiempo actual en el diagrama de Gantt
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

    // Mostrar la linea de tiempo
    cout << "Tiempo:  ";
    for (int j = 0; j < tiempoActual + 1; j++) {
        cout << j << "  ";
    }
    cout << endl;

    // Mostrar el diagrama de Gantt para cada proceso
    for (int i = 0; i < n; i++) {
        cout << "Proceso " << procesos[i].id << ": ";
        for (int j = 0; j < tiempoActual; j++) {
            cout << "|" << gantt[i][j] << "|";
        }
        cout << endl;
    }
}


// Funcion para calcular el tiempo de espera y finalizacion en Retroalimentacion (Multilevel Feedback Queue)
void Realimentacion(Proceso procesos[], int n) {
    int tiempoActual = 0;
    int completados = 0;
    int tiemposRafagaRestantes[n]; // Tiempo de ráfaga restante para cada proceso
    bool procesoIniciado[n];      // Indica si un proceso ya ha comenzado a ejecutarse
    int contadorInicios = 0;      // Contador de inicios de procesos

    // Inicializar los tiempos de ráfaga restantes y el estado de inicio de los procesos
    for (int i = 0; i < n; i++) {
        tiemposRafagaRestantes[i] = procesos[i].tiempoRafaga;
        procesoIniciado[i] = false; // Ningun proceso ha comenzado
        procesos[i].numPrioridades = 0; // Inicializar el contador de prioridades
    }

    // Crear una matriz para el diagrama de Gantt
    const int maxTiempo = 100; // Máximo tiempo asumido
    char gantt[n][maxTiempo];  // Matriz para el diagrama de Gantt

    // Inicializar la matriz con espacios en blanco
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < maxTiempo; j++) {
            gantt[i][j] = ' ';
        }
    }

    // Bucle principal: ejecutar hasta que todos los procesos hayan terminado
    while (completados < n) {
        bool procesoEjecutado = false;

        // Incrementar el contador de inicios si un proceso alcanza su tiempo de inicio
        for (int i = 0; i < n; i++) {
            if (procesos[i].tiempoLlegada == tiempoActual && !procesoIniciado[i]) {
                contadorInicios++;
                procesoIniciado[i] = true;
                procesos[i].inicio = tiempoActual + 1;
                cout << "Contador de inicios aumento a: " << contadorInicios << endl;
            }
        }

        // Buscar el proceso listo con el menor numero de prioridades
        int idx = -1; // indice del proceso seleccionado
        int minPrioridades = 999999; // Minimo numero de prioridades

        for (int i = 0; i < n; i++) {
            if (procesos[i].tiempoLlegada <= tiempoActual && tiemposRafagaRestantes[i] > 0) {
                if (procesos[i].numPrioridades < minPrioridades) {
                    minPrioridades = procesos[i].numPrioridades;
                    idx = i;
                }
            }
        }

        // Si se encontro un proceso listo, ejecutarlo
        if (idx != -1) {
            procesoEjecutado = true;

            // Verificar cuantos procesos estan listos para ejecutarse
            int procesosListos = 0;
            for (int i = 0; i < n; i++) {
                if (procesos[i].tiempoLlegada < tiempoActual && tiemposRafagaRestantes[i] > 0) {
                    procesosListos++;
                }
            }

            // Incrementar el numero de prioridades solo si hay mas de un proceso listo
            if (procesosListos < contadorInicios) {
                procesos[idx].numPrioridades++;
                cout << "Proceso " << procesos[idx].id << " tiene ahora " << procesos[idx].numPrioridades << " prioridades." << endl;
            }
            
            
            // Ejecutar el proceso por un quantum de 1 unidad de tiempo
            tiemposRafagaRestantes[idx]--;
            gantt[idx][tiempoActual] = '#'; // Marcar en el diagrama de Gantt
            tiempoActual++;

            // Si el proceso ha terminado, calcular sus tiempos
            if (tiemposRafagaRestantes[idx] == 0) {
                completados++;
                procesos[idx].tiempoFinalizacion = tiempoActual;
                procesos[idx].tiempoRetorno = procesos[idx].tiempoFinalizacion - procesos[idx].tiempoLlegada;
                procesos[idx].tiempoEspera = procesos[idx].tiempoFinalizacion - procesos[idx].tiempoLlegada - procesos[idx].tiempoRafaga;
            }
        }

        // Si no se ejecuto ningun proceso en este ciclo, avanzar el tiempo
        if (!procesoEjecutado) {
            tiempoActual++;
        }
    }

    // Imprimir el diagrama de Gantt
    cout << "Diagrama de Gantt (Realimentacion):" << endl;
    cout << "Tiempo:  ";
    for (int j = 0; j < tiempoActual; j++) {
        cout << j << "  ";
    }
    cout << endl;

    for (int i = 0; i < n; i++) {
        cout << "Proceso " << procesos[i].id << ": ";
        for (int j = 0; j < tiempoActual; j++) {
            cout << "|" << gantt[i][j] << "|";
        }
        cout << endl;
    }

    // Imprimir la cola de prioridad de cada proceso
    cout << "\nCola de prioridad de cada proceso:" << endl;
    for (int i = 0; i < n; i++) {
        cout << "Proceso " << procesos[i].id << ": ";
        for (int j = 0; j < procesos[i].numPrioridades; j++) {
            cout << procesos[i].colaPrioridad[j] << " ";
        }
        cout << endl;
    }
}

// Funcion para mostrar los resultados
void mostrarProcesos(Proceso procesos[], int n) {
    cout << "Proceso\tInicio\tTiempo de espera\tFinalizacion\tTiempo de retorno\tTiempo de llegada" << endl;
    for (int i = 0; i < n; i++) {
        cout << procesos[i].id << "\t" << procesos[i].inicio << "\t\t" << procesos[i].tiempoEspera << "\t\t" 
             << procesos[i].tiempoFinalizacion << "\t\t" << procesos[i].tiempoRetorno << "\t\t" << procesos[i].tiempoLlegada << endl;
    }
}

int main() {
    int n;
    Proceso procesos[100]; // Maximo 100 procesos
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
            cout << "6. Realimentacion (Expropiativo)\n";
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
                    Realimentacion(procesos, n);
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