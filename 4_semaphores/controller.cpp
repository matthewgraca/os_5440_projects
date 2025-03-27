#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <cstring>
#include <sys/shm.h>

using namespace std;

#define WRITE_KEY (1492)
#define READ_KEY (2941)
#define SHM_KEY (1234)
#define SHM_BUF_SIZE (1024)

void validate_semctl(int semctl_retval, string msg){
  if(semctl_retval == -1){
    char* err_msg = strerror(errno);
    cout << "Cannot set semaphore value: " << err_msg << endl;
  }
  else{
    cout << msg << endl;
  }
}

// exits if semaphore cannot be created 
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

