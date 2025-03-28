#include "readwritehelpers.h"
#include <cstring>
#include <iostream>
#include <sys/shm.h>
#include <errno.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <pthread.h>
#include <unistd.h>

using namespace std;

void validate_semctl(int semctl_retval, string msg){
  if(semctl_retval == -1){
    char* err_msg = strerror(errno);
    cout << "Cannot set semaphore value: " << err_msg << endl;
  }
  else{
    cout << msg << endl;
  }
}

// exits if semaphore cannot be created/accessed 
void validate_semget(int sem_id, string msg){
  if (sem_id < 0){
    char *err_msg = strerror(errno);
    cout << "Unable to create semaphore: " << err_msg << endl;
    cout << "Exiting... " << endl;
    exit(0);
  }
  else{
    cout << msg << endl;
  }
}

// prints if semop was successful or not
void validate_semop(int semop_retval, string msg){
  if (semop_retval == 0){
    cout << msg << endl;
  }
  else{
    char *err_msg = strerror(errno);
    cout << "Semaphore operation was unsuccessful: " << err_msg << endl;
  }
}

// prints if shmget was successful or not
void validate_shmget(int shm_retval){
  if (shm_retval != -1){
    cout  << "Successfully created shared memory space with id: "
          << to_string(shm_retval) << ".\n";
  }
  else{
    char *err_msg = strerror(errno);
    cout  << "Error with shared memory: " << err_msg << ", exiting." << endl;
    exit(0);
  }
}

// prints if shmdt was successful or not
void validate_shmdt(int shm_retval){
  if (shm_retval == 0){
    cout << "Successfully detached from shared memory space." << endl;
  }
  else{
    char *err_msg = strerror(errno);
    cout  << "Error with detaching from shared memory: " << err_msg 
          << ", exiting." << endl;
    exit(0);
  }
}

// prints if shmat was successful or not
void validate_shmat(char *shm){
  if (shm != (char *)-1){
    cout << "Successfully attached to shared memory space." << endl;
  }
  else{
    char *err_msg = strerror(errno);
    cout  << "Error with attaching to shared memory: " << err_msg 
          << ", exiting." << endl;
    exit(0);
  }
}

void *Reader(void *arg){
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
  sleep(2);

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

  pthread_exit(NULL);
}

void *Writer(void *arg){
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
  validate_semop(semop_retval, "Successfully incremented writer semaphore.");
  cout << endl;

  // attach to shared mem space
  int shm_id = shmget(SHM_KEY, SHM_BUF_SIZE, 0666);
  validate_shmget(shm_id);
  char *shm = (char *)shmat(shm_id, NULL, 0);
  validate_shmat(shm);

  // write
  cout << "Enter text: ";
  string msg;
  getline(cin, msg);
  strncpy(shm, msg.c_str(), SHM_BUF_SIZE - 1);
  shm[SHM_BUF_SIZE - 1] = '\0';
  cout << "\nData written: " << shm << "\n\n";

  // clean up
  cout << "Data written, cleaning up..." << endl;
  // detach from space 
  int shmdt_retval = shmdt((char *)shm);
  validate_shmdt(shmdt_retval);

  // notify semaphore
  sops[0] = {.sem_num = 0, .sem_op = -1, .sem_flg = 0};
  semop_retval = semop(write_sem_id, sops, 1);
  validate_semop(semop_retval, "Successfully decremented writer semaphore.");
  cout << "Writer exiting." << endl;

  pthread_exit(NULL);
}
