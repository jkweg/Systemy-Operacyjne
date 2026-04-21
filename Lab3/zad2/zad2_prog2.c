#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

double f(double x) {
    return 4.0 / (x * x + 1.0);
}

int main() {
    mkfifo("fifo_req", 0666);
    mkfifo("fifo_res", 0666);

    printf("[Program 2] Oczekuje na przedzial od Programu 1...\n");

    int fd_read = open("fifo_req", O_RDONLY);
    int fd_write = open("fifo_res", O_WRONLY);

    double params[2]; 
    read(fd_read, params, sizeof(params));
    
    printf("[Program 2] Otrzymano przedzial: [%.2f, %.2f]. Licze...\n", params[0], params[1]);

    double start = params[0];
    double end = params[1];
    double step = 0.000001; 
    double sum = 0.0;

    for (double x = start; x < end; x += step) {
        sum += f(x) * step;
    }

    write(fd_write, &sum, sizeof(sum));
    printf("[Program 2] Wynik odeslany. Koncze dzialanie.\n");

    close(fd_read);
    close(fd_write);

    return 0;
}