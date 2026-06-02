#ifndef LOG_H
#define LOG_H

void log_write(
    const char *action,
    const char *src,
    const char *dst
);

void log_print(void);

#endif