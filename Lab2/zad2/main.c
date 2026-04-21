#define _POSIX_C_SOURCE 200809L


#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 2) return 1;

    union sigval value;
    if (strcmp(argv[1], "default") == 0) value.sival_int = 0;
    else if (strcmp(argv[1], "ignore") == 0) value.sival_int = 1;
    else if (strcmp(argv[1], "handle") == 0) value.sival_int = 2;
    else if (strcmp(argv[1], "mask") == 0) value.sival_int = 3;
    else return 1;

    pid_t pid = fork();
    if (pid == 0) {
        execl("./child", "child", NULL);
        exit(1);
    } else if (pid > 0) {
        sleep(1); // Daj czas potomkowi na ustawienie obsługi USR2
        sigqueue(pid, SIGUSR2, value);
    }
    return 0;
}