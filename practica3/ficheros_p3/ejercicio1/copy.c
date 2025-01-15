//Alvaro Gomez Moran

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


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


// 1 fichero a copiar y 2 nombre que queremos darle al fichero copia destino
int main(int argc, char *argv[])
{

	int fdo = open(argv[1], O_RDONLY);
	if(fdo == -1){
		perror("Error al abrir el archivo");
		return EXIT_FAILURE;
	}

	int fdd = creat(argv[2], O_WRONLY | O_CREAT | O_TRUNC);
	if(fdd == -1){
		perror("Error al crear el archivo destino");
		return EXIT_FAILURE;
	}

	copy(fdo, fdd);

	close(fdo);
	close(fdd);

	return 0;
}
