#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    
    int N = atoi(argv[1]);
    
    for (int i = 0; i < N; i++) {
        if (fork() == 0) {
        
            execl("./child", "child", argv[2], NULL);
    
            return 1; 
        }
    }

    for (int i = 0; i < N; i++) {
        wait(NULL);
    }

    printf("Rodzic  (PID: %d)\n", getpid());

    return 0;
}