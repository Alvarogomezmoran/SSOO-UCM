//Alvaro Gomez Moran
#include <stdio.h>
#include <unistd.h> /* for getopt() */
#include <stdlib.h> /* for EXIT_SUCCESS, EXIT_FAILURE */
#include <string.h>
#include <errno.h>
#include "defs.h"


/* Assume lines in the text file are no larger than 100 chars */
#define MAXLEN_LINE_FILE 100
#define MAX_NAME_LENGTH 32

//parte opcional 1
student_t* read_student_text_file(FILE* students, int* nr_entries) {
    char line[MAXLEN_LINE_FILE];

    // Numero de entradas
    if (fgets(line, MAXLEN_LINE_FILE, students) != NULL) {
        if (sscanf(line, "%d", nr_entries) != 1) {
            perror("Error al leer el número de casos\n");
            fclose(students);
            return NULL;
        }
    } else {
        perror("Error al leer la primera línea\n");
        fclose(students);
        return NULL;
    }

    // Reservamos memoria para los registros
    student_t* stu = (student_t*)malloc((*nr_entries) * sizeof(student_t));
    if (stu == NULL) {
        perror("Error al asignar memoria para el array de estudiantes");
        fclose(students);
        return NULL;
    }

    // leemos cada registro del archivo
    for (int i = 0; i < *nr_entries; i++) {
        if (fgets(line, MAXLEN_LINE_FILE, students) == NULL) {
            perror("Error al leer la línea");
            fclose(students);
            free(stu); // liberar memoria en caso de error
            return NULL;
        }

        // reservamos memoria para el nombres y el apellido
        stu[i].first_name = (char*)malloc(MAX_NAME_LENGTH * sizeof(char));
        stu[i].last_name = (char*)malloc(MAX_NAME_LENGTH * sizeof(char));
        if (stu[i].first_name == NULL || stu[i].last_name == NULL) {
            perror("Error al asignar memoria para los nombres");
            fclose(students);
            free(stu); 
            return NULL;
        }

        // sacamos cada campo de line
        if (sscanf(line, "%d:%[^:]:%[^:]:%s", &stu[i].student_id, stu[i].NIF, stu[i].first_name, stu[i].last_name) != 4) {
            perror("Error al procesar los datos");
            fclose(students);
            free(stu[i].first_name);
            free(stu[i].last_name);
            free(stu); 
            return NULL;
        }
    }

    return stu;
}

//Con la opcional 1
int print_text_file(char *path) {
    
    FILE* file = fopen(path, "r");
    if (file == NULL) {
        perror("Error al abrir el archivo");
        return EXIT_FAILURE;
    }

    // leemos los registros del archivo
    int num_entries;
    student_t* students = read_student_text_file(file, &num_entries);
    if (students == NULL) {
        fclose(file);
        return EXIT_FAILURE;
    }

    // imprimimos los registros
    for (int i = 0; i < num_entries; i++) {
        printf("[Entry #%d]\n", i);
        printf("        student_id=%d\n", students[i].student_id);
        printf("        NIF=%s\n", students[i].NIF);
        printf("        first_name=%s\n", students[i].first_name);
        printf("        last_name=%s\n", students[i].last_name);

        // liberamos la memoria del nombre y apellido
        free(students[i].first_name);
        free(students[i].last_name);
    }

    // liberamos la memoria del array de estudiantes
    free(students);

    // Cerrar el archivo
    fclose(file);

    return 0;
}


/* Sin la parte opcional
int print_text_file(char *path){

	//abrir el fichero
	FILE* file = fopen(path, "r");
	if(file == NULL){
		perror("Error al abrir el fichero\n");
		return EXIT_FAILURE;
	} 
	
	//ver el numero de casos que tiene el fichero
	int num_casos = 0;
	int e_number = 0;
	char line[MAXLEN_LINE_FILE];

	if(fgets(line, MAXLEN_LINE_FILE, file) != NULL){
		if(sscanf(line, "%d", &num_casos) != 1){
			perror("Error al leer el numero de casos\n");
			fclose(file);
			return EXIT_FAILURE;
		}
	}

	//for con el numero de casos del fichero
	for (int i = 0; i < num_casos; i++){

		//leer cada caso que su informacion esta separado por ':'
		if(fgets(line, MAXLEN_LINE_FILE, file) == NULL){
			perror("Error al leer la linea");
			fclose(file);
			return EXIT_FAILURE;
		}

		student_t student;
		student.first_name = (char*) malloc(MAX_NAME_LENGTH * sizeof(char));
    	student.last_name = (char*) malloc(MAX_NAME_LENGTH * sizeof(char));

		if (sscanf(line, "%d:%[^:]:%[^:]:%s", &student.student_id, student.NIF, student.first_name, student.last_name) == 4){
			printf("[Entry #%d]\n", e_number);
            printf("\tstudent_id=%d\n", student.student_id);
            printf("\tNIF=%s\n", student.NIF);
            printf("\tfirst_name=%s\n", student.first_name);
            printf("\tlast_name=%s\n", student.last_name);
			e_number++;
			free(student.first_name);
    		free(student.last_name);
		}else{
			perror("Error al procesar los datos");
			fclose(file);
			free(student.first_name);
    		free(student.last_name);
			return EXIT_FAILURE;
		}
		


	}
	
	/*student_t* stu = read_student_text_file(file, num_casos);

	
	

	if (fclose(file) != 0)
    {
        perror("Error al cerrar el archivo");
        return EXIT_FAILURE;
    }

	// To be completed (part A) 
	return 0;
}
*/

int write_binary_file(char *input_file, char *output_file)
{
	//Abrimos los archivos
	FILE* fileInput = fopen(input_file, "r");
	if(fileInput == NULL){
		perror("Error al abrir el fichero\n");
		return EXIT_FAILURE;
	}
	FILE* fileOutput = fopen(output_file, "wb");
	if(fileOutput == NULL){
		perror("Error al abrir el fichero\n");
		return EXIT_FAILURE;
	}

	//ver el numero de casos que hay en el archivo
	int num_entries = 0;
    char line[MAXLEN_LINE_FILE];

	if(fgets(line, MAXLEN_LINE_FILE, fileInput) != NULL){
		if(sscanf(line, "%d", &num_entries) != 1){
			perror("Error al leer el numero de casos\n");
			fclose(fileInput);
			return EXIT_FAILURE;
		}
	}

	fwrite(&num_entries, sizeof(int), 1, fileOutput);

	//for con el numero de casos del fichero
	for (int i = 0; i < num_entries; i++){

		//leer cada caso que su informacion esta separado por ':'
		if(fgets(line, MAXLEN_LINE_FILE, fileInput) == NULL){
			perror("Error al leer la linea");
			fclose(fileInput);
			return EXIT_FAILURE;
		}

		student_t student;
		student.first_name = (char*) malloc(MAX_NAME_LENGTH * sizeof(char));
    	student.last_name = (char*) malloc(MAX_NAME_LENGTH * sizeof(char));

		if (sscanf(line, "%d:%[^:]:%[^:]:%s", &student.student_id, student.NIF, student.first_name, student.last_name) == 4){

            fwrite(&student.student_id, sizeof(int), 1, fileOutput);
            fwrite(student.NIF, sizeof(char), MAX_CHARS_NIF + 1, fileOutput);
            fwrite(student.first_name, sizeof(char), strlen(student.first_name) + 1, fileOutput);
            fwrite(student.last_name, sizeof(char), strlen(student.last_name) + 1, fileOutput);

			free(student.first_name);
    		free(student.last_name);
		}else{
			perror("Error al procesar los datos");
			fclose(fileInput);
			fclose(fileOutput);
			free(student.first_name);
    		free(student.last_name);
			return EXIT_FAILURE;
		}
		
	}

	fclose(fileInput);
    fclose(fileOutput);

	/* To be completed  (part B) */
	return 0;
}

//Lo usamos ya que no sabemos lo que va a medir el nombre y el apellido
char* loadstr(FILE *file) {
    // Guardar la posición actual en el archivo
    long current_pos = ftell(file);
    if (current_pos == -1) {
        perror("Error al recuperar la posición actual en el fichero de lectura");
        return NULL;
    }

    // Leer hasta encontrar el terminador '\0'
    char c;
    int length = 0;
    while (fread(&c, 1, 1, file) == 1 && c != '\0') {
        length++;
    }

    // Retroceder al inicio de la cadena
    if (fseek(file, current_pos, SEEK_SET) != 0) {
        perror("Error al restaurar la posición");
        return NULL;
    }

    // Reservar memoria para la cadena
    char *str = (char *)malloc(length + 1); // +1 para incluir el terminador '\0'
    if (str == NULL) {
        perror("Error al reservar memoria");
        return NULL;
    }

    // Leer la cadena completa
    if (fread(str, sizeof(char), length + 1, file) != (length + 1)) {
        perror("Error al leer el archivo");
        free(str);
        return NULL;
    }

    return str;
}

//parte 1 opcional
student_t* read_student_binary_file(FILE* students, int* nr_entries) {
    // Leer el número de entradas del archivo binario
    if (fread(nr_entries, sizeof(int), 1, students) != 1) {
        perror("Error al leer el número de registros del archivo binario");
        fclose(students);
        return NULL;
    }

    // Reservar memoria para el número de registros
    student_t* stu = (student_t*)malloc((*nr_entries) * sizeof(student_t));
    if (stu == NULL) {
        perror("Error al asignar memoria para el array de estudiantes");
        fclose(students);
        return NULL;
    }

    // Leer los registros uno por uno
    for (int i = 0; i < *nr_entries; i++) {
        // Leer el student_id
        if (fread(&stu[i].student_id, sizeof(int), 1, students) != 1) {
            perror("Error al leer el ID del estudiante");
            fclose(students);
            free(stu);
            return NULL;
        }

        // Leer el NIF (tamaño fijo conocido)
        if (fread(stu[i].NIF, sizeof(char), MAX_CHARS_NIF + 1, students) != (MAX_CHARS_NIF + 1)) {
            perror("Error al leer el NIF");
            fclose(students);
            free(stu);
            return NULL;
        }

        // Leer el nombre y apellido (longitud desconocida, requiere `loadstr`)
        stu[i].first_name = loadstr(students);
        if (stu[i].first_name == NULL) {
            fclose(students);
            free(stu);
            return NULL;
        }

        stu[i].last_name = loadstr(students);
        if (stu[i].last_name == NULL) {
            free(stu[i].first_name);
            fclose(students);
            free(stu);
            return NULL;
        }
    }

    return stu;
}


// Función para imprimir el contenido del archivo binario (-b) sin parte opcional
/*
int print_binary_file(char *binary_file) {
    // Abrir el archivo binario en modo lectura
    FILE *file = fopen(binary_file, "rb");
    if (file == NULL) {
        perror("Error al abrir el archivo binario");
        return EXIT_FAILURE;
    }

    // Leer el número de estudiantes
    int num_students;
    if (fread(&num_students, sizeof(int), 1, file) != 1) {
        perror("Error al leer el número de estudiantes");
        fclose(file);
        return EXIT_FAILURE;
    }

    // Leer y mostrar cada estudiante
    for (int i = 0; i < num_students; i++) {
        student_t student;

        //NIF Y ESTUDIANTE CONOCEMOS SU 
        if (fread(&student.student_id, sizeof(int), 1, file) != 1) {
            perror("Error al leer el ID del estudiante");
            fclose(file);
            return EXIT_FAILURE;
        }

        
        if (fread(student.NIF, sizeof(char), MAX_CHARS_NIF + 1, file) != (MAX_CHARS_NIF + 1)) {
            perror("Error al leer el NIF");
            fclose(file);
            return EXIT_FAILURE;
        }

        //loadstr para leer el nombre y el apellido(no sabemos su longitud)
        student.first_name = loadstr(file);
        if (student.first_name == NULL) {
            fclose(file);
            return EXIT_FAILURE;
        }

        student.last_name = loadstr(file);
        if (student.last_name == NULL) {
            free(student.first_name);
            fclose(file);
            return EXIT_FAILURE;
        }

        //imprimimos la info
		printf("[Entry #%d]\n", i);
        printf("        student_id=%d\n", student.student_id);
        printf("        NIF=%s\n", student.NIF);
        printf("        first_name=%s\n", student.first_name);
        printf("        last_name=%s\n", student.last_name);
        // Liberamos la memoria
        free(student.first_name);
        free(student.last_name);
    }

    // Cerrar el archivo
    fclose(file);

    return 0;
}
*/

//Parte opcional 1
int print_binary_file(char *binary_file) {
    // Abrir el archivo binario
    FILE* file = fopen(binary_file, "rb");
    if (file == NULL) {
        perror("Error al abrir el archivo binario");
        return EXIT_FAILURE;
    }

    // Leer los registros del archivo binario
    int num_entries;
    student_t* students = read_student_binary_file(file, &num_entries);
    if (students == NULL) {
        fclose(file);
        return EXIT_FAILURE;
    }

    // Imprimir los registros
    for (int i = 0; i < num_entries; i++) {
        printf("[Entry #%d]\n", i);
        printf("        student_id=%d\n", students[i].student_id);
        printf("        NIF=%s\n", students[i].NIF);
        printf("        first_name=%s\n", students[i].first_name);
        printf("        last_name=%s\n", students[i].last_name);

        // Liberar la memoria de los nombres
        free(students[i].first_name);
        free(students[i].last_name);
    }

    // Liberar la memoria del array de estudiantes
    free(students);

    // Cerrar el archivo
    fclose(file);

    return 0;
}


int main(int argc, char *argv[])
{
	int ret_code, opt;
	struct options options;

	/* Initialize default values for options */
	options.input_file = NULL;
	options.output_file = NULL;
	options.action = NONE_ACT;
	ret_code = 0;

	/* Parse command-line options (incomplete code!) */
	while ((opt = getopt(argc, argv, "hi:pbo:")) != -1)
	{
		switch (opt)
		{
		case 'h':
			fprintf(stderr, "Usage: %s [ -h ]\n", argv[0]);
			exit(EXIT_SUCCESS);
		case 'i':
			options.input_file = optarg;
			break;
		case 'p':
            options.action = PRINT_TEXT_ACT;
            break;
		case 'o':
			options.output_file = optarg;
            options.action = WRITE_BINARY_ACT;
            break;
		case 'b':
			options.action = PRINT_BINARY_ACT;
			break;
		/**
		 **  To be completed ...
		 **/

		default:
			exit(EXIT_FAILURE);
		}
	}

	if (options.input_file == NULL)
	{
		fprintf(stderr, "Must specify one record file as an argument of -i\n");
		exit(EXIT_FAILURE);
	}

	switch (options.action)
	{
	case NONE_ACT:
		fprintf(stderr, "Must indicate one of the following options: -p, -o, -b \n");
		ret_code = EXIT_FAILURE;
		break;
	case PRINT_TEXT_ACT:
		/* Part A */
		ret_code = print_text_file(options.input_file);
		break;
	case WRITE_BINARY_ACT:
		/* Part B */
		ret_code = write_binary_file(options.input_file, options.output_file);
		break;
	case PRINT_BINARY_ACT:
		/* Part C */
		ret_code = print_binary_file(options.input_file);
		break;
	default:
		break;
	}
	exit(ret_code);
}
