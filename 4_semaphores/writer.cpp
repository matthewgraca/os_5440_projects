#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <errno.h>
#include <cstring>
#include <unistd.h>

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
  union semun{
    int val;
    struct semid_ds *buf;
    ushort * array;
  } argument;

  // connect to r/w semaphores
  int write_sem_id = semget(WRITE_KEY, 0, 0666);
  validate_semget(write_sem_id, "Writer semaphore found.");
  int read_sem_id = semget(READ_KEY, 0, 0666);
  validate_semget(read_sem_id, "Reader semaphore found.");
  cout << endl;

  int semop_retval = 0;
  int semctl_retval = 0;
  struct sembuf sops[1];

  // wait for readers and writers to completely exit the space
  cout << "Waiting for all readers and writers to exit the space..." << endl;
  sops[0] = {.sem_num = 0, .sem_op = 0, .sem_flg = 0};
  semop_retval = semop(write_sem_id, sops, 1);
  validate_semop(semop_retval, "Writer semaphore available.");
  semop_retval = semop(read_sem_id, sops, 1);
  validate_semop(semop_retval, "Reader semaphore available.");
  cout << "No reader or writer remaining\n\n";

  // begin writing
  cout << "Preparing to write into the shared memory space..." << endl;
  sops[0] = {.sem_num = 0, .sem_op = 1, .sem_flg = 0};
  semop_retval = semop(write_sem_id, sops, 1);
  validate_semop(semop_retval, "Writer semaphore incremented.");
  cout << endl;

  // write
  cout << "WRITING WOOO" << endl;
  sleep(5);

  // clean up
  cout << "Data written, cleaning up..." << endl;
  sops[0] = {.sem_num = 0, .sem_op = -1, .sem_flg = 0};
  semop_retval = semop(write_sem_id, sops, 1);
  validate_semop(semop_retval, "Writer semaphore decremented.");
  cout << "Writer exiting." << endl;
}
