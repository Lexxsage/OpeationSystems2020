#include "round_buffer.h"

#include <sys/uio.h> // readv, writev
#include <stdlib.h> // malloc

int rb_init(struct round_buffer *buf, size_t capacity) {
    void *ptr = malloc(capacity);
    if (ptr == NULL)
        return -1;

    buf->offset = 0;
    buf->length = 0;
    buf->capacity = capacity;
    buf->buffer = ptr;

    return 0;
}

void rb_destroy(const struct round_buffer *buf) {
    free(buf->buffer);
}

int rb_full(const struct round_buffer *buf) {
    return buf->length == buf->capacity;
}

int rb_empty(const struct round_buffer *buf) {
    return buf->length == 0;
}

// Returns the number of parts (0, 1 or 2)
int make_write_iov(const struct round_buffer *buf, struct iovec *iov) {
    if (buf->length == buf->capacity)
        return 0;

    int cnt = 0;
    size_t part_start = buf->offset + buf->length;
    if (part_start < buf->capacity) {
        iov[cnt].iov_base = buf->buffer + part_start;
        iov[cnt].iov_len = buf->capacity - part_start;
        cnt++;
    }

    if (buf->offset > 0) {
        iov[cnt].iov_base = buf->buffer;
        iov[cnt].iov_len = buf->offset;
        cnt++;
    }

    return cnt;
}

// Returns the number of parts (0, 1 or 2)
int make_read_iov(const struct round_buffer *buf, struct iovec *iov) {
    if (buf->length == 0)
        return 0;

    size_t end = buf->offset + buf->length;
    if (end <= buf->capacity) {
        iov[0].iov_base = buf->buffer + buf->offset;
        iov[0].iov_len = buf->length;
        return 1;
    }

    iov[0].iov_base = buf->buffer + buf->offset;
    iov[0].iov_len = buf->capacity - buf->offset;

    iov[1].iov_base = buf->buffer;
    iov[1].iov_len = end - buf->capacity;
    return 2;
}

ssize_t write_rb(int fd, struct round_buffer *buf) {
    struct iovec iov[2];
    int cnt = make_read_iov(buf, iov);
    if (cnt == 0)
        return -2;

    ssize_t res = writev(fd, iov, cnt);
    if (res != -1) {
        if (res == buf->length) {
            buf->length = 0;
            buf->offset = 0;
        } else {
            buf->length -= res;
            buf->offset = (buf->offset + res) % buf->capacity;
        }
    }

    return res;
}

ssize_t read_rb(int fd, struct round_buffer *buf) {
    struct iovec iov[2];
    int cnt = make_write_iov(buf, iov);
    if (cnt == 0)
        return -2;

    ssize_t res = readv(fd, iov, cnt);
    if (res != -1)
        buf->length += res;

    return res;
}

