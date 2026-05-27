#include "common.h"

int main() {
    int clients_queues[MAX_CLIENTS];
    int clients_count = 0;

    key_t server_key = ftok(SERVER_PATH, SERVER_PROJ);
    int server_qid = msgget(server_key, IPC_CREAT | 0666);
    
    if (server_qid == -1) {
        perror("Błąd tworzenia kolejki serwera");
        exit(1);
    }

    printf("Serwer uruchomiony. Oczekiwanie na klientów...\n");

    struct msgbuf msg;

    while (1) {
        if (msgrcv(server_qid, &msg, MSG_SIZE, 0, 0) == -1) {
            perror("Błąd odbierania komunikatu");
            continue;
        }

        if (msg.mtype == MSG_INIT) {
            if (clients_count < MAX_CLIENTS) {
                int client_qid = msgget(msg.client_key, 0666);
                
                clients_queues[clients_count] = client_qid;
                int new_id = clients_count + 1;
                clients_count++;

                struct msgbuf reply;
                reply.mtype = MSG_INIT;
                reply.client_id = new_id;
                msgsnd(client_qid, &reply, MSG_SIZE, 0);
                
                printf("Nowy klient podłączony. Nadano ID: %d\n", new_id);
            } else {
                printf("Odrzucono klienta - brak miejsc w tablicy.\n");
            }

        } else if (msg.mtype == MSG_CHAT) {
            printf("Otrzymano wiadomość od ID %d: %s", msg.client_id, msg.mtext);
            
            for (int i = 0; i < clients_count; i++) {
                if ((i + 1) != msg.client_id) {
                    msgsnd(clients_queues[i], &msg, MSG_SIZE, 0);
                }
            }
        }
    }

    msgctl(server_qid, IPC_RMID, NULL);
    return 0;
}