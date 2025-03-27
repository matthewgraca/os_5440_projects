#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <cstring>

using namespace std;

#define WRITE_KEY (1492)
#define READ_KEY (2941)

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

int main(){
  union semun{
    int val;
    struct semid_ds *buf;
    ushort * array;
  } argument;

  argument.val = 0;
  int sem_id = 0;
  int semctl_retval = 0;
  int semop_retval = 0;

  // create write semaphore
  sem_id = semget(WRITE_KEY, 1, 0666 | IPC_CREAT);
  validate_semget(sem_id, "Writer semaphore successfully created.");
  semctl_retval = semctl(sem_id, 0, SETVAL, argument);
  validate_semctl(semctl_retval, "Writer semaphore set to 0.");
  cout << endl;

  // create read semaphore
  sem_id = semget(READ_KEY, 1, 0666 | IPC_CREAT);
  validate_semget(sem_id, "Reader semaphore successfully created.");
  semctl_retval = semctl(sem_id, 0, SETVAL, argument);
  validate_semctl(semctl_retval, "Reader semaphore set to 0.");
}

