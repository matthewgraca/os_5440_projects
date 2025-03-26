#include <iostream>
#include <cstdlib>
#include <time.h>

using namespace std;

int main(){
  // without time
  long t0 = time(NULL);
  system("ping -c 4 ibm.com");
  long t1 = time(NULL);
  cout << endl << t1 - t0 << " seconds elapsed." << endl;

  cout << endl << "--------------------" << endl << endl;

  // with time
  // time -f "%E" : time command, formatted in seconds
  system("time -f \"%E\" ping -c 4 ibm.com");

  return 0;
}
