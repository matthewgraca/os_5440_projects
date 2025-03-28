#ifndef VALIDATORS_H 
#define VALIDATORS_H 

#include <string>
using namespace std;

#define WRITE_KEY (1492)
#define READ_KEY (2941)
#define SHM_KEY (1234)
#define SHM_BUF_SIZE (1024)

void validate_semget(int sem_id);
void validate_semctl(int semctl_retval);
void validate_semop(int semop_retval);
void validate_shmget(int shm_retval);
void validate_shmdt(int shm_retval);
void validate_shmat(char *shm);

#endif
