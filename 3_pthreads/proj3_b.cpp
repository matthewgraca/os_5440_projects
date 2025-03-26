#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>

using namespace std;

struct /*class*/ msgbuffer{
  public:
    long mtype;
    char messageContent[1024];
};

// checks if the queue exists
bool msgq_exists(key_t key){
  int msqid = msgget(key, 0666);
  if (msqid == -1 && errno == ENOENT){
    return false;
  }
  else{
    return true;
  }
}

struct receiver_args{
  public:
    key_t a_key;
    int a_msqid;
    key_t b_key;
};

struct sender_args{
  public:
    int b_msqid;
    int a_msqid;
    key_t a_key;
};

void *receiver(void *thread_args){
  struct receiver_args *rcv_args = (struct receiver_args *) thread_args;

  // begin receiving msgs from A
  bool q_open = true;
  while (q_open){
    msgbuffer rbuf;
    int rcvid = msgrcv(rcv_args->a_msqid, &rbuf, 1024, 1, 0);
    
    // if A or B has left the chat, stop listening
    if (!msgq_exists(rcv_args->b_key) || !msgq_exists(rcv_args->a_key)){
      q_open = false;
    }
    else{
      cout << "A: " << rbuf.messageContent << endl;
    }
  }

  pthread_exit(NULL);
}

void *sender(void *thread_args){
  struct sender_args *snd_args= (struct sender_args *) thread_args;
  cout << "Ready to begin sending messages to process A; type \"/quit\" to close." << endl;

  bool q_open = true;
  while (q_open){
    // create message to send
    msgbuffer sbuf;
    sbuf.mtype = 1;
    string userMsg;
    getline(cin, userMsg);

    // if A left the chat, exit
    if (!msgq_exists(snd_args->a_key)){
      cout << "A has left the chat, closing chatroom." << endl;
      q_open = false;
    }
    else{
      // send msg
      strcpy(sbuf.messageContent, userMsg.c_str());
      size_t buf_len = sizeof(sbuf.messageContent);
      msgsnd(snd_args->b_msqid, &sbuf, buf_len, IPC_NOWAIT);

      // process quit command
      if (userMsg == "/quit"){
        cout << "Leaving chatroom." << endl;
        q_open = false;
      }
      else{
        cout << "B: " << userMsg << endl;
      }
    }
  }

  // destruct queues 
  msgctl(snd_args->b_msqid, IPC_RMID, NULL);
  msgctl(snd_args->a_msqid, IPC_RMID, NULL);

  pthread_exit(NULL);
}

bool thread_success(int ret_val, string name){
  if (ret_val){
    cout << name << " thread could not be created. Exiting with error: ";

    char buffer[256];
    char * errorMsg = strerror_r(errno, buffer, 256); 
    printf("%s\n", errorMsg); 

    return false;
  }
  else{
    return true;
  }
}

int main(){
  // create msg queue
  cout << "Creating message queue for process B..." << endl;
  int msgflg = IPC_CREAT | 0666;
  key_t b_key = 4321;
  int b_msqid = msgget(b_key, msgflg);

  // confirm msg queue was created
  if (b_msqid == -1){
    cout << "Message queue for process B could not be created. Exiting..." << endl;
    exit(0);
  }
  else{
    cout << "Message queue for process B created.\n" << endl;
  }

  // log into process A's msg queue
  cout << "Establishing connection to process A..." << endl;
  key_t a_key = 1234;
  while (!msgq_exists(a_key)){
    sleep(1);
    cout << "..." << endl;
  }
  int a_msqid = msgget(a_key, 0666);
  cout << "Connection established.\n" << endl;

  // create a receiver and sender thread
  pthread_t receiver_thread, sender_thread;
  struct receiver_args rcv_args = {a_key, a_msqid, b_key};
  struct sender_args snd_args = {b_msqid, a_msqid, a_key};

  int r_ret = pthread_create(&receiver_thread, NULL, receiver, (void*) &rcv_args);
  int s_ret = pthread_create(&sender_thread, NULL, sender, (void*) &snd_args);

  // send msg and exit if thread failed to create
  thread_success(r_ret, "Receiver");
  thread_success(s_ret, "Sender");

  // return threads to main
  pthread_join(receiver_thread, NULL);
  pthread_join(sender_thread, NULL);

  return 0;
}
