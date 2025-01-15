//Alvaro Gomez Moran
#include <stdio.h>
#include <stdlib.h>
#include <err.h>

int main(int argc, char* argv[]) {
	FILE* file=NULL;
	int c,ret;

	if (argc!=2) {
		fprintf(stderr,"Usage: %s <file_name>\n",argv[0]);
		exit(1);
	}

	/* Open file */
	if ((file = fopen(argv[1], "r")) == NULL)
		err(2,"The input file %s could not be opened",argv[1]);

	char buffer[128];
	int n;

	while ((n = fread(buffer, sizeof(char), 128, file)) != 0) //n es el numero de elementos leidos. Si n es 0 es que ya no hay elementos que leer
	{
		fwrite(buffer, sizeof(char), n, stdout);//leemos n elementos del buffer de tamaño  sizeof(char)(por que son char) y lo sacamos por consola
	}
	printf("\n");
	
	/* Usamos el while de arriba para hacer esto
	 //Read file byte by byte 
	while ((c = getc(file)) != EOF) {
		 //Print byte to stdout 
		ret=putc((unsigned char) c, stdout);

		if (ret==EOF){
			fclose(file);
			err(3,"putc() failed!!");
		}
	}
	*/

	fclose(file);
	return 0;
}
