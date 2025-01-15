//Alvaro Gomez Moran

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>

int get_size_dir(char *dname, size_t *blocks);

/* Obtiene en el buffer blocks el tamaño del archivo fname usando lstat. 
 * Si fname es un directorio, se llama a get_size_dir para añadir el tamaño 
 * de su contenido.
 */
int get_size(char *fname, size_t *blocks) {
    struct stat statbuf;
    
    if (lstat(fname, &statbuf) == -1) {
        perror("lstat");
        return -1;
    }

    // El número de bloques es dado por st_blocks
    *blocks += statbuf.st_blocks;

    if (S_ISDIR(statbuf.st_mode)) {
        // Si es un directorio, calcula su tamaño recursivamente
        return get_size_dir(fname, blocks);
    }

    return 0;
}

/* Obtiene el número total de bloques ocupados por todos los archivos en un 
 * directorio. Si un archivo contenido es un directorio, se realiza una 
 * llamada recursiva a get_size_dir. Las entradas . y .. son convenientemente 
 * ignoradas.
 */
int get_size_dir(char *dname, size_t *blocks) {
    DIR *dir;
    struct dirent *entry;
    char path[PATH_MAX];

    if ((dir = opendir(dname)) == NULL) {
        perror("opendir");
        return -1;
    }

    while ((entry = readdir(dir)) != NULL) {
        // Saltar las entradas "." y ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Construir la ruta completa para la entrada
        snprintf(path, sizeof(path), "%s/%s", dname, entry->d_name);
        get_size(path, blocks);
    }

    closedir(dir);
    return 0;
}

/* Procesa todos los archivos en la línea de comandos llamando a get_size 
 * en ellos para obtener el número de bloques de 512 B que ocupan y 
 * imprime el tamaño total en kilobytes en la salida estándar.
 */
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <archivo1> <archivo2> ...\n", argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        size_t blocks = 0;
        if (get_size(argv[i], &blocks) == -1) {
            fprintf(stderr, "Error procesando el archivo: %s\n", argv[i]);
            continue;
        }
        printf("%zu KB\t%s\n", blocks / 2, argv[i]); // Convertir bloques a KB (1 bloque = 512 bytes)
    }

    return 0;
}
