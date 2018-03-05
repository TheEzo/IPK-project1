#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <getopt.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <regex.h>

#define BUFFSIZE 1024
int main (int argc, char **argv) {
    int flag_n = 0, flag_f = 0, flag_l = 0;
    char *login;
    int c;
    int port = 0, welcome_socket;
    struct sockaddr_in6 sa;
    struct sockaddr_in6 sa_client;

    while((c = getopt(argc, argv, "h:p:nfl")) != -1){
        switch(c){
            case 'p':
                port = atoi(optarg);
                break;
            case '?':
                if(c == 'p')
                    fprintf( stderr, "Option -%c requires an argument.\n", c);
                else
                    fprintf (stderr, "Unknown option `-%c'.\n", c);
                exit(EXIT_FAILURE);
        }
    }

    if(port == 0){
        fprintf(stderr, "Port required!\n");
        exit(EXIT_FAILURE);
    }

    socklen_t sa_client_len=sizeof(sa_client);
    if ((welcome_socket = socket(PF_INET6, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket creation failed\n");
        exit(EXIT_FAILURE);
    }

    memset(&sa,0,sizeof(sa));
    sa.sin6_family = AF_INET6;
    sa.sin6_addr = in6addr_any;
    sa.sin6_port = htons(port);

    if (bind(welcome_socket, (struct sockaddr*)&sa, sizeof(sa)) < 0)
    {
        perror("Binding failed\n");
        exit(EXIT_FAILURE);
    }
    if ((listen(welcome_socket, 1)) < 0)
    {
        perror("Listening failed\n");
        exit(EXIT_FAILURE);
    }
    while(1)
    {
        int comm_socket = accept(welcome_socket, (struct sockaddr*)&sa_client, &sa_client_len);
        if (comm_socket > 0)
        {
            char buffer[BUFFSIZE];
            ssize_t res = 0;
            res = recv(comm_socket, buffer, BUFFSIZE,0);
            if (res <= 0)
                break;
            if(buffer[0] == 'n')
                flag_n = 1;
            else if(buffer[0] == 'f')
                 flag_f = 1;
            else if(buffer[0] == 'l')
                flag_l = 1;
            if(flag_l && strlen(buffer) < 1 || flag_f || flag_n){
                login = (char *) malloc(strlen(buffer -1)*sizeof(char));
                strcpy(login, buffer+1);
            }
            printf("%s\n", login);
            FILE *f = fopen("/etc/passwd", "r");
            if(f == NULL){
                perror("File failed to open\n");
                exit(EXIT_FAILURE);
            }
            while(fgets(buffer, BUFFSIZE, f) != NULL){
                printf("%s\n", login);
                if(login != NULL){
//                    if(flag_l){
//                        int i = strlen(login) -1;
//                        printf("%c", login[i]);
//                    }
//                    else{
//
//                    }
                }
                while(1){
                    char *pch;
                    if(flag_n){
                        pch = strtok(buffer, ":");
                        int i = 0;
                        while(i != 4){
                            pch = strtok(NULL, ":");
                            i++;
                        }
                        strcpy(buffer, pch);
                    }
                    else if(flag_f){
                        pch = strtok(buffer, ":");
                        int i = 0;
                        while(i != 5){
                            pch = strtok(NULL, ":");
                            i++;
                        }
                        strcpy(buffer, pch);
                    }
                    else if(flag_l){

                    }
                    strcat(buffer, "\n");
                    send(comm_socket, buffer, BUFFSIZE, 0);
                    recv(comm_socket, buffer, BUFFSIZE, 0);
                    if(buffer[0] == '1')
                        break;
                }
            }
            fclose(f);
        }
        close(comm_socket);
        if(login != NULL)
            free(login);
        login = NULL;
    }
}
