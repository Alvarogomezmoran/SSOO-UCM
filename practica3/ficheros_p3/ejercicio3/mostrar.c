//Alvaro Gomez Moran

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFFER_SIZE 128

int main(int argc, char *argv[]){

	int opt, 
		offset, 
		modo = 0;//0 -> se saltaran los primeros N bytes
				 //1 -> se leen los ultimos N bytes

	while ((opt = getopt(argc, argv, "n:e")) != -1)
	{
		switch (opt)
		{
		case 'n':
			offset = atoi(optarg);
			break;
		case 'e': //si aparece, se leerán los últimos N bytes del fichero. Si no aparece, se saltarán los primeros N bytes del fichero.
            modo = 1;
            break;
		default:
			exit(EXIT_FAILURE);
		}
	}

	//abrir el fichero
	int fd = open(argv[optind], O_RDONLY);
	if(fd == -1){
		perror("Error al abrir el archivo");
		return EXIT_FAILURE;
	}
	

	if(modo == 1){//Leemos ultimos N bytes
		lseek(fd, - offset,SEEK_END);
	}else{//saltamos primeros N bytes
		lseek(fd, offset,SEEK_SET);
	}

	char Buffer[BUFFER_SIZE];
	int num;

	while((num = read(fd, Buffer, BUFFER_SIZE)) > 0){
		if(write(1, Buffer, num) != num){ //No puede ser stdout
			perror("Error al escribir por consola");
			return EXIT_FAILURE;
		}
	}

	return 0;
}
