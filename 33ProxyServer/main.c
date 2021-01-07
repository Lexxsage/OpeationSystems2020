#include <errno.h>
#include <signal.h> // sigaction
#include <sys/types.h> // Man recomends to include this header alongside with socket.h altough it is not required
#include <sys/socket.h> // socket, bind
#include <netinet/in.h> // sockaddr_in, in_port_t, in_addr
#include <arpa/inet.h> // inet_aton, inet_ntoa
#include <fcntl.h> // fcntl
#include <poll.h> // poll, pollfd, nfds_t
#include <unistd.h> // close
#include <stdio.h> // perror
#include <stdlib.h> // malloc
#include <string.h> // memset

#include "round_buffer.h"

// Maximal number of pending connections.
#define SOCK_BACKLOG 50
#define MAX_CLIENTS 510
#define BUFFER_SIZE 1024

struct client {
    int src_fd;
    int dst_fd;
    unsigned src_dst_active : 1;
    unsigned dst_src_active : 1;
    struct round_buffer src_dst_buf;
    struct round_buffer dst_src_buf;
    struct sockaddr_in src_address;
};

struct sockaddr_in listen_addr;
struct sockaddr_in dst_address;

int listening_socket;
int clients_count = 0; // Number of connections (i.e. in-out socket pairs, excluding main listening socket).
struct client *clients[MAX_CLIENTS];
struct pollfd fds[1 + 2 * MAX_CLIENTS];

void copy_slot(int ind_from, int ind_to) {
    if (ind_to == ind_from)
        return;

    clients[ind_to] = clients[ind_from];
    fds[1 + 2 * ind_to + 0] = fds[1 + 2 * ind_from + 0];
    fds[1 + 2 * ind_to + 1] = fds[1 + 2 * ind_from + 1];
}

int can_add_slot() {
    return clients_count < MAX_CLIENTS;
}

int add_slot(struct client *client) {
    int slot = clients_count++;
    clients[slot] = client;
    fds[1 + 2 * slot + 0].fd = client->src_fd;
    fds[1 + 2 * slot + 0].events = POLLIN;
    fds[1 + 2 * slot + 1].fd = client->dst_fd;
    fds[1 + 2 * slot + 1].events = POLLIN;
    return slot;
}

void clear_slot(int slot) {
    clients[slot] = NULL;
}

void shrink_slots() {
    int i = 0;
    for (int j = 0; j < clients_count; j++) {
        if (clients[j] != NULL) {
            copy_slot(j, i);
            i++;
        }
    }
    clients_count = i;
}

struct client *make_client(const struct sockaddr_in *addr, int src_fd, int dst_fd) {
    struct client *client = malloc(sizeof(struct client));
    if (client == NULL) {
        return NULL;
    }

    if (rb_init(&client->dst_src_buf, BUFFER_SIZE) == -1) {
        free(client);
        return NULL;
    }
    if (rb_init(&client->src_dst_buf, BUFFER_SIZE) == -1) {
        free(client);
        rb_destroy(&client->dst_src_buf);
        return NULL;
    }

    client->src_address = *addr;
    client->src_fd = src_fd;
    client->dst_fd = dst_fd;
    client->src_dst_active = 1;
    client->dst_src_active = 1;

    return client;
}

void destroy_client(struct client *client) {
    rb_destroy(&client->src_dst_buf);
    rb_destroy(&client->dst_src_buf);
    free(client);
}

// Returns slot of created client if operation succeed.
// -2 is returned if no slots available
// -1 if system error occured.
int add_client(const struct sockaddr_in *addr, int src_fd, int dst_fd) {
    if (!can_add_slot()) {
        return -2;
    }

    struct client *c = make_client(addr, src_fd, dst_fd);
    if (c == NULL)
        return -1;

    return add_slot(c);
}

void disconnect_client(int slot) {
    struct client *client = clients[slot];
    clear_slot(slot);

    if (close(client->src_fd) == -1) 
        perror("close");
    if (close(client->dst_fd) == -1)
        perror("close");

    destroy_client(client);
}

void accept_connection() {
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    int outcoming;
    int incoming = accept(listening_socket, (struct sockaddr *) &addr, &addrlen);
    if (incoming == -1) {
        // Ignore any errors, but warn about unexpected ones.
        switch (errno) {
            case EAGAIN:
            case EINTR: // Should never happen because after poll, accept does not block.
            case ENETDOWN: // Manual recomends to treat following error codes as EAGAIN.
            case EPROTO:
            case ENOPROTOOPT:
            case EHOSTDOWN:
            case ENONET:
            case EHOSTUNREACH:
            case EOPNOTSUPP:
            case ENETUNREACH:
                // Just try again later.
                return;

            default:
                perror("WARNING accept");
                return;
        }
    }

    fprintf(stderr, "Connection from %s:%hu\n",
            inet_ntoa(addr.sin_addr),
            ntohs(addr.sin_port));

    if (!can_add_slot()) {
        // Should never hapen, because POLLIN flag for listening socket is not set
        // if we have MAX_CLIENTS connections.
        fprintf(stderr, "Connection limit exceded\n");
        goto close_incoming_socket;
    }

    fprintf(stderr, "Creating connection to the target...\n");
    outcoming = socket(AF_INET, SOCK_STREAM, 0);
    if (outcoming == -1) {
        perror("socket");
        goto close_incoming_socket;
    }
    if (connect(outcoming, (struct sockaddr *) &dst_address, sizeof(dst_address)) == -1) {
        perror("connect");
        goto close_both_sockets;
    }

    int slot = add_client(&addr, incoming, outcoming);
    if (slot == -1) {
        perror("add_client");
        goto close_both_sockets;
    }

    return;

close_both_sockets:
    fprintf(stderr, "Closing outcoming connection...\n");
    if (close(outcoming) == -1) {
        perror("close");
        return;
    }

close_incoming_socket:
    fprintf(stderr, "Closing incoming connection...\n");
    if (close(incoming) == -1) {
        perror("close");
        return;
    }
    fprintf(stderr, "Connection aborted\n");
}

#define CAUSE_NONE 0
#define CAUSE_EOF 1
#define CAUSE_ERROR 2
int get_rw_error_cause(int res, int err) {
    if (res == -1) {
        switch (err) {
            case EAGAIN:
            case EINTR:
                // Ignore these errors.
                break;
            case ECONNRESET:
            case EPIPE:
                return CAUSE_EOF;
            default:
                return CAUSE_ERROR;
        }
    } else if (res == 0) {
        return CAUSE_EOF;
    }
    return CAUSE_NONE;
}

void try_shutdown(int fd, int dir) {
    if (fd < 0)
        fd = ~fd;

    if (shutdown(fd, dir) == -1 && errno != ENOTCONN)
        // ENOTCONN appears when remote host closes both directions
        perror("shutdown");
}

void clear_pollfd_flags(struct pollfd *fd, short mask) {
    if ((fd->events &= ~mask) == 0 && fd->fd >= 0) {
        // We don't want to receive POLLHUP after we shutdown reading from socket.
        fd->fd = ~fd->fd; // Negative fd values are ignored by pooll
    }
}

void set_pollfd_flags(struct pollfd *fd, short mask) {
    if (!mask)
        return;

    if (fd->fd < 0)
        fd->fd = ~fd->fd;
    
    fd->events |= mask;
}

int transfer(
        struct pollfd *in_fd,
        struct pollfd *out_fd,
        struct round_buffer *buf) {
    if (in_fd->revents & POLLIN && !rb_full(buf)) {
        ssize_t res = read_rb(in_fd->fd, buf);
        switch (get_rw_error_cause(res, errno)) {
            case CAUSE_ERROR:
                perror("read");
                /* FALLTHROUGH */
            case CAUSE_EOF:
                clear_pollfd_flags(in_fd, POLLIN);
                clear_pollfd_flags(out_fd, POLLOUT);
                try_shutdown(out_fd->fd, SHUT_WR);
                return -1;
        }
    }
    if (out_fd->revents & POLLOUT && !rb_empty(buf)) {
        ssize_t res = write_rb(out_fd->fd, buf);
        switch (get_rw_error_cause(res, errno)) {
            case CAUSE_ERROR:
                perror("write");
                /* FALLTHROUGH */
            case CAUSE_EOF:
                clear_pollfd_flags(in_fd, POLLIN);
                clear_pollfd_flags(out_fd, POLLOUT);
                try_shutdown(in_fd->fd, SHUT_RD);
                return -1;
        }
    }

    if (rb_empty(buf))
        clear_pollfd_flags(out_fd, POLLOUT);
    else
        set_pollfd_flags(out_fd, POLLOUT);

    return 0;
}

int parse_port(const char *str, in_port_t *res) {
    char *end;
    errno = 0;
    long r = strtol(str, &end, 10);
    if (r < 0 || r > 65535)
        return -1;
    *res = (in_port_t) r;
    return 0;
}

void print_usage_and_exit(const char *name) {
    fprintf(stderr, "USAGE %s <listen-port> <target-ip> <target-port>", name);
    exit(1);
}

void parse_args(int argc, char *const argv[]) {
    if (argc != 4) {
        print_usage_and_exit(argv[0]);
    }

    in_port_t listen_port;
    if (parse_port(argv[1], &listen_port) == -1) {
        fprintf(stderr, "Invalid listening port value\n");
        print_usage_and_exit(argv[0]);
    }

    struct in_addr target_ip_addr;
    if (!inet_aton(argv[2], &target_ip_addr)) {
        fprintf(stderr, "Invalid target ip address\n");
        print_usage_and_exit(argv[0]);
    }

    in_port_t target_port;
    if (parse_port(argv[3], &target_port) == -1) {
        fprintf(stderr, "Invalid target port value\n");
        print_usage_and_exit(argv[0]);
    }

    struct in_addr listen_ip_addr;
    listen_ip_addr.s_addr = htonl(INADDR_ANY);

    listen_addr.sin_family = AF_INET;
    listen_addr.sin_port = htons(listen_port);
    listen_addr.sin_addr = listen_ip_addr; 

    dst_address.sin_family = AF_INET;
    dst_address.sin_port = htons(target_port);
    dst_address.sin_addr = target_ip_addr;
}

volatile int term_signal_received = 0;
void termination_signal_handler(int sig) {
    term_signal_received = 1;
}

void setup_signals() {
    struct sigaction action;
    action.sa_handler = termination_signal_handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    sigaction(SIGINT, &action, NULL);
    sigaction(SIGQUIT, &action, NULL);

    action.sa_handler = SIG_IGN;

    sigaction(SIGPIPE, &action, NULL);
}

void close_listening_socket() {
    if (close(listening_socket) == -1)
        perror("close");
}

int start_listening_socket() {
    listening_socket = socket(AF_INET,  SOCK_STREAM, 0);
    if (listening_socket == -1) {
        perror("Create listening socket");
        return 1;
    }

    if (bind(listening_socket,
                (struct sockaddr *) &listen_addr,
                sizeof(listen_addr)) == -1) {
        perror("Bind listening socket");
        close_listening_socket();
        return 1;
    }

    if (listen(listening_socket, SOCK_BACKLOG) == -1) {
        perror("Cannot start listening");
        close_listening_socket();
        return 1;
    }

    return 0;
}

void prepare_poll_structures() {
    memset(fds, 0, sizeof(fds));

    // Setup listenig socket events.
    fds[0].fd = listening_socket;
}

int main(int argc, char *const argv[]) {
    parse_args(argc, argv); 
    setup_signals();

    int start_res = start_listening_socket();
    if (start_res != 0)
        return start_res;

    prepare_poll_structures();

    while (!term_signal_received) {
        if (clients_count < MAX_CLIENTS)
            fds[0].events = POLLIN;
        else
            fds[0].events = 0;

        nfds_t nfds = 1 + 2 * clients_count;
        int cnt = poll(fds, nfds, -1);
        if (cnt == -1) {
            if (errno == EINTR || errno == EAGAIN)
                continue;
            perror("poll");
            break;
        }

        int saved_clients_count = clients_count;

        // Accept connections.
        if (fds[0].revents & POLLIN) {
            accept_connection();
        }

        for (int i = 0; i < saved_clients_count; i++) {
            struct pollfd *src_fd = &fds[2 * i + 1];
            struct pollfd *dst_fd = &fds[2 * i + 2];
            struct client *client = clients[i];

            if (client->src_dst_active) {
                int res = transfer(src_fd, dst_fd, &client->src_dst_buf);

                if (res == -1)
                    client->src_dst_active = 0;
            }
            if (client->dst_src_active) {
                int res = transfer(dst_fd, src_fd, &client->dst_src_buf);

                if (res == -1)
                    client->dst_src_active = 0;
            }
            if (!client->src_dst_active && !client->dst_src_active) {
                fprintf(stderr, "Disconnecting %s:%hu...\n",
                        inet_ntoa(client->src_address.sin_addr),
                        ntohs(client->src_address.sin_port));

                disconnect_client(i);
            }
        }
        shrink_slots();
    }

    if (term_signal_received)
        fprintf(stderr, "Termination signal received\n");

    fprintf(stderr, "Terminating connections\n");
    for (int i = 0; i < clients_count; i++) {
        disconnect_client(i);
    }

    fprintf(stderr, "Closing listening socket\n");
    close_listening_socket();

    return 0;
}
