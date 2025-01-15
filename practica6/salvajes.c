#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>

#define NUMITER 100

//--------------------------------SEMAFOROS-------------------------
#define SEM_EMPTY "/sem_empty_buffer"
#define SEM_FULL "/sem_full_buffer"

//--------------------------------MEMORIA COMPARTIDA------------------------
#define SHM_NAME "/mi_memoria_compartida" //"zona de memoria compartida"
#define SHM_SIZE sizeof(int) //tamaño que va a tener nuestra memoria compartida

int *shared_counter = NULL; //contador que estara en la zona compartida (mi caldero)

//Los semaforos con nombre llevan el *
sem_t *sem_empty = NULL;
sem_t *sem_full = NULL;



int getServingsFromPot(void)
{
	
	if((*shared_counter) <= 1){
		sem_post(sem_empty);
	}
	
	sem_wait(sem_full);
	(*shared_counter)--;

	printf("%lu comiendo. %d servido aun en el caldero\n", getpid(), (*shared_counter));


}

void eat(void)
{
	unsigned long id = (unsigned long) getpid();
	printf("Savage %lu eating\n", id);
	sleep(rand() % 5);
}

void savages(void)
{

	for(int i = 0; i < NUMITER; i++){
		getServingsFromPot();
		eat();
	}

}

int main(int argc, char *argv[])
{
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


	//------------------------------------------------------SEMAFOROS--------------------------------------------------

	sem_empty = sem_open(SEM_EMPTY, 0);
	if(sem_empty == SEM_FAILED){
		perror("Error al abrir el semaforo de empty");
		shm_unlink(SHM_NAME);//Eliminamos la memoria compartida que hemos creado con anterioridad
		close(shm_fd);
		exit(EXIT_FAILURE);
	}
	
	sem_full = sem_open(SEM_FULL, 0);
	if(sem_full == SEM_FAILED){
		perror("Error al abrir el semaforo de full");
		sem_close(sem_empty);//Eliminamos el primer semaforo que hemos creado
		shm_unlink(SHM_NAME);//Eliminamos la memoria compartida que hemos creado con anterioridad
		close(shm_fd);
		exit(EXIT_FAILURE);
	}

	//--------------------------------EAT----------------------------

	savages();

	//--------------------------------------ELIMINAMOS LO UTILIZADO----------------------

	if (close(shm_fd) == -1) {
    	perror("Error al cerrar shm_fd");
	}

	if (sem_close(sem_empty) == -1) {
    	perror("Error al cerrar sem_empty");
	}

	if (sem_close(sem_full) == -1) {
		perror("Error al cerrar sem_full");
	}

	if (munmap(shared_counter, SHM_SIZE) == -1) {
		perror("Error al desmapear shared_counter");
	}

	return 0;
}
