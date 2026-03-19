#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define M 5

int zmiennaGlobalna = 10;

int main(int argc, char *argv[]) {

    int N = atoi(argv[1]);

    for (int i = 0; i < N; i++) {

        if (fork() == 0) {

            zmiennaGlobalna++;
            
            for (int j = 0; j < M; j++) {
                
                printf("Potomek (PID: %d)\n", getpid());

                usleep(250000); 
            }
            return 0; 
        }
    }

    for (int i = 0; i < N; i++) {
        wait(NULL);
    }

    printf("Rodzic  (PID: %d) zmiennaGlobalna=%d\n", getpid(), zmiennaGlobalna);

    return 0;
}