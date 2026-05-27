#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <time.h>
#include <sys/wait.h>

#define K 10
#define N 2
#define M 2
#define TASK_LEN 11

typedef struct {
    char data[TASK_LEN];
} Task;

typedef struct {
    Task queue[K];
    int head; // stad pobieramy dane
    int tail; // tu zapisujemy dane
    int count; // ile jest aktualnie zadan w buforze
} SharedData;

void producer(SharedData *shm, sem_t *empty, sem_t *full, sem_t *mutex, int id) {
    srand(time(NULL) ^ (getpid() << 16));
    while (1) {
        sem_wait(empty); // Czekaj na wolne miejsce
        sem_wait(mutex); // Zablokuj dostęp do pamięci
        
        Task new_task;
        for (int i = 0; i < 10; i++) new_task.data[i] = 'A' + (rand() % 26);
        new_task.data[10] = '\0';

        shm->queue[shm->tail] = new_task; // dodajemy zadanie
        shm->tail = (shm->tail + 1) % K; // przesuwamy indeks
        shm->count++;
        printf("Producent %d Wygenerowano: %s\n", id, new_task.data);

        sem_post(mutex); // Odblokuj dostęp
        sem_post(full);  // Zgłoś, że jest nowe zadanie
        sleep(1);
    }
}

void consumer(SharedData *shm, sem_t *empty, sem_t *full, sem_t *mutex, int id) {
    while (1) {
        sem_wait(full);  // Czekaj na zadanie w buforze
        sem_wait(mutex); // Zablokuj dostęp do pamięci
        
        Task my_task = shm->queue[shm->head];
        shm->head = (shm->head + 1) % K;
        shm->count--;

        sem_post(mutex); // Odblokuj dostęp
        sem_post(empty); // Zgłoś, że zwolniono miejsce

        // Opóźnienie 0.3s ZAWSZE poza sekcją krytyczną
        
        printf("[Konsument %d] Pobrano: ", id);
        for (int i = 0; i < 10; i++) {
            printf("%c", my_task.data[i]);
            fflush(stdout);
            usleep(300000); // 0.3s
        }
        printf("\n");
    }
}

int main() {
    int shm_fd = shm_open("/shm_zad1", O_CREAT | O_RDWR, 0666); // Tworzymy miejsce w pamieci
    ftruncate(shm_fd, sizeof(SharedData)); // ustawiamy rozmiar na potrzebny przez nas
    SharedData *shm = mmap(NULL, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0); // mapujemy wskaznik zeby mozna sie bylo odniesc
    memset(shm, 0, sizeof(SharedData)); // czyscimy pamiec

    sem_unlink("/sem_empty1"); sem_unlink("/sem_full1"); sem_unlink("/sem_mutex1");
    sem_t *empty = sem_open("/sem_empty1", O_CREAT, 0666, K); // ile wolnych miejsc
    sem_t *full = sem_open("/sem_full1", O_CREAT, 0666, 0); // ile gotowych zadan 
    sem_t *mutex = sem_open("/sem_mutex1", O_CREAT, 0666, 1);

    for (int i = 0; i < N; i++) 
    if (fork() == 0) {

         producer(shm, empty, full, mutex, i + 1); exit(0);

    }

    for (int i = 0; i < M; i++) 
    if (fork() == 0) { 

        consumer(shm, empty, full, mutex, i + 1); exit(0); 

    }
    
    wait(NULL);
    return 0;
}