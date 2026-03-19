#include "definitions.h"

int main(int argc, char *argv[]) {
    if (argc != 2) return 1;
    
    int M = atoi(argv[1]);

    FILE *plik = fopen(OUTPUT_FILE, "a");
    if (plik == NULL) {
        perror("Blad otwarcia pliku");
        return 1;
    }

    int fd = fileno(plik);

    
    flock(fd, LOCK_EX);

    for (int i = 0; i < M; i++) {
        fprintf(plik, "Potomek (PID: %d)\n", getpid());
        
        fflush(plik);
        
        usleep(250000); 
    }

    flock(fd, LOCK_UN);

    fclose(plik);

    return 0;
}