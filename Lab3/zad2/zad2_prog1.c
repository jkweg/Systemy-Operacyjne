#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int main() {
    double start, end;
    printf("Podaj poczatek i koniec przedzialu (np. 0 1): ");
    scanf("%lf %lf", &start, &end);

    double params[2] = {start, end};

    int fd_write = open("fifo_req", O_WRONLY);
    int fd_read = open("fifo_res", O_RDONLY);

    write(fd_write, params, sizeof(params));

    double result;
    read(fd_read, &result, sizeof(result));

    printf("\n[Program 1] Odebrano wynik!\n");
    printf("Wartosc calki w przedziale [%.2f, %.2f] wynosi: %.10f\n", start, end, result);

    close(fd_write);
    close(fd_read);

    unlink("fifo_req");
    unlink("fifo_res");

    return 0;
}