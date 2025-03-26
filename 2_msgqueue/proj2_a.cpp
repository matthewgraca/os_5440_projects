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

int main(){
  // create msg queue
  cout << "Creating message queue for process A..." << endl;
  int msgflg = IPC_CREAT | 0666;
  key_t a_key = 1234;
  int a_msqid = msgget(a_key, msgflg);

  // confirm msg queue was created
  if (a_msqid == -1){
    cout << "Message queue for process A could not be created. Exiting..." << endl;
    exit(0);
  }
  else{
    cout << "Message queue for process A created.\n" << endl;
  }

  // log into process B's msg queue
  cout << "Establishing connection to process B..." << endl;
  key_t b_key = 4321;
  while (!msgq_exists(b_key)){
    sleep(1);
    cout << "..." << endl;
  }
  int b_msqid = msgget(b_key, 0666);
  cout << "Connection established.\n" << endl;

  // parent will send, child will receive
  int pid = fork();
  if (pid == 0){ // child (receiver)
    // begin receiving msgs from B
    while (msgq_exists(b_key)){
      msgbuffer rbuf;
      int rcvid = msgrcv(b_msqid, &rbuf, 1024, 1, 0);
      // if process B closed the queue, stop listening
      if (rcvid == -1 && errno == EIDRM){
        cout << "B has left the chat." << endl;
        exit(0);
      }
      // if process A left the chat, stop listening
      else if (!msgq_exists(a_key)){
        exit(0);
      }
      else{
        cout << "B: " << rbuf.messageContent << endl;
      }
    }
  }
  else{ // parent (sender)
    cout << "Ready to begin sending messages to process B; type \"/quit\" to close." << endl;

    bool quit_chat = false;
    while (!quit_chat){
      // create message to send
      msgbuffer sbuf;
      sbuf.mtype = 1;
      string userMsg;
      getline(cin, userMsg);

      // process quit command
      if (userMsg == "/quit"){
        quit_chat = true;
      }
      else{
        // send message
        cout << "A: " << userMsg << endl;
        strcpy(sbuf.messageContent, userMsg.c_str());
        size_t buf_len = sizeof(sbuf.messageContent);
        msgsnd(a_msqid, &sbuf, buf_len, IPC_NOWAIT);
      }
    }

    // destruct queue 
    msgctl(a_msqid, IPC_RMID, NULL);
  }
}
