#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>

//--------------------------------SEMAFOROS-------------------------
#define SEM_EMPTY "/sem_empty_buffer"
#define SEM_FULL "/sem_full_buffer"

//--------------------------------MEMORIA COMPARTIDA------------------------
#define SHM_NAME "/mi_memoria_compartida" //"zona de memoria compartida"
#define SHM_SIZE sizeof(int) //tamaño que va a tener nuestra memoria compartida

int *shared_counter = NULL; //contador que estara en la zona compartida (mi caldero)

#define M 10

int finish = 0;

volatile sig_atomic_t terminate = 0;

//Los semaforos con nombre llevan el *
sem_t *sem_empty = NULL;
sem_t *sem_full = NULL;

//Paso 1 Gestion de señales

//Paso 2 Generar la memoria compartida

//Paso 3 Semaforos

//Paso 4 Problema dentro del while()


//Procedimiento para manejar las señales(SIGINT y SEGTERM)
void handle_signal(int sig) {
    if (sig == SIGINT || sig == SIGTERM) {
        printf("Recibida señal de terminación\n");
        terminate = 1;
    }
}


void putServingsInPot(int servings)
{

	for (int i = 0; i < servings; i++){


		(*shared_counter)++;//aumentamos +1(cada racion)

		sem_post(sem_full);//aumnetamos igual que las raciones(debe coincidir sem_full con shared_counter)

	}
	
	printf("%d raciones servidas por %lu\n", (*shared_counter), getpid());

}

void cook(void)
{
	while(!terminate) {
		putServingsInPot(M);
		sem_wait(sem_empty);//cocinero a llenado el caldero y espera aqui hasta que los salvajes lo vacien
	}
}


int main(int argc, char *argv[])
{
	
	//-------------------------------------------SEÑALES----------------------------------------
	struct sigaction sa;
	sa.sa_handler = handle_signal;
	sa.sa_flags = 0;

	if(sigemptyset(&sa.sa_mask) == -1){
		perror("Error al inicializar el conjunto de señales");
		exit(EXIT_FAILURE);
	}

	//Estos if siempre hay que tener 1 por cada señal que queremos manejar
	if(sigaction(SIGINT, &sa, NULL) == -1){
		perror("Error en el sigaction de SIGINT");
		exit(EXIT_FAILURE);
	}

	if(sigaction(SIGTERM, &sa, NULL) == -1){
		perror("Error en el sigaction de SIGTERM");
		exit(EXIT_FAILURE);
	}

	//-------------------------------------------MEMORIA COMPARTIDA------------------------------

	//Abrir o crear la memoria compartida
	int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
	if(shm_fd == -1){
		perror("Error al abrir/crear la memoria compartida");
		exit(EXIT_FAILURE);
	}

	//Ahora damos tamaño a la memoria compartido(seguridad y medida de proteccion)
	if(ftruncate(shm_fd, SHM_SIZE) == -1){
		perror("Error al dar tamaño a la memoria compartida");
		close(shm_fd);
		shm_unlink(SHM_NAME);//hay que eliminar la zona de memoria compartida si falla al dar tamaño a esta misma
		exit(EXIT_FAILURE);
	}

	//Por ultimo debemos mapear la memoria compartida
	shared_counter = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

	if(shared_counter == MAP_FAILED){//Comprobamos si ha fallado el mapeo de memoria
		perror("Error al mapear la memoria");
		close(shm_fd);
		shm_unlink(SHM_NAME);//Eliminamos la memoria compartida si falla el proceso de mapearla
		exit(EXIT_FAILURE);
	}

	*shared_counter = 0; //ponemos el contador que se encuentra en la zona de memoria compartida a 0

	//------------------------------------------------------SEMAFOROS--------------------------------------------------(INICIALIZADOS A 0 LOS 2 SEMAFOROS)
	/*
		Empty nos indica(0 o 1): 0 -> vacio
		Full nos indica(0 .... servings) raciones disponibles
	*/

	sem_empty = sem_open(SEM_EMPTY, O_CREAT, 0644, 0);
	if(sem_empty == SEM_FAILED){
		perror("Error al crear el semaforo de empty");
		shm_unlink(SHM_NAME);//Eliminamos la memoria compartida que hemos creado con anterioridad
		close(shm_fd);
		exit(EXIT_FAILURE);
	}
	
	sem_full = sem_open(SEM_FULL, O_CREAT, 0644, 0);
	if(sem_full == SEM_FAILED){
		perror("Error al crear el semaforo de full");
		sem_close(sem_empty);//Eliminamos el primer semaforo que hemos creado
		shm_unlink(SHM_NAME);//Eliminamos la memoria compartida que hemos creado con anterioridad
		close(shm_fd);
		exit(EXIT_FAILURE);
	}

	//---------------------COOK------------------------------------------------


	cook();


	//---------------------ELIMINAMOS TODO LO QUE HEMOS CREADO--------------------

	if (close(shm_fd) == -1) {
    	perror("Error al cerrar shm_fd");
	}

	if (sem_close(sem_empty) == -1) {
    	perror("Error al cerrar sem_empty");
	}

	if (sem_close(sem_full) == -1) {
		perror("Error al cerrar sem_full");
	}

	if (sem_unlink(SEM_EMPTY) == -1) {
		perror("Error al desvincular SEM_EMPTY");
	}

	if (sem_unlink(SEM_FULL) == -1) {
		perror("Error al desvincular SEM_FULL");
	}

	if (munmap(shared_counter, SHM_SIZE) == -1) {
		perror("Error al desmapear shared_counter");
	}

	if (shm_unlink(SHM_NAME) == -1) {
		perror("Error al desvincular SHM_NAME");
	}


	exit(EXIT_SUCCESS);

}
