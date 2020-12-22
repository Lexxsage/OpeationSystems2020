#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define CLIENTMES "Hello darkness my old friend\n"

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_un server;

    if (argc < 2) 
    {
        printf("Usage:%s <pathname>\n", argv[0]);
        exit(1);
    }

    sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0) 
    {
        perror("Opening stream socket\n");
        return -1;
    }

    server.sun_family = AF_UNIX;
    strcpy(server.sun_path, argv[1]);

    if (connect(sock, (struct sockaddr *) &server, sizeof(struct sockaddr_un)) < 0) 
    {    
        perror("Connecting stream socket\n");
        if (close(sock) == -1)
            perror("Closing socket\n");
        return -1;
    }

    if (write(sock, CLIENTMES, sizeof(CLIENTMES)) != strlen(CLIENTMES)) 
    {
        perror("Witing message from CLIENT\n");
        if (close(sock) == -1)
            perror("Closing socket\n");
        return -1;
    }

    if (close(sock) == -1)
        perror("Closing socket\n");
}
