//Alvaro Gomez Moran
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

/** Loads a string from a file.
 *
 * file: pointer to the FILE descriptor
 *
 * The loadstr() function must allocate memory from the heap to store
 * the contents of the string read from the FILE.
 * Once the string has been properly built in memory, the function returns
 * the starting address of the string (pointer returned by malloc())
 *
 * Returns: !=NULL if success, NULL if error
 */
char *loadstr(FILE *file)
{
	//vemos donde esta el puntero
	long current_pos = ftell(file);
	
	if(current_pos == -1){
		perror("Error al recuperar la posicion actual en el fichero de lectura");
		return NULL;
	}

	//vemos lo que ocupa lo que vamos a leer hasta el \0 excepto en la ultima palabra que es hasta EOF ya que no tiene \0. 
	char c;
	int length = 0;
	while (fread(&c, 1, 1, file) == 1 && c != '\0') //leemos caracter a caracter
	{
		length++;
	}

	//retrocedemos el puntero ya que al hacer fread lo hemos avanzado
	if(fseek(file, current_pos,SEEK_SET) != 0){
		perror("Error al restaurar la posicion");
		return NULL;
	}

	//reservamos memoria para ello
	char *str = (char*)malloc(length);
	if(str == NULL){
		perror("Error al reservar memoria");
		return NULL;
	}


	//leemos todo sabiendo lo que mide
	if(fread(str, sizeof(char), length,file) != (length)){
		perror("Error al leer el archivo");
		free(str);
		return NULL;
	}

	fread(&c, sizeof(char), 1, file);//AVANZAMOS PARA SALTAR EL CARACTER '\0'. EN LA ULTIMA PALABRA SERA EOF Y NO LEERA NADA (RETURN -1)

	return str;

	/* To be completed */
	
}

int main(int argc, char *argv[])
{

	//abrimos el archivo binario pasado por parametro
	FILE* file = fopen(argv[1], "rb"); //Modo binario
	if(file == NULL){
		perror("Error al abrir el archivo");
		return EXIT_FAILURE;
	}

	//bucle hasta EOF llamamos a loadstr e imprimimos lo que devuelve si no es nulo y liberamos memoria de lo devuelto
	while (!feof(file))//feof verifica si hemos llegado al final del archivo
	{
		char* str = loadstr(file);

		if(str != NULL){
			printf("%s\n", str);
			free(str);
		}
	}
	
	fclose(file);

	/* To be completed */
	return EXIT_SUCCESS;
}
