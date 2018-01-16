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
      break;
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
      break;
    case SYS_OPEN:                   /* Open a file. */
      break;
    case SYS_FILESIZE:               /* Obtain a file's size. */
    { 
      f->eax = filesize(stack_pointer+1);
      break;
    }
    case SYS_READ:                   /* Read from a file. */
      break;
    case SYS_WRITE:                  /* Write to a file. */
    {
      f->eax = write(stack_pointer + 1, stack_pointer + 2, stack_pointer + 3);
      break;
    }
    case SYS_SEEK:                   /* Change position in a file. */
      break;
    case SYS_TELL:                   /* Report current position in a file. */
      break;
    case SYS_CLOSE:                  /* Close a file. */
      break;
  }
 // printf ("system call!\n");
 // thread_exit ();
}

void exit (int status)
{
  struct thread* current = thread_current();
  /*returning status to the kernel.
 * If the process's parent waits for it (see below), this is the status that will be returned.
 * Conventionally, a status of 0 indicates success and nonzero values indicate errors.*/
  status = stack_pointer + 1;
  thread_exit();
}

int write (int fd, const void *buffer, unsigned size) 
{
  unsigned temp = size;
  if (fd == 1){ // write to the console
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