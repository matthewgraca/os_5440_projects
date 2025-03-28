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
#include "validators.h"

using namespace std;

int main(){
  struct sembuf sops[1];

  // connect to r/w semaphores
  cout << "Connecting to read/write semaphores... ";
  int write_sem_id = semget(WRITE_KEY, 0, 0666);
  validate_semget(write_sem_id);
  int read_sem_id = semget(READ_KEY, 0, 0666);
  validate_semget(read_sem_id);
  cout << "success." << "\n";

  // wait for write semaphore to open
  cout  << "Waiting for writer semaphore to signal that all writers are no " 
        << "longer in the shared memory space... ";
  sops[0] = {.sem_num = 0, .sem_op = 0, .sem_flg = 0};
  validate_semop(semop(write_sem_id, sops, 1));
  cout << "success." << "\n";

  // start reading
  cout << "Incrementing reader semaphore... ";
  sops[0] = {.sem_num = 0, .sem_op = 1, .sem_flg = 0};
  validate_semop(semop(read_sem_id, sops, 1));
  cout << "success." << "\n";

  // attach to shared mem space
  cout << "Attaching to shared memory space... ";
  int shm_id = shmget(SHM_KEY, SHM_BUF_SIZE, 0666);
  validate_shmget(shm_id);
  char *shm = (char *)shmat(shm_id, NULL, 0);
  validate_shmat(shm);
  cout << "success." << "\n\n";

  // read
  cout << "Data read: " << shm << "\n\n";
  sleep(3);

  // clean up
  cout << "Cleaning up..." << endl;
  // detach from space 
  cout << "Detaching from shared memory space... ";
  validate_shmdt(shmdt((char *)shm));
  cout << "success." << "\n";

  // notify semaphore
  cout << "Decrementing reader semaphore... ";
  sops[0] = {.sem_num = 0, .sem_op = -1, .sem_flg = 0};
  validate_semop(semop(read_sem_id, sops, 1));
  cout << "success." << "\n\n";
  cout << "Reader exiting." << endl;
}
