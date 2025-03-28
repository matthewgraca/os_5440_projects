#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <cstring>
#include <sys/shm.h>
#include "readwritehelpers.h"

using namespace std;

int main(){
  union semun{
    int val;
    struct semid_ds *buf;
    ushort * array;
  } argument;

  argument.val = 0;
  int semctl_retval = 0;
  int semop_retval = 0;
  string success_msg = "";

  // create write semaphore
  int write_sem_id = semget(WRITE_KEY, 1, 0666 | IPC_CREAT);
  validate_semget(
      write_sem_id, 
      "Writer semaphore successfully created with id: " +
      to_string(write_sem_id)
  );
  semctl_retval = semctl(write_sem_id, 0, SETVAL, argument);
  validate_semctl(semctl_retval, "Writer semaphore set to 0.");
  cout << endl;

  // create read semaphore
  int read_sem_id = semget(READ_KEY, 1, 0666 | IPC_CREAT);
  validate_semget(
      read_sem_id, 
      "Reader semaphore successfully created with id: " + to_string(read_sem_id)
  );
  semctl_retval = semctl(read_sem_id, 0, SETVAL, argument);
  validate_semctl(semctl_retval, "Reader semaphore set to 0.");
  cout << endl;

  // create shared mem space
  int shm_id = shmget(SHM_KEY, SHM_BUF_SIZE, IPC_CREAT | 0666);
  validate_shmget(shm_id);
}

