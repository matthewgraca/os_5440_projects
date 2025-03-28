#include "readwritehelpers.h"
#include <cstring>
#include <iostream>

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

void validate_semop(int semop_retval, string msg){
  if (semop_retval == 0){
    cout << msg << endl;
  }
  else{
    char *err_msg = strerror(errno);
    cout << "Semaphore operation was unsuccessful: " << err_msg << endl;
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
