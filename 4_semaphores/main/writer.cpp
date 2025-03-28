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
#include <sys/shm.h>
#include "validators.h"

using namespace std;

int main(){
  union semun{
    int val;
    struct semid_ds *buf;
    ushort * array;
  } argument;

  int semop_retval = 0;
  int semctl_retval = 0;
  struct sembuf sops[1];

  // connect to r/w semaphores
  cout << "Connecting to read/write semaphores... ";
  int write_sem_id = semget(WRITE_KEY, 0, 0666);
  validate_semget(write_sem_id);
  int read_sem_id = semget(READ_KEY, 0, 0666);
  validate_semget(read_sem_id);
  cout << "success." << "\n";

  // wait for readers and writers to completely exit the space
  cout  << "Waiting for all reader and writer semaphores to signal that the "
        << "shared memory space is open." << endl;;
  cout << "Waiting for writer semaphore... ";
  sops[0] = {.sem_num = 0, .sem_op = 0, .sem_flg = 0};
  validate_semop(semop(write_sem_id, sops, 1));
  cout << "success." << "\n";
  cout << "Waiting for reader semaphores... ";
  validate_semop(semop(read_sem_id, sops, 1));
  cout << "success." << "\n";
  cout << "No readers or writers remaining\n\n";

  // begin writing
  cout << "Preparing to write into the shared memory space." << endl;
  cout << "Incrementing writer semaphore... ";
  sops[0] = {.sem_num = 0, .sem_op = 1, .sem_flg = 0};
  validate_semop(semop(write_sem_id, sops, 1));
  cout << "success." << "\n\n";

  // attach to shared mem space
  cout << "Attaching to shared memory space... ";
  int shm_id = shmget(SHM_KEY, SHM_BUF_SIZE, 0666);
  validate_shmget(shm_id);
  char *shm = (char *)shmat(shm_id, NULL, 0);
  validate_shmat(shm);
  cout << "success." << "\n\n";

  // write
  cout << "Enter text: ";
  string msg;
  getline(cin, msg);
  strncpy(shm, msg.c_str(), SHM_BUF_SIZE - 1);
  shm[SHM_BUF_SIZE - 1] = '\0';
  cout << "\nData written: " << shm << "\n\n";

  // clean up
  cout << "Cleaning up..." << endl;
  // detach from space 
  cout << "Detaching from shared memory space... ";
  validate_shmdt(shmdt((char *)shm));
  cout << "success." << "\n";

  // notify semaphore
  cout << "Decrementing writer semaphore... ";
  sops[0] = {.sem_num = 0, .sem_op = -1, .sem_flg = 0};
  validate_semop(semop(write_sem_id, sops, 1));
  cout << "success." << "\n\n";
  cout << "Writer exiting." << endl;
}
