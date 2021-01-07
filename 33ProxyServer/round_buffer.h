#ifndef ROUND_BUFFER_H
#define ROUND_BUFFER_H

#include <sys/types.h> // ssize_t

struct round_buffer {
    size_t offset;
    size_t length;
    size_t capacity;
    void *buffer;
};

int rb_init(struct round_buffer *buf, size_t capacity);
void rb_destroy(const struct round_buffer *buf);

int rb_full(const struct round_buffer *buf);
int rb_empty(const struct round_buffer *buf);

ssize_t write_rb(int fd, struct round_buffer *buf);
ssize_t read_rb(int fd, struct round_buffer *buf);

#endif
