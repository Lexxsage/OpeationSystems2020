#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>

#define   MSGSIZE   32
#define   NAME   "JustSocket"

int main() {
    int sock, msgsock;
    int ReadNum;
    struct sockaddr_un server;
    char buf[MSGSIZE];
    
    unlink(NAME);
    
    sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0) 
    {
        perror("Opening stream socket\n");
        return -1;
    }

    server.sun_family = AF_UNIX;
    strcpy(server.sun_path, NAME);

    if (bind(sock, (struct sockaddr *) &server, sizeof(struct sockaddr_un))) 
    {
        perror("Binding stream socket\n");
        if (close(sock) == -1)
            perror("Closing socket\n");
        return -1;
    }

    printf("Socket has name %s\n", server.sun_path);
    listen(sock, 5);

    while (1) 
    {
        msgsock = accept(sock, 0, 0);
        if (msgsock == -1)
            perror("Accepting\n");
        else
            while (ReadNum > 0) {
                if ((ReadNum = read(msgsock, buf, 1024)) < 0) {
                    perror("Reading stream message\n");
                    break;
                }
                if (ReadNum== 0) {
                    printf("Ending connection\n");
                    break;
                } else {
                    for (int i = 0; i < MSGSIZE; i++)
                        buf[i] = (char) toupper(buf[i]);
                }
                printf("%s\n", buf);
            }
        if (close(sock) == -1)
            perror("Closing socket\n");
        break;
    }
    if (close(sock) == -1)
        perror("Closing socket\n");
}
