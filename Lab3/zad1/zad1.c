#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

double f(double x) {
    return 4.0 / (x * x + 1.0);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uzycie: %s <szerokosc_prostokata> <maks_liczba_procesow_n>\n", argv[0]);
        return 1;
    }

    double step = atof(argv[1]);
    int n = atoi(argv[2]);

    for (int k = 1; k <= n; k++) {
        int pipes[k][2];
        struct timespec start_time, end_time;
        
        clock_gettime(CLOCK_MONOTONIC, &start_time);

        for (int i = 0; i < k; i++) {
            if (pipe(pipes[i]) == -1) {
                perror("Blad tworzenia potoku");
                return 1;
            }

            pid_t pid = fork();
            if (pid == -1) {
                perror("Blad fork");
                return 1;
            }

            if (pid == 0) { 
                close(pipes[i][0]);

                double start = i * (1.0 / k);
                double end = (i + 1) * (1.0 / k);
                double partial_sum = 0.0;

                for (double x = start; x < end; x += step) {
                    partial_sum += f(x) * step;
                }

                write(pipes[i][1], &partial_sum, sizeof(partial_sum));
                close(pipes[i][1]);
                
                exit(0);
            }
        }

        double total_sum = 0.0;
        
        for (int i = 0; i < k; i++) {
            close(pipes[i][1]); 
            
            double partial;
            read(pipes[i][0], &partial, sizeof(partial));
            total_sum += partial;
            
            close(pipes[i][0]); 
            wait(NULL);       
        }

        clock_gettime(CLOCK_MONOTONIC, &end_time);
        double elapsed = (end_time.tv_sec - start_time.tv_sec) + 
                         (end_time.tv_nsec - start_time.tv_nsec) / 1e9;

        printf("Liczba procesow (k) = %d, Wynik = %.10f, Czas = %.6f s\n", k, total_sum, elapsed);
    }

    return 0;
}