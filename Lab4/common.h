#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>

#define SERVER_PATH "/tmp" // Ścieżka do generowania klucza serwera
#define SERVER_PROJ 'S'    // Identyfikator projektu serwera
#define MAX_MSG_LEN 512    // Maksymalna długość wiadomości
#define MAX_CLIENTS 10     // Maksymalna liczba klientów w tablicy

#define MSG_INIT 1
#define MSG_CHAT 2

struct msgbuf {
    long mtype;             
    int client_id;          
    key_t client_key;       
    char mtext[MAX_MSG_LEN];
};

// makro do obliczania rozmiaru komunikatu
#define MSG_SIZE (sizeof(struct msgbuf) - sizeof(long))

#endif