#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

void handler(int signum) {
    printf("Wywołano handler dla sygnału %d\n", signum);
}

void sig_default() { signal(SIGUSR1, SIG_DFL); }
void sig_ignore() { signal(SIGUSR1, SIG_IGN); }
void sig_handle() { signal(SIGUSR1, handler); }

void sig_mask() {
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigprocmask(SIG_BLOCK, &mask, NULL);
}

void sig_unblock() {
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigprocmask(SIG_UNBLOCK, &mask, NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 2) return 1;

    if (strcmp(argv[1], "default") == 0) sig_default();
    else if (strcmp(argv[1], "ignore") == 0) sig_ignore();
    else if (strcmp(argv[1], "handle") == 0) sig_handle();
    else if (strcmp(argv[1], "mask") == 0) sig_mask();

    for (int i = 1; i <= 20; i++) {
        printf("%d\n", i);
        if (i == 5 || i == 15) {
            printf("Wysyłam sygnał USR1\n");
            raise(SIGUSR1);
        }
        if (i == 10) {
            sigset_t pending;
            sigpending(&pending);
            if (sigismember(&pending, SIGUSR1)) {
                printf("Odblokowuję USR1\n");
                sig_unblock();
            }
        }
        sleep(1);
    }
    printf("Pętla została wykonana w całości\n");
    return 0;
}