//Alvaro Gomez Moran
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

int main(int argc, char* argv[])
{

	//abrimos el archivo
	FILE* file = fopen(argv[1], "wb");
	if(file == NULL){
		perror("Error al abrir el archivo");
	}


	//recorremos los argumentos que vamos a escribir en el archivo abierto
	for (int i = 2; i < argc; i++)
	{
		if(fwrite(argv[i], sizeof(char), strlen(argv[i]), file) != strlen(argv[i])){
			perror("Error al escribir en el fichero");
			fclose(file);
			return 1;
		} 

		//añadimos el caracter \0 detras de cada palabra
		if(i != argc - 1){
			char null_char = '\0';
			if(fwrite(&null_char, sizeof(char), 1, file) != 1){
				perror("Error al escribir el valor nulo");
				fclose(file);
				return 1;
			}
		}
		

	}
	
	fclose(file);
	printf("Cadenas escritas correctamente en el fichero. \n");


	/* To be completed */
	return 0;
}
