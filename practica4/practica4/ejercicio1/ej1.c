#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

typedef struct {
    pid_t pid;
    int command_number;
} CommandInfo;

// Función para lanzar un comando en un nuevo proceso
pid_t launch_command(char** argv){

    pid_t pid = fork();
    
    if(pid < 0){
        perror("Error al crear el proceso hijo");
        return EXIT_FAILURE;
    }else if(pid == 0){//proceso hijo
        execvp(argv[0], argv);
        perror("Error al ejecutar execvp");
        return EXIT_FAILURE;
    }

    /* To be completed */
    return pid;
}


/**
    "ls -l /home/user"
    lo pasa a:
    argv[0] = "ls";
    argv[1] = "-l";
    argv[2] = "/home/user";
    argv[3] = NULL;
 */
char **parse_command(const char *cmd, int* argc) {
    // Allocate space for the argv array (initially with space for 10 args)
    size_t argv_size = 10;
    const char *end;
    size_t arg_len; 
    int arg_count = 0;
    const char *start = cmd;
    char **argv = malloc(argv_size * sizeof(char *));

    if (argv == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    while (*start && isspace(*start)) start++; // Skip leading spaces

    while (*start) {
        // Reallocate more space if needed
        if (arg_count >= argv_size - 1) {  // Reserve space for the NULL at the end
            argv_size *= 2;
            argv = realloc(argv, argv_size * sizeof(char *));
            if (argv == NULL) {
                perror("realloc");
                exit(EXIT_FAILURE);
            }
        }

        // Find the start of the next argument
        end = start;
        while (*end && !isspace(*end)) end++;

        // Allocate space and copy the argument
        arg_len = end - start;
        argv[arg_count] = malloc(arg_len + 1);

        if (argv[arg_count] == NULL) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        strncpy(argv[arg_count], start, arg_len);
        argv[arg_count][arg_len] = '\0';  // Null-terminate the argument
        arg_count++;

        // Move to the next argument, skipping spaces
        start = end;
        while (*start && isspace(*start)) start++;
    }

    argv[arg_count] = NULL; // Null-terminate the array

    (*argc)=arg_count; // Return argc

    return argv;
}

/**
 * Función para ejecutar comandos desde un archivo.
 * Entrada:
 * - file_path: Ruta al archivo que contiene los comandos.
 * - b: Si es 1, los comandos se ejecutan en paralelo; si es 0, secuencialmente.
 */
void execute_command_in_file(const char *file_path, int b){

    FILE *file = fopen(file_path, "r");

    if(file == NULL){
        perror("Error al abrir el fichero que contiene los comandos");
        exit(EXIT_FAILURE);
    }

    char line[1024];
    CommandInfo commands[100];
    int command_count = 0;


    while (fgets(line, sizeof(line), file)){
        
        line[strcspn(line, "\n")] = '\0';
        int argc;
        char **argv = parse_command(line, &argc);
        printf("@@ Running command #%d: %s\n", command_count + 1, line);
        pid_t pid = launch_command(argv);

        if(b == 1){//no se espera para ejecutar el siguiente comando
            commands[command_count].pid = pid;
            commands[command_count].command_number = command_count + 1;
            command_count++;
        }else{
            waitpid(pid, NULL, 0);//esperamos a que el hijo finalice para seguir con los demas comandos
        }

        for (int i = 0; i < argc; i++) {
            free(argv[i]);
        }
        free(argv);

    }
    
    fclose(file);

    if(b == 1){
        int status;
        for (int i = 0; i < command_count; i++)
        {
            pid_t pid = waitpid(commands[i].pid, &status, 0);
            if (pid > 0) {
                printf("@@ Command #%d terminated (pid: %d, status: %d)\n", commands[i].command_number, pid, WEXITSTATUS(status));
            }
        }
        
    }
}

int main(int argc, char *argv[]) {
    char *command = NULL;
    char *file_path = NULL;
    int opt;
    int b = 0;

    while ((opt = getopt(argc, argv, "x:s:b")) != -1)
	{
		switch (opt)
		{
		case 'x'://ejecuta el comando indicado. ./run_commands -x ls
			command = optarg;
			break;
        case 's'://ejecuta todos los comandos en un fichero de 1 en 1. 
			file_path = optarg;
			break;
        case 'b'://ejecuta todos los comandos del fichero (opcion -s) sin esperar a que acabe el anterior 
			b = 1;
			break;
		default:
			exit(EXIT_FAILURE);
		}
	}


    if(command){

        int cmd_argc;
        char **cmd_argv = parse_command(command, &cmd_argc);
        pid_t pid = launch_command(cmd_argv);

        waitpid(pid, NULL, 0);//esperamos a que el hijo finalice

        //liberar memoria
        for (int i = 0; i < cmd_argc; i++) {
            free(cmd_argv[i]);
        }
        free(cmd_argv);

    }else if(file_path){

        execute_command_in_file(file_path, b);
    
    }


    /**  
    //Print parsed arguments
    printf("argc: %d\n", cmd_argc);
    for (i = 0; cmd_argv[i] != NULL; i++) {
        printf("argv[%d]: %s\n", i, cmd_argv[i]);
        free(cmd_argv[i]);  // Free individual argument
    }

    free(cmd_argv);  // Free the cmd_argv array
    */

    return EXIT_SUCCESS;
}
