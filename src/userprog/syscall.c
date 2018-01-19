#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "filesys/file.h"
#include "filesys/filesys.h"
#include "threads/synch.h"
#include "threads/vaddr.h"

uint32_t* stack_pointer = NULL;
static struct lock lock;

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
  lock_init(&lock);
}

static void
syscall_handler (struct intr_frame *f) 
{
  stack_pointer = f->esp;

  if(is_user_vaddr(stack_pointer))
    exit(-1);

  int syscall_number = (int) *stack_pointer;
  switch(syscall_number)
  { 
    case SYS_HALT:                   /* Halt the operating system. */
    {
      halt();
      break;
    }
    case SYS_EXIT:                   /* Terminate this process. */
    {
      checkArgs(1);
      exit(*(stack_pointer + 1));
      break;
    }  
    case SYS_EXEC:                   /* Start another process. */
    {
      break;
    }
    case SYS_WAIT:                   /* Wait for a child process to die. */
    {
      break;
    }
    case SYS_CREATE:                 /* Create a file. */
    {
      checkArgs(2);
      f->eax = create((const char *)(stack_pointer + 1), (unsigned )(*(stack_pointer + 2)));
      break;
    } 
    case SYS_REMOVE:                 /* Delete a file. */
    {
      checkArgs(1);
      f->eax = remove((const char *)(stack_pointer + 1));
      break;
    }
    case SYS_OPEN:                   /* Open a file. */
    {
      checkArgs(1);
      f->eax = open((const char *)(stack_pointer + 1));
      break;
    }
    case SYS_FILESIZE:               /* Obtain a file's size. */
    { 
      checkArgs(1);
      f->eax = filesize((int)(*(stack_pointer + 1)));
      break;
    }
    case SYS_READ:                   /* Read from a file. */
    {
      checkArgs(3);
      f->eax = read((int)(*(stack_pointer + 1)), *(stack_pointer + 2), (unsigned)(*(stack_pointer + 3)));
      break;
    }
    case SYS_WRITE:                  /* Write to a file. */
    {
      checkArgs(3);
      f->eax = write((int)(*(stack_pointer + 1)), *(stack_pointer + 2), (unsigned)(*(stack_pointer + 3)));
      break;
    }
    case SYS_SEEK:                   /* Change position in a file. */
    {
      checkArgs(2);
      seek((int)(stack_pointer + 1), (unsigned)(*(stack_pointer + 2)));
      break;
    }
    case SYS_TELL:                   /* Report current position in a file. */
    {
      checkArgs(1);
      f->eax = tell((int)(*(stack_pointer + 1)));
      break;
    }
    case SYS_CLOSE:                  /* Close a file. */
    {
      checkArgs(1);
      close((int)(*(stack_pointer + 1)));
      break;
    }
  }
}

/*Terminates Pintos */
void halt (void)
{
  shutdown_power_off();
}

/*Terminates the current user program, returning status to the kernel */
void exit (int status)
{
  struct thread *t = thread_current();
  printf ("%s: exit(%d)\n",t->name,status);
  thread_exit();
}

/*Writes size bytes from buffer to the open file fd. 
Returns the number of bytes actually written*/
int write (int fd, const void *buffer, unsigned size) 
{
  unsigned temp = size;
  // if(file == NULL) thread_exit();
  lock_acquire(&lock);
  
  if (!is_user_vaddr (buffer) || !is_user_vaddr(buffer+size)){
    lock_release (&lock);
    exit (-1);
  }

  if (fd == 1){ // write to the console
    while(size > 100){
      putbuf(buffer, 100);
      size = size - 100;
      buffer = buffer + 100;
    }
    putbuf(buffer, size);
  }else if(fd == 0){
    lock_release(&lock);  
    return 0;
  }else{
    struct file *file = get_file(fd);
    temp = file_write(file, buffer, size);
  }
  lock_release(&lock);  
  return temp; 
}

/* Returns the size, in bytes, of the file open as fd.*/
int filesize (int fd){
  struct file *file = get_file(fd);
  if(file == NULL) thread_exit();
  lock_acquire(&lock);
  int ret = (int)file_length(file); 
  lock_release(&lock);
  return ret;  
}

/* Creates a new file called file initially initial_size bytes in size. 
   Returns true if successful, false otherwise. */
bool create (const char *file, unsigned initial_size){ 
  
  if(file == NULL)
    return false;

  if (!is_user_vaddr(file))
    exit(-1);

  lock_acquire(&lock);
  bool ret = filesys_create (file, initial_size);
  lock_release(&lock);
  return ret;
}

/* Deletes the file called file. Returns true if successful, false otherwise.*/
bool remove (const char *file){
  // implement file descriptor which owned by processes already open the file
  
  if (!is_user_vaddr(file))
    exit(-1);

  lock_acquire(&lock);
  bool ret = filesys_remove (file); 
  lock_release(&lock);
  return ret;
}

/* Opens the file called file. Returns a nonnegative integer handle called 
   a "file descriptor" (fd), or -1 if the file could not be opened.*/
int open (const char *file){
  
  int fd = -1;
  
  if(file != NULL){

    /* If not valid address, terminate the process*/
    if(! is_user_vaddr(file))
      exit(-1);

    lock_acquire(&lock);
    struct file *f = filesys_open(file);
    lock_release(&lock);

    struct thread *cur_th = thread_current();
    struct descriptor *d;
    d->file = f;
    d->fd = cur_th->fileNumber++;
    list_push_back(&cur_th->fd_table, &d->fd_elem);
    fd = d->fd;
  }
  return fd;
}

/*Reads size bytes from the file open as fd into buffer. 
Returns the number of bytes actually read */
int read (int fd, void *buffer, unsigned size)
{

  lock_acquire(&lock);

  if(buffer == NULL)
    return 0;

  if (!is_user_vaddr(buffer))
    exit(-1);

  if(fd == 0)
  {
    char *line = (char*) buffer;
    unsigned  i;
    for (i = 0; i < size; i++)
      line[i] = input_getc();
    return i;
  }
  struct file *f = get_file(fd);
  if(f == NULL) thread_exit();
  int ret = file_read(f, buffer, size);
  lock_release(&lock);
  return ret;
}

/*Changes the next byte to be read or written in open file fd to position */
void seek (int fd, unsigned position)
{
   lock_acquire(&lock);
   struct file *f = get_file(fd);
   if(f == NULL) thread_exit();
   file_seek(f, position);
   lock_release(&lock);
}

/*Returns the position of the next byte to be read or written in open file fd */
unsigned tell (int fd)
{
   struct file *f = get_file(fd);
   if(f == NULL) thread_exit();
   lock_acquire(&lock);
   unsigned ret = file_tell(f);
   lock_release(&lock);
   return ret;
}

/*Closes file descriptor fd */
void close (int fd)
{
   struct file *f = get_file(fd);
   if(f == NULL) thread_exit();
   lock_acquire(&lock);
   file_close (f);
   lock_release(&lock);
   removeFromList(fd);
}

/*Runs the executable whose name is given in cmd_line */
pid_t exec (const char *cmd_line)
{

}

/*Waits for a child process pid and retrieves the child's exit status */
int wait (pid_t pid)
{
    return process_wait(pid);
}

/* Check arguments of syscall, terminate if invalid*/
void checkArgs(int argc){
  int i;
  for (i = 1; i <= argc; i++){
    if(stack_pointer + i == NULL)
      thread_exit();
  }
}

/*get file from fd */
struct file * get_file(int fd)
{
   struct list_elem *e;
   struct list *fd_table = &thread_current()->fd_table; 
   for(e = list_begin(fd_table); e != list_end(fd_table); e = list_next(e)){
     struct descriptor *f_descriptor = list_entry(e, struct descriptor, fd_elem);
     if(f_descriptor->fd == fd)
        return f_descriptor->file;
  }
  return NULL;  
}

void removeFromList(int fd)
{
   struct list_elem *e;
   struct list *fd_table = &thread_current()->fd_table; 
   for(e = list_begin(fd_table); e != list_end(fd_table); e = list_next(e)){
     struct descriptor *f_descriptor = list_entry(e, struct descriptor, fd_elem);
     if(f_descriptor->fd == fd){
             list_remove(e);
             return;
     }
   }       
}