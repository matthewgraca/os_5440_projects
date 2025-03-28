#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <cstring>
#include <unistd.h>
#include <sys/shm.h>
#include "readwritehelpers.h"

using namespace std;

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
  validate_semop(semop_retval, "Successfully incremented reader semaphore.");
  cout << endl;

  // attach to shared mem space
  int shm_id = shmget(SHM_KEY, SHM_BUF_SIZE, 0666);
  validate_shmget(shm_id);
  char *shm = (char *)shmat(shm_id, NULL, 0);
  validate_shmat(shm);

  // read
  cout << "\nData read: " << shm << "\n\n";

  // clean up
  cout << "Data read, cleaning up..." << endl;
  // detach from space 
  int shmdt_retval = shmdt((char *)shm);
  validate_shmdt(shmdt_retval);

  // notify semaphore
  sops[0] = {.sem_num = 0, .sem_op = -1, .sem_flg = 0};
  semop_retval = semop(read_sem_id, sops, 1);
  validate_semop(semop_retval, "Successfully decremented reader semaphore.");
  cout << "Reader exiting." << endl;
}
