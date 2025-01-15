#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h> 

pid_t pid_chill;

/*programa que temporiza la ejecución de un proceso hijo */
// Manejador de la señal SIGALRM
void handle_sigalrm(int sig) {
    if (pid_chill > 0) {
        printf("Tiempo agotado, enviando SIGKILL al proceso hijo...\n");
        kill(pid_chill, SIGKILL);  // Mata al proceso hijo
    }
}

// Ignorar SIGINT
void ignore_sigint(int sig) {
    printf("SIGINT recibido, pero será ignorado.\n");
}

int main(int argc, char **argv){

	if(argc < 2){
		fprintf(stderr, "Uso: %s <comando> [args...]\n", argv[0]);
        exit(EXIT_FAILURE);
	}

	// Establecer el manejador de SIGALRM
    struct sigaction sa;
    sa.sa_handler = handle_sigalrm;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    // Ignorar SIGINT 
    struct sigaction sa_ignore;
    sa_ignore.sa_handler = ignore_sigint;
    sigemptyset(&sa_ignore.sa_mask);
    sa_ignore.sa_flags = 0;
    if (sigaction(SIGINT, &sa_ignore, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
	
	// Crear proceso hijo
	pid_chill = fork();
	if(pid_chill == -1){
		perror("Error al crear el proceso hijo");
		return EXIT_FAILURE;
	}

	// Proceso hijo
	
	if(pid_chill == 0){
		// Reemplazar el hijo con el programa dado en los argumentos
		if(execvp(argv[1], &argv[1]) == -1){
			perror("Error en el execvp");
			return EXIT_FAILURE;
		}

	}else{// Proceso padre
		// Establecer una alarma para 5 segundos
		alarm(5);

		// Esperar al hijo
		int stat;
		pid_t p = waitpid(pid_chill, &stat, 0);

		if (p == -1){
			perror("Error waitpid");
			return EXIT_FAILURE;
		}
		
		// Comprobar cómo terminó el proceso hijo
		if(WIFEXITED(stat)){//returns true if the child terminated normally
			printf("El proceso hijo termino normalmente %d\n", WEXITSTATUS(stat));
		}else if(WIFSIGNALED(stat)){//returns true if the child process was terminated by a signal
			printf("El proceso hijo termino por la señal %d\n", WTERMSIG(stat));
		}

	}
	
        

	return 0;
}

/*

En este ejercicio vamos a experimentar el envío de señales, haciendo que un proceso cree a un hijo, 
espere a una señal de un temporizador y, cuando la reciba, termine con la ejecución del hijo.

El programa principal recibirá como argumento el comando del programa que se desea que ejecute el proceso hijo. 
Si a su vez este comando consta de varios argumentos, estos se pasarán separados por espacios a continuación del nombre del programa a ejecutar.

El proceso padre creará un hijo, que cambiará su ejecutable con una llamada a execvp. 
A continuación, el padre establecerá que el manejador de la señal SIGALRM sea una función que envíe una señal SIGKILL al proceso hijo y 
programará una alarma para que le envíe una señal a los 5 segundos. 
Antes de finalizar, el padre esperará a que finalice el hijo y comprobará la causa por la 
que ha finalizado el hijo (finalización normal o por recepción de una señal), imprimiendo un mensaje por pantalla.

El alumno debe consultar las páginas de manual de: sigaction, alarm, kill, wait.

Para comprobar el funcionamiento correcto de nuestro programa podemos usar
 como argumento un ejecutable que termine en menos de 5 segundos (como ls o echo) y uno que no finalice hasta que le llegue la señal (como xeyes).

Una vez funcione el programa, modificar el padre para que ignore la señal SIGINT y comprobar que, efectivamente, lo hace.

*/