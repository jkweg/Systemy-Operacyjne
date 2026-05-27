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
    Task normal_queue[K];
    int norm_head, norm_tail, norm_count;
    Task prio_queue[K];
    int prio_head, prio_tail, prio_count;
} SharedData;

void producer(SharedData *shm, sem_t *empty, sem_t *full, sem_t *mutex, int id) {
    srand(time(NULL) ^ (getpid() << 16));
    while (1) {
        sem_wait(empty);
        sem_wait(mutex);
        
        Task new_task;
        for (int i = 0; i < 10; i++) new_task.data[i] = 'A' + (rand() % 26);
        new_task.data[10] = '\0';

        if (rand() % 100 < 30) {
            shm->prio_queue[shm->prio_tail] = new_task;
            shm->prio_tail = (shm->prio_tail + 1) % K;
            shm->prio_count++;
            printf("[Producent %d] Wygenerowano PRIORITY: %s\n", id, new_task.data);
        } else {
            shm->normal_queue[shm->norm_tail] = new_task;
            shm->norm_tail = (shm->norm_tail + 1) % K;
            shm->norm_count++;
            printf("[Producent %d] Wygenerowano NORMAL:   %s\n", id, new_task.data);
        }

        sem_post(mutex);
        sem_post(full);
        sleep(1);
    }
}

void consumer(SharedData *shm, sem_t *empty, sem_t *full, sem_t *mutex, int id) {
    while (1) {
        sem_wait(full);
        sem_wait(mutex);
        
        Task my_task;
        int is_prio = 0;
        
        if (shm->prio_count > 0) {
            my_task = shm->prio_queue[shm->prio_head];
            shm->prio_head = (shm->prio_head + 1) % K;
            shm->prio_count--;
            is_prio = 1;
        } else {
            my_task = shm->normal_queue[shm->norm_head];
            shm->norm_head = (shm->norm_head + 1) % K;
            shm->norm_count--;
        }

        sem_post(mutex);
        sem_post(empty);

        printf("[Konsument %d] Pobrano %s: ", id, is_prio ? "PRIO" : "NORM");
        for (int i = 0; i < 10; i++) {
            printf("%c", my_task.data[i]);
            fflush(stdout);
            usleep(300000);
        }
        printf("\n");
    }
}

void manager(SharedData *shm, sem_t *mutex) {
    while (1) {
        sleep(5);
        sem_wait(mutex);
        
        printf("\n--- [MANAGER] Stan Systemu: NORM: %d/10 | PRIO: %d/10 ---\n", shm->norm_count, shm->prio_count);
        
        // zapobieganie starvation - przeniesienie zadania
        if (shm->norm_count > 0) {
            Task to_promote = shm->normal_queue[shm->norm_head];
            shm->norm_head = (shm->norm_head + 1) % K;
            shm->norm_count--;
            
            shm->prio_queue[shm->prio_tail] = to_promote;
            shm->prio_tail = (shm->prio_tail + 1) % K;
            shm->prio_count++;
            printf("[MANAGER] Awansowano zadanie na PRIORITY!\n");
        }
        printf("------------------------------------------------------\n\n");
        
        sem_post(mutex);
    }
}

int main() {
    int shm_fd = shm_open("/shm_zad3", O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(SharedData));
    SharedData *shm = mmap(NULL, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    memset(shm, 0, sizeof(SharedData));

    sem_unlink("/sem_empty3"); sem_unlink("/sem_full3"); sem_unlink("/sem_mutex3");
    sem_t *empty = sem_open("/sem_empty3", O_CREAT, 0666, K);
    sem_t *full = sem_open("/sem_full3", O_CREAT, 0666, 0);
    sem_t *mutex = sem_open("/sem_mutex3", O_CREAT, 0666, 1);

    for (int i = 0; i < N; i++) if (fork() == 0) { producer(shm, empty, full, mutex, i + 1); exit(0); }
    for (int i = 0; i < M; i++) if (fork() == 0) { consumer(shm, empty, full, mutex, i + 1); exit(0); }
    if (fork() == 0) { manager(shm, mutex); exit(0); } // proces menegera
    
    wait(NULL);
    return 0;
}