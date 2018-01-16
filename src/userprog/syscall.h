#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

void syscall_init (void);
void exit (int status);
int write (int fd, const void *buffer, unsigned size);
int filesize (int fd);
bool create (const char *file, unsigned initial_size);
bool remove (const char *file);
int open (const char *file);

#endif /* userprog/syscall.h */
