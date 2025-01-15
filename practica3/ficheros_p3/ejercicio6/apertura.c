//Alvaro Gomez Moran

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>

void print_usage() {
    printf("Uso: apertura -f nombre_fichero [-r] [-w] [-rw]\n");
}

int main(int argc, char *argv[]) {
    char *filename = NULL;
    int open_flags = 0;
    int mode = 0644;  // permisos por defecto 0644 -rw-r--r-- 0444 -r--r--r--

    int opt;
    while ((opt = getopt(argc, argv, "f:rw")) != -1) {
        switch (opt) {
            case 'f':
                filename = optarg;
                break;
            case 'r':
                open_flags |= O_RDONLY;  // Modo lectura solo
                break;
            case 'w':
                open_flags |= O_WRONLY;  // Modo escritura solo
                break;
            default:
                print_usage();
                return EXIT_FAILURE;
        }
    }

    if (filename == NULL) {
        print_usage();
        return EXIT_FAILURE;
    }

    // Si no se ha especificado un modo, por defecto abrimos en lectura/escritura
    if (open_flags == 0) {
        open_flags = O_RDWR; //Si no hay ningun flag se pone a lectura/escritura
    }

    // Crear o abrir el archivo con los permisos seleccionados
    int fd = open(filename, open_flags | O_CREAT | O_TRUNC, mode);
    if (fd == -1) {
        perror("Error al abrir el archivo");
        return EXIT_FAILURE;
    }

    // Escribir en el archivo
    const char *message = "Hola, mundo!\n";
    if (write(fd, message, strlen(message)) == -1) {
        perror("Error al escribir en el archivo");
        close(fd);
        return EXIT_FAILURE;
    }

    // Volver al principio del archivo para leer
    lseek(fd, 0, SEEK_SET);

    // Leer del archivo
    char buffer[100];
    ssize_t bytesRead = read(fd, buffer, sizeof(buffer) - 1);
    if (bytesRead == -1) {
        perror("Error al leer del archivo");
        close(fd);
        return EXIT_FAILURE;
    }

    printf("%s", buffer);

    // Cerrar el archivo
    close(fd);
    return EXIT_SUCCESS;
}
