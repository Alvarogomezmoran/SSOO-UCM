//Alvaro Gomez Moran
//Opcion B
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/file.h>
#include <errno.h>

int main(void)
{
    int fd, i;
    char buffer[6]; // Para almacenar la cadena de 5 dígitos y el carácter nulo

    // Abrir el archivo con permisos de escritura y truncar su contenido si existe
    fd = open("output.txt", O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("Error al abrir el archivo para escritura");
        exit(EXIT_FAILURE);
    }

    // Escribir los primeros cinco ceros
    if (write(fd, "00000", 5) != 5) {
        perror("Error al escribir en el archivo");
        close(fd);
        exit(EXIT_FAILURE);
    }

    for (i = 1; i < 10; i++) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("Error al crear proceso hijo");
            close(fd);
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {  // Proceso hijo
            sprintf(buffer, "%d", i * 11111); // Crear cadena de 5 dígitos iguales

            // Bloquear el archivo para escribir
            if (flock(fd, LOCK_EX) == -1) {
                perror("Error al bloquear el archivo");
                close(fd);
                exit(EXIT_FAILURE);
            }

            /*
            // Mover el puntero del archivo a la posición correcta para el hijo
            off_t offset = (i - 1) * 10 + 5; // Calculamos la posicion 
            
            //Para el caso de i = 2
            //00000|11111|00000|(Aqui nos posicionamos) para escribir los 5 doses -> 22222 para i = 2 -> (2 - 1) * 10 + 5 = 15
            
            if (lseek(fd, offset, SEEK_SET) == (off_t) -1) {
                perror("Error en lseek");
                flock(fd, LOCK_UN);
                close(fd);
                exit(EXIT_FAILURE);
            }
            */
            // Escribir la cadena del hijo
            if (write(fd, buffer, 5) != 5) {
                perror("Error al escribir en el archivo");
                flock(fd, LOCK_UN);
                close(fd);
                exit(EXIT_FAILURE);
            }

            // Desbloquear el archivo
            if (flock(fd, LOCK_UN) == -1) {
                perror("Error al desbloquear el archivo");
                close(fd);
                exit(EXIT_FAILURE);
            }

            // Cerrar el archivo y terminar el proceso hijo
            close(fd);
            exit(0);
        } else {
            //esperar a que el hijo termine
            wait(NULL);
            if(i < 9){
                // Bloquear el archivo para escribir
                if (flock(fd, LOCK_EX) == -1) {
                    perror("Error al bloquear el archivo (padre)");
                    close(fd);
                    exit(EXIT_FAILURE);
                }

                /*
                // Mover el puntero del archivo a la posición adecuada para el padre
                off_t offset = i * 10; // El padre escribe después del hijo 
                
                //Para el caso de i = 2 -> 00000|11111|00000|22222(Aqui nos posicionamos) para escribir los 5 ceros -> 00000
                
                if (lseek(fd, offset, SEEK_SET) == (off_t) -1) {
                    perror("Error en lseek (padre)");
                    flock(fd, LOCK_UN);
                    close(fd);
                    exit(EXIT_FAILURE);
                }
                */
                // Escribir los cinco ceros del padre
                if (write(fd, "00000", 5) != 5) {
                    perror("Error al escribir en el archivo (padre)");
                    flock(fd, LOCK_UN);
                    close(fd);
                    exit(EXIT_FAILURE);
                }

                // Desbloquear el archivo
                if (flock(fd, LOCK_UN) == -1) {
                    perror("Error al desbloquear el archivo (padre)");
                    close(fd);
                    exit(EXIT_FAILURE);
                }
            }
        }
    }

    // Esperar a que todos los hijos terminen
    //while (wait(NULL) != -1);

    // Cerrar el archivo
    close(fd);

    // Reabrir el archivo en modo lectura para mostrar su contenido
    fd = open("output.txt", O_RDONLY);
    if (fd == -1) {
        perror("Error abriendo el archivo para lectura");
        exit(EXIT_FAILURE);
    }

    char c;
    printf("File contents are:\n");
    while (read(fd, &c, 1) > 0) {
        printf("%c", c);
    }
    printf("\n");

    close(fd);
    return 0;
}
