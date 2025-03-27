#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <cstring>

using namespace std;

#define WRITE_KEY (1492)
#define READ_KEY (2941)

// exits if semaphore cannot be found
void validate_semget(int sem_id, string msg){
  if (sem_id < 0){
    char *err_msg = strerror(errno);
    cout << "Semaphore not found: " << err_msg << endl;
    cout << "Exiting... " << endl;
    exit(0);
  }
  else{
    cout << msg << endl;
  }
}

// prints if sem op was successful or not
void validate_semop(int semop_retval, string msg){
  if (semop_retval == 0){
    cout << msg << endl;
  }
  else{
    char *err_msg = strerror(errno);
    cout << "Semaphore operation was unsuccessful: " << err_msg << endl;
  }
}

int main(){
  // connect to r/w semaphores
  int write_sem_id = semget(WRITE_KEY, 0, 0666);
  validate_semget(write_sem_id, "Writer semaphore found.");
  int read_sem_id = semget(READ_KEY, 0, 0666);
  validate_semget(read_sem_id, "Reader semaphore found.");
  cout << endl;

  int semop_retval = 0;
  int semctl_retval = 0;
  struct sembuf sops[1];

  // wait for write semaphore to open
  cout << "Checking if a writer is in the shared memory space..." << endl;
  sops[0] = {.sem_num = 0, .sem_op = 0, .sem_flg = 0};
  semop_retval = semop(write_sem_id, sops, 1);
  validate_semop(semop_retval, "Writer semaphore is avaiable, no writer remaining.");
  cout << endl;

  // start reading
  cout << "Preparing to read from the shared memory space..." << endl;
  sops[0] = {.sem_num = 0, .sem_op = 1, .sem_flg = 0};
  semop_retval = semop(read_sem_id, sops, 1);
  validate_semop(semop_retval, "Reader semaphore incremented.");
  cout << endl;

  // read
  cout << "READING WOOO" << endl;

  // once reader has finished, clean up
  cout << "Data read, cleaning up..." << endl;
  sops[0] = {.sem_num = 0, .sem_op = -1, .sem_flg = 0};
  semop_retval = semop(read_sem_id, sops, 1);
  validate_semop(semop_retval, "Reader semaphore decremented.");
  cout << "Reader exiting." << endl;
}
