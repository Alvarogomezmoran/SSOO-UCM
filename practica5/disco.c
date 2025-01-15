//Alvaro Gomez Moran
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define CAPACITY 5
#define VIPSTR(vip) ((vip) ? "  vip  " : "not vip") //0 o 1 ( no vip o vip )

//turnos para mantener el orden dentro de las filas de clientes, ya sea vip o normal
int turno_actual_vip = 0;
int turno_actual_normal = 0;

//variables que dispensan los turnos de cada grupo
int dispensador_turnos = 0;
int dispensador_turnos_vip = 0;

int aforo_actual = 0;//Numero de clientes dentro de la discoteca
int vips_esperando = 0;//Conocer si hay vips esperando

static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t clientes_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t clientes_vip_cond = PTHREAD_COND_INITIALIZER;

void enter_normal_client(int id)
{
    int turno;
    pthread_mutex_lock(&m); //empieza la zona critica

    turno = dispensador_turnos++; //damos un turno al bailarin normal

    while(aforo_actual == CAPACITY || turno != turno_actual_normal || vips_esperando != 0){ //controlo el aforo, el turno(tienen que entrar en orden de llegada) y que no existan vips esperando 
        pthread_cond_wait(&clientes_cond, &m);
    }

    turno_actual_normal++; //ha entrado y aumentamos el turno normal

    aforo_actual++; //incrementamos el aforo ya que ha entrado 1 normal

    if(vips_esperando == 0){
        pthread_cond_broadcast(&clientes_cond); //si no hay vips esperando se despiertan los hilos esperando en la condicion de normales
    }else{
        pthread_cond_broadcast(&clientes_vip_cond); //si hay vips esperando se despiertan los hilos esperando en esa condicion
    }

    printf("Cliente %2d entra como normal. Turno: %2d\n", id, turno);

    pthread_mutex_unlock(&m);


}

void enter_vip_client(int id)
{
    
    int turno;
    pthread_mutex_lock(&m);

    turno = dispensador_turnos_vip++; //damos un turno al bailarin vip

    vips_esperando++; //aumentamos el vip esperando para conocer si hay vips esperando en ciertas zonas de nuestro codigo

    while(aforo_actual == CAPACITY || turno != turno_actual_vip){ //controlo el aforo y el turno del que quiere entrar (tienen que entrar en orden de llegada)
        pthread_cond_wait(&clientes_vip_cond, &m);
    }

    vips_esperando--; //ha entrado el vip y restamos 1 a la cola de vips
    turno_actual_vip++; //ha entrado y aumentamos el turno vips (por printf("Ha entrado el cliente con id %d que es normal\n", id);si hay vips en cola)

    aforo_actual++; //incrementamos el aforo ya que ha entrado 1 vips a la disco

    if(vips_esperando == 0){
        pthread_cond_broadcast(&clientes_cond); //si no hay vips esperando se despiertan los hilos esperando en la condicion de normales
    }else{
        pthread_cond_broadcast(&clientes_vip_cond); //si hay vips esperando se despiertan los hilos esperando en esa condicion
    }

    printf("Cliente %2d entra como vip. Turno: %2d\n", id, turno);
    pthread_mutex_unlock(&m);

}

void dance(int id, int isvip)
{
	printf("Client %2d (%s) dancing in dicomparosco\n", id, VIPSTR(isvip));
	//sleep((rand() % 3) + 1);
    sleep(2);
}

void disco_exit(int id, int isvip)
{
    pthread_mutex_lock(&m);

    --aforo_actual; //sale un bailarin y le restamos del aforo

    if(vips_esperando == 0){
        pthread_cond_broadcast(&clientes_cond); //si no hay vips esperando se despiertan los hilos esperando en la condicion de normales
    }else{
        pthread_cond_broadcast(&clientes_vip_cond); //si hay vips esperando se despiertan los hilos esperando en esa condicion
    }


    pthread_mutex_unlock(&m);

    printf("Cliente %2d (%s) sale de la discoteca. Aforo: %d\n", id ,VIPSTR(isvip), aforo_actual);

}

void *client(void *arg)
{
	int id = *((int *)arg);
	int isvip = *((int *)(arg + sizeof(int)));

	if ( isvip )
        enter_vip_client( id );
    else
        enter_normal_client( id );
    dance( id, isvip);
    disco_exit( id, isvip);


}

int main(int argc, char *argv[])
{
	if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <nombre_del_archivo>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    srand((unsigned int)time(NULL));

    FILE *file = fopen(argv[1], "r"); //modo lectura
    if (file == NULL)
    {
        perror("Error abriendo el fichero");
        exit(EXIT_FAILURE);
    }

    int M;
    if (fscanf(file, "%d", &M) != 1)//M es el numero total de clientes que nos van a pasar
    {
        fprintf(stderr, "Error leyendo el nuemro total de clientes\n");
        exit(EXIT_FAILURE);
    }

    pthread_t threads[M]; //array para almacenar los hilos y poder recorrerlos

    for (int i = 0; i < M; i++)
    {
        int *args = (int *)malloc(2 * sizeof(int)); //solicita memoria para almacenar 2 enteros y se hace casting a (int *)
        									//En args->primer argumento sera el ID del cliente y el segundo argumento el 0 o 1 (vip o no)

        if (fscanf(file, "%d", &args[1]) != 1)
        {
            fprintf(stderr, "Error leyendo el tipo de cliente\n");
            exit(EXIT_FAILURE);
        }
        args[0] = i + 1;          //ID del cliente
        pthread_create(&threads[i], NULL, client, (void *)args);
    
    }

    fclose(file);

    for (int i = 0; i < M; i++)
    {
        pthread_join(threads[i], NULL);
    }

    printf("\n");
    printf("-----------------------\n");
    printf("La discoteca esta vacia\n");
    printf("-----------------------\n");
    printf("\n");

    return 0;
}
