import java.util.concurrent.*;

class MyThread extends Thread {
  Semaphore pre_semaphore, post_semaphore;

  MyThread(Semaphore pre_semaphore, Semaphore post_semaphore){
    this.pre_semaphore = pre_semaphore;
    this.post_semaphore = post_semaphore;
  }

  public void run() {
    // acquire requires the try/catch block
    try {
       pre_semaphore.acquire();
       System.out.println("Hello " + this.getName());
       sleep(2000);
    } 
    catch (Exception e) {}
    finally {
      System.out.println("Goodbye " + this.getName());
      post_semaphore.release();
    }
  }
}

public class MySemaphoreD {
  public static void main(String args[]) {
    // gives fine-tuned control of the order of which threads get to go
    Semaphore semaphore2 = new Semaphore(1);
    Semaphore semaphore1 = new Semaphore(0);
    Semaphore semaphore0 = new Semaphore(0);

    // thread 2 gets to go first, then gives control to the thread
    // holding semaphore 0 (thread 0), who then gives control to the
    // thread holding semaphore 1 (thread 1), who then gives it back to
    // the thread with semaphore 2
    MyThread mt[] = new MyThread[3];
    mt[2] = new MyThread(semaphore2, semaphore0);
    mt[2].setName("Thread-2");

    mt[1] = new MyThread(semaphore1, semaphore2);
    mt[1].setName("Thread-1");

    mt[0] = new MyThread(semaphore0, semaphore1);
    mt[0].setName("Thread-0");

    for (int i = 0; i < 3; i++){
      mt[i].start();
    }
  }
}
