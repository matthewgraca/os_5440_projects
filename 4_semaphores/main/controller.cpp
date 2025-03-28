#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <cstring>
#include <sys/shm.h>
#include "validators.h"

using namespace std;

int main(){
  union semun{
    int val;
    struct semid_ds *buf;
    ushort * array;
  } argument;

  argument.val = 0;

  // create write semaphore
  cout << "Creating writer semaphore... ";
  int write_sem_id = semget(WRITE_KEY, 1, 0666 | IPC_CREAT);
  validate_semget(write_sem_id);
  cout << "success." << endl;
  cout << "Setting writer semaphore value... ";
  validate_semctl(semctl(write_sem_id, 0, SETVAL, argument));
  cout << "success." << "\n\n";

  // create read semaphore
  cout << "Creating reader semaphore... ";
  int read_sem_id = semget(READ_KEY, 1, 0666 | IPC_CREAT);
  validate_semget(read_sem_id);
  cout << "success." << endl;
  cout << "Setting reader semaphore value... ";
  validate_semctl(semctl(read_sem_id, 0, SETVAL, argument));
  cout << "success." << "\n\n";

  // create shared mem space
  cout << "Creating shared memory space... ";
  int shm_id = shmget(SHM_KEY, SHM_BUF_SIZE, IPC_CREAT | 0666);
  validate_shmget(shm_id);
  cout << "success." << "\n\n";
  cout << "Exiting controller." << endl;
}
