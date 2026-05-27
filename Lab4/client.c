#include "common.h"

int main() {
    // Generowanie klucza klienta na podstawie jego PID, by był unikalny
    key_t client_key = ftok(SERVER_PATH, getpid() & 0xFF);
    int client_qid = msgget(client_key, IPC_CREAT | 0666);
    
    // Otwarcie kolejki serwera
    key_t server_key = ftok(SERVER_PATH, SERVER_PROJ);
    int server_qid = msgget(server_key, 0666);

    if (server_qid == -1) {
        printf("Nie można połączyć się z serwerem. Czy serwer jest uruchomiony?\n");
        exit(1);
    }

    // Rejestracja u serwera (wysłanie INIT)
    struct msgbuf msg;
    msg.mtype = MSG_INIT;
    msg.client_key = client_key;
    msgsnd(server_qid, &msg, MSG_SIZE, 0);

    // Oczekiwanie na odpowiedź serwera z nadanym ID
    msgrcv(client_qid, &msg, MSG_SIZE, MSG_INIT, 0);
    int my_id = msg.client_id;
    printf("Zalogowano do serwera pomyślnie! Twoje ID to: %d\n", my_id);
    printf("Możesz zacząć pisać (naciśnij Enter po każdej wiadomości).\n\n");

    // Tworzenie procesu potomnego do asynchronicznego czytania i pisania
    pid_t pid = fork();

    if (pid < 0) {
        perror("Błąd tworzenia procesu");
        exit(1);
    }

    if (pid == 0) {
        // PROCES POTOMNY: Odbieranie i wyświetlanie wiadomości od serwera
        while (1) {
            struct msgbuf rcv_msg;
            if (msgrcv(client_qid, &rcv_msg, MSG_SIZE, MSG_CHAT, 0) > 0) {
                printf("\n[Klient %d]: %s", rcv_msg.client_id, rcv_msg.mtext);
            }
        }
    } else {
        // PROCES MACIERZYSTY: Wczytywanie z klawiatury i wysyłanie do serwera
        while (1) {
            char buffer[MAX_MSG_LEN];
            if (fgets(buffer, MAX_MSG_LEN, stdin) != NULL) {
                struct msgbuf send_msg;
                send_msg.mtype = MSG_CHAT;
                send_msg.client_id = my_id;
                strcpy(send_msg.mtext, buffer);
                
                msgsnd(server_qid, &send_msg, MSG_SIZE, 0);
            }
        }
    }

    msgctl(client_qid, IPC_RMID, NULL); // Oczyszczenie w razie wyjścia
    return 0;
}