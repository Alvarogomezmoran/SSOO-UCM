//Alvaro Gomez Moran

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/sysmacros.h>

#define BUFFER_SIZE 512

void copy(int fdo, int fdd){
	int num;
	char Buffer[BUFFER_SIZE];
	//Usamos un buffer PASOS->
		//Leemos del fichero en cadenas de 512 y lo guardamos en el buffer
		//Del buffer lo escribimos en el fichero destino
	while((num = read(fdo, Buffer, BUFFER_SIZE)) > 0){
		if(write(fdd, Buffer, num) != num){
			perror("Error al escribir en el fichero destino");
			return;
		}
	}
}

void copy_regular(char *orig, char *dest){
	int fdo = open(orig, O_RDONLY);
	if(fdo == -1){
		perror("Error al abrir el archivo");
		return;
	}

	int fdd = creat(dest, O_WRONLY | O_CREAT | O_TRUNC);
	if(fdd == -1){
		perror("Error al crear el archivo destino");
		return;
	}

	copy(fdo, fdd);

	close(fdo);
	close(fdd);
}

/*
Reservar memoria para hacer una copia de la ruta apuntada. 
Una llamada a lstat sobre el fichero origen nos permitirá conocer el número de bytes que ocupa el enlace simbólico, 
que se corresponde con el tamaño de esta ruta sin el carácter null (‘\0’) de final de cadena (consultar la página de manual de lstat). 
Por tanto sumaremos uno al tamaño obtenido de lstat.

Copiar en este buffer la ruta del fichero apuntado haciendo uso de la llamada al sistema readlink. 
Deberemos añadir manualmente el caracter null de final de cadena.

Usar la llamada al sistema symlink para crear un nuevo enlace simbólico que apunte a esta ruta.
*/
void copy_link(char *orig, char *dest){
	struct stat sb;
	if (lstat(orig, &sb) == -1) {
		perror("Error al llamar a lstat");
		return;
	}

	off_t tamPath = sb.st_size + 1;
	char Buffer[tamPath];

	if(readlink(orig, Buffer, tamPath) == -1){
		perror("Error al leer la ruta del enlace simbolico");
		return;
	}
	Buffer[tamPath - 1] = '\0';

	if(symlink(Buffer, dest) == -1){
		perror("Error al ejecutar symlink");
		return;
	}

}

int main(int argc, char *argv[]){

	struct stat sb;
	if (lstat(argv[1], &sb) == -1) {
		perror("Error al llamar a lstat");
		exit(EXIT_FAILURE);
	}

	if(S_ISLNK(sb.st_mode)){//Es un enlace simbolico

		copy_link(argv[1], argv[2]);

	}else if(S_ISREG(sb.st_mode)){//Es un archivo regular

		copy_regular(argv[1], argv[2]);

	}else{
		perror("Tipo de fichero no soportado por nuestro programa");
		return EXIT_FAILURE;
	}

	return 0;
}
