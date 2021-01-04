#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h> // perror
#include <errno.h>

#define MSG_SIZE 32

void close_socket(int fd) {
    if (close(fd) == -1)
        perror("close");
}

int write_all(int fd, const void *buf, size_t len) {
    size_t written = 0;
    while (written < len) {
        ssize_t res = write(fd, buf, len);
        if (res == -1) {
            if (errno == EAGAIN || errno == EINTR)
                continue;
            return -1;
        }
        written += res;
    }
    return 0;
}

int main(int argc, char *const argv[]) {
    int client = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client == -1) {
        perror("socket");
        return 1;
    }
    char *path = "sock";
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, path);

    int conres = connect(client, (struct sockaddr *) &addr, sizeof(addr));
    if (conres == -1) {
        perror("connect");
        close_socket(client);
        return 1;
    }

    const char *msg = "Hello darkness my old friend";
    if (write_all(client, msg, strlen(msg)) == -1) {
        perror("write_all");
        close_socket(client);
        return 1;
    }

    close_socket(client);

    return 0;
}

