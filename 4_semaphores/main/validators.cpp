#include "validators.h"
#include <cstring>
#include <iostream>

using namespace std;

// prints error and exits if semaphore control operation failed
void validate_semctl(int semctl_retval){
  if(semctl_retval == -1){
    char* err_msg = strerror(errno);
    cout  << "Failed to run semaphore control operation: " << err_msg
          << ". Exiting..." << endl;
    exit(1);
  }
}

// prints error and exits if semaphore cannot be created/found
void validate_semget(int sem_id){
  if (sem_id == -1){
    char *err_msg = strerror(errno);
    cout  << "Unable to create/find semaphore: " << err_msg
          << ". Exiting... " << endl;
    exit(1);
  }
}

// prints error and exits if semaphore operation failed
void validate_semop(int semop_retval){
  if (semop_retval != 0){
    char *err_msg = strerror(errno);
    cout  << "Semaphore operation was unsuccessful: " << err_msg 
          << ". Exiting..." << endl;
    exit(1);
  }
}

// prints error and exits if shared memory space cannot be found/created
void validate_shmget(int shm_retval){
  if (shm_retval == -1){
    char *err_msg = strerror(errno);
    cout  << "Error with finding/creating shared memory: " << err_msg 
          << ". Exiting..." << endl;
    exit(1);
  }
}

// prints error and exits if can't detach from shared memory space
void validate_shmdt(int shm_retval){
  if (shm_retval != 0){
    char *err_msg = strerror(errno);
    cout  << "Error with detaching from shared memory: " << err_msg 
          << ". Exiting..." << endl;
    exit(1);
  }
}

// prints error and exits if can't attach to shared memory space
void validate_shmat(char *shm){
  if (shm == (char *)-1){
    char *err_msg = strerror(errno);
    cout  << "Error with attaching to shared memory: " << err_msg 
          << ". Exiting..." << endl;
    exit(1);
  }
}
