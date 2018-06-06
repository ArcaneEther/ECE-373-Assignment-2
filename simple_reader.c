/*
  ECE 373
  Instructors PJ Waskiewicz Jr. and Shannon Nelson
  Higgins, Jeremy
  Assignment #2 - Simple Reader
  
  The purpose of this program is to open the module created by
  "char_device.c" and print the value of "syscall_val" found within.
*/



/* Include Required Libraries. */
#include <sys/types.h> /* Required for open() System Call. */
#include <sys/stat.h>  /* Required for open() System Call. */
#include <fcntl.h>     /* Required for open() System Call. */
#include <unistd.h>    /* Required for close(), read(), and write() System Calls. */
#include <stdio.h>     /* Required for printf() Function. */



/* Main function. */
int main(int argc, char *argv[]){
  /* File Descriptor for "/dev/char_device" module. */
  int file_descriptor = 0;
  
  /* Integer Buffer for "syscall_val" integer. */
  int buffer;
  
  /* Open "/dev/char/char_device" module. */
  file_descriptor = open("/dev/char_device", O_RDONLY);
  
  /* Open failed. */
  if(file_descriptor < 0){
    printf("Could not open char_device.\n");
    perror("Error");
    return(-1);
  }
  
  /* Read the syscall_val and print. */
  if(read(file_descriptor, &buffer, sizeof(int)) < 0){
    printf("Could not read char_device.\n");
    return(-1);
  }

  /* Print "syscall_val" integer. */
  printf("Received syscall_val = %d from module.\n", (int)buffer);
  
  /* Close the opened "/dev/char_device" module. */
  close(file_descriptor);
  
  /* Exit Program. */
  return(0);
}
