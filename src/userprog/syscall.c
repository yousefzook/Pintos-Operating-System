#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "filesys/file.h"
#include "filesys/filesys.h"
#include "threads/synch.h"

uint32_t* stack_pointer = NULL;
struct lock lock ;

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
  /*System calls that return a value can do so by modifying the "eax" member of struct intr_frame. */

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
      checkArg(1);
      exit(*(stack_pointer + 1));
      break;
    }  
    case SYS_EXEC:                   /* Start another process. */
      break;
    case SYS_WAIT:                   /* Wait for a child process to die. */
      break;
    case SYS_CREATE:                 /* Create a file. */
    {
      checkArg(2);
      f->eax = create((const char *)(stack_pointer + 1), (unsigned )(*(stack_pointer + 2)));
      break;
    } 
    case SYS_REMOVE:                 /* Delete a file. */
    {
      checkArg(1);
      f->eax = remove((const char *)(stack_pointer + 1));
      break;
    }
    case SYS_OPEN:                   /* Open a file. */
    {
      checkArg(1);
      f->eax = open((const char *)(stack_pointer + 1));
      break;
    }
    case SYS_FILESIZE:               /* Obtain a file's size. */
    { 
      checkArg(1);
      f->eax = filesize((int)(*(stack_pointer + 1)));
      break;
    }
    case SYS_READ:                   /* Read from a file. */
    {
      checkArg(3);
      f->eax = read((int)(*(stack_pointer + 1)), *(stack_pointer + 2), (unsigned)(*(stack_pointer + 3)));
      break;
    }
    case SYS_WRITE:                  /* Write to a file. */
    {
      checkArg(3);
      f->eax = write((int)(*(stack_pointer + 1)), *(stack_pointer + 2), (unsigned)(*(stack_pointer + 3)));
      break;
    }
    case SYS_SEEK:                   /* Change position in a file. */
    {
      checkArg(2);
      seek((int)(stack_pointer + 1), (unsigned)(*(stack_pointer + 2)));
      break;
    }
    case SYS_TELL:                   /* Report current position in a file. */
    {
      checkArg(1);
      f->eax = tell((int)(*(stack_pointer + 1)));
      break;
    }
    case SYS_CLOSE:                  /* Close a file. */
    {
      close((int)(*(stack_pointer + 1)));
      break;
    }
  }
 // printf ("system call!\n");
 // thread_exit ();
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
  // get file descriptor for file
  struct file *file = NULL;
  if (fd == 1){ // write to the console
    while(size > 100){
      putbuf(buffer, 100);
      size = size - 100;
      buffer = buffer + 100;
    }
    putbuf(buffer, size);
  }else{
    lock_acquire(&lock);
    temp = file_write(file, buffer, size);
    lock_release(&lock);  
  }
  return temp; 
}

/* Returns the size, in bytes, of the file open as fd.*/
int filesize (int fd){
  struct file *file;
  // get file from fd;
  lock_acquire(&lock);
  int ret = (int)file_length(file); 
  lock_release(&lock);
  return ret;  
}

/* Creates a new file called file initially initial_size bytes in size. 
   Returns true if successful, false otherwise. */
bool create (const char *file, unsigned initial_size){
  lock_acquire(&lock);
  bool ret = filesys_create (file, initial_size); 
  lock_release(&lock);
  return ret;
}

/* Deletes the file called file. Returns true if successful, false otherwise.*/
bool remove (const char *file){
  // implement file descriptor which owned by processes already open the file
  lock_acquire(&lock);
  bool ret = filesys_remove (file); 
  lock_release(&lock);
  // struct list_elem *e;
  // struct list *fd_table = &thread_current()->fd_table; 
  // int cnt = 0;
  // for(e = list_begin(fd_table); e != list_end(fd_table);e = list_next(e)){
  //   struct file *f = list_entry(e, struct descriptor, fd_elem)->file;
  //   if(f == file)
  //     break;
  //   cnt++;
  // }
  return ret;
}

/* Opens the file called file. Returns a nonnegative integer handle called 
   a "file descriptor" (fd), or -1 if the file could not be opened.*/
int open (const char *file){
  int fd = -1;
  lock_acquire(&lock);
  struct file *f = filesys_open(file);
  lock_release(&lock);
  if(f != NULL){
    // struct thread *cur_th = thread_current();
    // struct descriptor *d;
    // d->file = f;
    // d->fd = list_size(&cur_th->fd_table)+2;
    // list_push_back(&cur_th->fd_table, &d->fd_elem);
    // fd = d->fd;
  }
  return fd;
}

/*Reads size bytes from the file open as fd into buffer. 
Returns the number of bytes actually read */
int read (int fd, void *buffer, unsigned size)
{
  if(fd == 0)
  {
      char *line = (char*) buffer;
      unsigned  i;
    for (i = 0; i < size; i++) // check valid?
    {
      line[i] = input_getc();
    }
        return i;
  }
  struct file *f = NULL; // get file, locks
  lock_acquire(&lock);
  int ret = file_read(f, buffer, size);
  lock_release(&lock);
  return ret;
}

/*Changes the next byte to be read or written in open file fd to position */
void seek (int fd, unsigned position)
{
   struct file *f = NULL; //get file, locks
   lock_acquire(&lock);
   file_seek(f, position);
   lock_release(&lock);
}

/*Returns the position of the next byte to be read or written in open file fd */
unsigned tell (int fd)
{
   struct file *f = NULL; //get file;
   lock_acquire(&lock);
   unsigned ret = file_tell(f);
   lock_release(&lock);
   return ret;
}

/*Closes file descriptor fd */
void close (int fd)
{
   struct file *f = NULL; //get file;
   lock_acquire(&lock);
   file_close (f);
   lock_release(&lock);
}


// chdck>> opening a null file
/* Check arguments of syscall, terminate if invalid*/
void checkArg(int argc){
  int i;
  for (i = 1; i <= argc; i++){
    if(stack_pointer+i == NULL)
      thread_exit();
  }
}
// fd, CHECK FILE EXISTENCE,...