#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>

#define BUFSIZE 1024

int main(int argc, char **argv) {
    int c;
    char *hostname = NULL, *login = NULL;
    int port = 0;
    int  flag_n = 0, flag_f = 0, flag_l = 0, client_socket;
    ssize_t bytesrx, bytestx;
    struct hostent *server;
    struct sockaddr_in server_address;
    char buffer[BUFSIZE] = "\0";
    int l;

    while((c = getopt(argc, argv, "h:p:n:f:l")) != -1){
        switch(c){
            case 'h':
                l = (int) strlen(optarg);
                hostname = (char *) malloc(l*sizeof(char));
                strcpy(hostname, optarg);
                break;
            case 'p':
                port = atoi(optarg);
                break;
            case 'n':
                flag_n = 1;
                if(!strcmp(optarg, "f") || !strcmp(optarg, "f")){
                    perror("Parameters -f|-n|-l can not be combined\n");
                    exit(EXIT_FAILURE);
                }
                login = (char *) malloc(strlen(optarg) * sizeof(char));
                strcpy(login, optarg);
                break;
            case 'f':
                flag_f = 1;
                if(!strcmp(optarg, "f") || !strcmp(optarg, "f")){
                    perror("Parameters -f|-n|-l can not be combined\n");
                    exit(EXIT_FAILURE);
                }
                login = (char *) malloc(strlen(optarg) * sizeof(char));
                strcpy(login, optarg);
                break;
            case 'l':
                flag_l = 1;
                break;
            case '?':
                if (optopt == 'h' || optopt == 'p' || optopt == 'n' || optopt == 'f')
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                else
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                exit(EXIT_FAILURE);
        }
    }
    if(flag_l && argc%2){
        login = (char *) malloc(strlen(argv[argc-1]) * sizeof(char));
        strcpy(login, argv[argc-1]);
    }

    if(!(flag_f || flag_l || flag_n)){
        perror("One of -f|-n|-l parameters required\n");
        exit(EXIT_FAILURE);
    }

    if(flag_n)
        buffer[0] = 'n';
    else if(flag_l)
        buffer[0] = 'l';
    else if(flag_f)
        buffer[0] = 'f';

    if(login != NULL)
        strcat(buffer, login);

    if(hostname == NULL || port == 0){
        perror("Host and port required!\n");
        exit(EXIT_FAILURE);
    }

    if ((server = gethostbyname(hostname)) == NULL) {
        fprintf(stderr,"No such host as %s\n", hostname);
        exit(EXIT_FAILURE);
    }

    bzero((char *) &server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    bcopy(server->h_addr_list[0], (char *)&server_address.sin_addr.s_addr, server->h_length);
    server_address.sin_port = htons(port);

    /* Vytvoreni soketu */
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) <= 0)
    {
        perror("Socket creation failed\n");
        exit(EXIT_FAILURE);
    }

    if (connect(client_socket, (const struct sockaddr *) &server_address, sizeof(server_address)) != 0)
    {
        perror("Connection failed\n");
        exit(EXIT_FAILURE);
    }

    /* odeslani zpravy na server */
    bytestx = send(client_socket, buffer, strlen(buffer), 0);
    if (bytestx < 0) {
        perror("Message sending failed\n");
    }

    /* prijeti odpovedi a jeji vypsani */
    while(1){
        bytesrx = recv(client_socket, buffer, BUFSIZE, 0);
        if(bytesrx > 0){
            printf("%s", buffer);
            send(client_socket, "1", 1, 0);
        }
        else if(bytesrx == 0){
            send(client_socket, "1", 1, 0);
            break;
        }
        else
            send(client_socket, "0", 1, 0);
    }

    close(client_socket);

    return 0;
}