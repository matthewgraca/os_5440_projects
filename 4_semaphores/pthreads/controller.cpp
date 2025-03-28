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
#include <unistd.h>

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

  // set up scenario tests with pthreads
  pthread_t thread1, thread2;

  // scenario 1: r/r
  cout << "Scenario 1: read/read" << endl;
  pthread_create(&thread1, NULL, Reader, NULL);
  sleep(1); // ensure the first thread executes first
  pthread_create(&thread2, NULL, Reader, NULL);
  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);
  sleep(2);

  // scenario 2: r/w
  cout << "Scenario 2: read/write" << endl;
  pthread_create(&thread1, NULL, Reader, NULL);
  sleep(1); // ensure the first thread executes first
  pthread_create(&thread2, NULL, Writer, NULL);
  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);
  sleep(2);

  // scenario 3: w/r
  cout << "Scenario 3: write/read" << endl;
  pthread_create(&thread1, NULL, Writer, NULL);
  sleep(1); // ensure the first thread executes first
  pthread_create(&thread2, NULL, Reader, NULL);
  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);
  sleep(2);

  // scenario 4: w/w
  cout << "Scenario 4: write/write" << endl;
  pthread_create(&thread1, NULL, Writer, NULL);
  sleep(1); // ensure the first thread executes first
  pthread_create(&thread2, NULL, Writer, NULL);
  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);
  sleep(2);

  // clean up
  shmctl(shm_id, IPC_RMID, 0);
  semctl(write_sem_id, 1, IPC_RMID);
  semctl(read_sem_id, 1, IPC_RMID);
}
