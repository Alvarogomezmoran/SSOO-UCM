#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#define MAX_THREADS 10

struct hilo {
    int numeroHilo;
    char prioridad; // P (prioritario) o N (no prioritario)
};

void *thread_usuario(void *arg) {
    struct hilo *hilo_info = (struct hilo *)arg; // Convertir el argumento a la estructura
    long tid = (long) pthread_self(); // Obtener el identificador del hilo

    // Imprimir la información del hilo
    printf("Thread %ld: Número de hilo = %d, Prioridad = %c\n", tid, hilo_info->numeroHilo, hilo_info->prioridad);

    // Liberar la memoria dinámica
    free(hilo_info);

    pthread_exit(0);
}

int main(int argc, char *argv[]) {
    pthread_t threads[MAX_THREADS];
    pthread_attr_t attr;

    // Inicializar atributos de los hilos
    pthread_attr_init(&attr);

    for (int i = 0; i < MAX_THREADS; i++) {
        // Crear una nueva estructura para cada hilo
        struct hilo *hilo_info = malloc(sizeof(struct hilo));
        if (hilo_info == NULL) {
            perror("No se pudo asignar memoria");
            exit(EXIT_FAILURE);
        }

        // Inicializar la estructura
        hilo_info->numeroHilo = i;
        hilo_info->prioridad = (i % 2 == 0) ? 'P' : 'N'; // Prioridad 'P' para pares, 'N' para impares

        // Crear el hilo
        if (pthread_create(&threads[i], &attr, thread_usuario, hilo_info) != 0) {
            perror("Error al crear el hilo");
            free(hilo_info); // Liberar memoria si hay un error
            exit(EXIT_FAILURE);
        }
    }

    // Esperar a que todos los hilos terminen
    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Destruir los atributos de los hilos
    pthread_attr_destroy(&attr);

    return 0;
}
