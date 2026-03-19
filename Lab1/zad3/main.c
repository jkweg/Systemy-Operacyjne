#include "definitions.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uzycie: %s <N_procesow> <M_komunikatow>\n", argv[0]);
        return 1;
    }

    remove(OUTPUT_FILE);

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

    return 0;
}