#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void handler_usr1(int signum) { printf("Wywołano handler dla sygnału %d\n", signum); }
void sig_default() { signal(SIGUSR1, SIG_DFL); }
void sig_ignore() { signal(SIGUSR1, SIG_IGN); }
void sig_handle() { signal(SIGUSR1, handler_usr1); }
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

void handler_usr2(int signum, siginfo_t *info, void *context) {
    int action = info->si_value.sival_int;
    if (action == 0) sig_default();
    else if (action == 1) sig_ignore();
    else if (action == 2) sig_handle();
    else if (action == 3) sig_mask();
}

int main() {
    struct sigaction act;
    act.sa_sigaction = handler_usr2;
    act.sa_flags = SA_SIGINFO;
    sigemptyset(&act.sa_mask);
    sigaction(SIGUSR2, &act, NULL);

    pause();

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