#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include "sig_lib.h"

#ifdef DYNAMIC
#include <dlfcn.h>
#endif

int main(int argc, char *argv[]) {
    if (argc != 2) return 1;

    void (*p_unblock)() = NULL;

#ifdef DYNAMIC
    void *handle = dlopen("./libsigs.so", RTLD_LAZY);
    if (!handle) return 1;

    void (*fn)() = NULL;
    if (strcmp(argv[1], "default") == 0) fn = dlsym(handle, "sig_default");
    else if (strcmp(argv[1], "ignore") == 0) fn = dlsym(handle, "sig_ignore");
    else if (strcmp(argv[1], "handle") == 0) fn = dlsym(handle, "sig_handle");
    else if (strcmp(argv[1], "mask") == 0) fn = dlsym(handle, "sig_mask");
    
    if (fn) fn();
    p_unblock = dlsym(handle, "sig_unblock");
#else
    if (strcmp(argv[1], "default") == 0) sig_default();
    else if (strcmp(argv[1], "ignore") == 0) sig_ignore();
    else if (strcmp(argv[1], "handle") == 0) sig_handle();
    else if (strcmp(argv[1], "mask") == 0) sig_mask();
    p_unblock = sig_unblock;
#endif

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
                if (p_unblock) p_unblock();
            }
        }
        sleep(1);
    }
    printf("Pętla została wykonana w całości\n");
    
#ifdef DYNAMIC
    dlclose(handle);
#endif
    return 0;
}