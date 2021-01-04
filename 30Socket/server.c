#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h> // perror, printf
#include <ctype.h> // toupper
#include <errno.h>

void to_upper_case(char *buf, size_t len) {
    for (size_t i = 0; i < len; i++)
        buf[i] = toupper(buf[i]);
}

void close_unix_socket(int fd, const char *path) {
    if (close(fd) == -1)
        perror("close");
    if (path != NULL && unlink(path) == -1)
        perror("unlink");
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
    int listener = socket(AF_UNIX, SOCK_STREAM, 0);
    if (listener == -1) {
        perror("socket");
        return 1;
    }
    char *path = "sock";
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, path);

    if (bind(listener, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        perror("bind");
        close_unix_socket(listener, NULL);
        return 1;
    }

    if (listen(listener, 1) == -1) {
        perror("listen");
        close_unix_socket(listener, path);
        return 1;
    }

    struct sockaddr_un client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int client = accept(listener, (struct sockaddr *) &client_addr, &client_addr_len);
    if (client == -1) {
        perror("accept");
        close_unix_socket(listener, path);
        return 1;
    }

    char buf[1024];

    for (;;) {
        ssize_t len = read(client, buf, sizeof(buf));
        if (len == 0) {
            if (write(0, "\n", 1) == -1)
                perror("\nwrite");
            break;
        }
        if (len == -1) {
            if (errno == EAGAIN || errno == EINTR)
                continue;
            perror("\nread");
            break;
        }

        to_upper_case(buf, len);

        if (write_all(0, buf, len) == -1) {
            perror("\nwrite_all");
            break;
        }
    }

    close_unix_socket(client, NULL);
    close_unix_socket(listener, path);

    return 0;
}
