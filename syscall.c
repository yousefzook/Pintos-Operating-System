#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "filesys/file.h"
#include "filesys/filesys.h"

uint32_t* stack_pointer = null;

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
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
      exit(stack_pointer + 1);
      break;
    }  
    case SYS_EXEC:                   /* Start another process. */
	    break;
    case SYS_WAIT:                   /* Wait for a child process to die. */
	    break;
    case SYS_CREATE:                 /* Create a file. */
	  {
      f->eax = create(stack_pointer+1, stack_pointer+2);
      break;
    } 
    case SYS_REMOVE:                 /* Delete a file. */
    {
      f->eax = remove(stack_pointer + 1);
      break;
    }
    case SYS_OPEN:                   /* Open a file. */
    {
      f->eax = open(stack_pointer + 1);
      break;
    }
    case SYS_FILESIZE:               /* Obtain a file's size. */
    { 
      f->eax = filesize(stack_pointer + 1);
      break;
    }
    case SYS_READ:                   /* Read from a file. */
    {
      f->eax = read(stack_pointer + 1, stack_pointer + 2, stack_pointer + 3);
      break;
    }
    case SYS_WRITE:                  /* Write to a file. */
    {
      f->eax = write(stack_pointer + 1, stack_pointer + 2, stack_pointer + 3);
      break;
    }
    case SYS_SEEK:                   /* Change position in a file. */
    {
      seek(stack_pointer + 1, stack_pointer + 2);
      break;
    }
    case SYS_TELL:                   /* Report current position in a file. */
    {
      f->eax = tell(stack_pointer + 1);
      break;
    }
    case SYS_CLOSE:                  /* Close a file. */
    {
      close(stack_pointer + 1);
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
  struct thread* current = thread_current();
  status = stack_pointer + 1;   // ??????????????
  thread_exit();
}

/*Writes size bytes from buffer to the open file fd. 
Returns the number of bytes actually written*/
int write (int fd, const void *buffer, unsigned size) 
{
  unsigned temp = size;
  if (fd == 1){ // write to the console, locks
    while(size > 100){
      putbuf(buffer, 100);
      size = size - 100;
      buffer = buffer + 100;
    }
    putbuf(buffer, size);
    return temp;
  } 
}

/* Returns the size, in bytes, of the file open as fd.*/
int filesize (int fd){
  struct file *file;
  // get file from fd;
  return (int)file_length(file);  
}

/* Creates a new file called file initially initial_size bytes in size. 
   Returns true if successful, false otherwise. */
bool create (const char *file, unsigned initial_size){
  return filesys_create (file, initial_size);
}

/* Deletes the file called file. Returns true if successful, false otherwise.*/
bool remove (const char *file){
  // implement file descriptor which owned by processes already open the file
  return filesys_remove (file);  
}

/* Opens the file called file. Returns a nonnegative integer handle called 
   a "file descriptor" (fd), or -1 if the file could not be opened.*/
int open (const char *file){
  int fd = -1;
  struct file *f = filesys_open(file);
  if(f != null){
    // give it an fd
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
      for (i = 0; i < size; i++) // check valid?
	{
	  line[i] = input_getc();
	}
        return i;
  }
  struct file *f = null; // get file, locks
  return file_read(f, buffer, size);
}

/*Changes the next byte to be read or written in open file fd to position */
void seek (int fd, unsigned position)
{
   struct file *f = null; //get file, locks
   file_seek(f, position);
}

/*Returns the position of the next byte to be read or written in open file fd */
unsigned tell (int fd)
{
   struct file *f = null; //get file;
   return file_tell(f);
}

/*Closes file descriptor fd */
void close (int fd)
{
   struct file *f = null; //get file;
   file_close (f);
}




//CHECK ARGUMENTS, LOCKS, CHECK FILE EXISTENCE,...











