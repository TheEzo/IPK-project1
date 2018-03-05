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
    char flag[2] = "\0";
    int c;
    char *hostname = NULL, *login = NULL;
    uint16_t port = 0;
    int  flag_n = 0, flag_f = 0, flag_l = 0,client_socket;
    ssize_t bytesrx;
    ssize_t bytestx;
    struct hostent *server;
    struct sockaddr_in server_address;
    char buffer[BUFSIZE];
    int l;

    while((c = getopt(argc, argv, "h:p:nfl")) != -1){
        switch(c){
            case 'h':
                l = strlen(optarg);
                hostname = (char *) malloc(l*sizeof(char));
                strcpy(hostname, optarg);
                break;
            case 'p':
                port = (uint16_t) atoi(optarg);
                break;
            case 'n':
                flag_n = 1;
                break;
            case 'f':
                flag_f = 1;
                break;
            case 'l':
                flag_l = 1;
                break;
            case '?':
                if (c == 'h')
                    fprintf (stderr, "Option -%c requires an argument.\n", c);
                else if(c == 'p')
                    fprintf( stderr, "Option -%c requires an argument.\n", c);
                else
                    fprintf (stderr, "Unknown option `-%c'.\n", c);
                exit(EXIT_FAILURE);
        }
    }

    if(flag_f && flag_l || flag_f && flag_n || flag_n && flag_l){
        perror("Parameters -f|-n|-l can not be combined\n");
        exit(EXIT_FAILURE);
    }
    if(flag_l && argc%2 || flag_n || flag_f){
        if(!flag_l && argc%2)
            exit(EXIT_FAILURE);
        login = (char *) malloc(strlen(argv[argc-1])*sizeof(char));
        strcpy(login, argv[argc-1]);
    }
    else
        exit(EXIT_FAILURE);

    if(flag_n)
        flag[0] = 'n';
    else if(flag_l)
        flag[0] = 'l';
    else if(flag_f)
        flag[0] = 'f';

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
    bcopy((char *)server->h_addr, (char *)&server_address.sin_addr.s_addr, server->h_length);
    server_address.sin_port = htons(port);

    /* Vytvoreni soketu */
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) <= 0)
    {
        perror("Socket creation failed\n");
        exit(EXIT_FAILURE);
    }

    strcpy(buffer, flag);
    if(login != NULL)
        strcat(buffer, login);
    strcat(buffer, "\0");

    if (connect(client_socket, (const struct sockaddr *) &server_address, sizeof(server_address)) != 0)
    {
        perror("Connection failed\n");
        exit(EXIT_FAILURE);
    }

    /* odeslani zpravy na server */
    bytestx = send(client_socket, buffer, strlen(buffer), 0);
    if (bytestx < 0)
        perror("Message sending failed\n");

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