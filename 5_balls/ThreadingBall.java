import java.awt.*;
import javax.swing.*;
import java.util.concurrent.*;

class MyBall extends Thread{
  int x;
  int y;
  int sleeptime;
  int str_index;
  MyBallWindow mBw;
  Color c;
  String str, substr;
  boolean goingDown;
  Semaphore pre_semaphore, post_semaphore;
  int fallDist;
  int yPrev;

  public MyBall(
      int xx, 
      int yy, 
      int sleepTime, 
      MyBallWindow ssbb, 
      Color cc,
      Semaphore pre_semaphore,
      Semaphore post_semaphore
  ){
    str = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    str_index = 0;
    substr = "A";
    goingDown = true;
    fallDist = 50;
    x = xx;
    y = yy;
    sleeptime = sleepTime;
    mBw = ssbb;
    c = cc;
    this.pre_semaphore = pre_semaphore;
    this.post_semaphore = post_semaphore;
  }

  public void run(){
    while (true){
      try{
        // lock semaphore
        pre_semaphore.acquire();
        Thread.sleep(sleeptime);

        // paint ball
        mBw.repaint();

        // update next string
        substr = str.substring(str_index, str_index+1);
        str_index++;
        if (str_index >= str.length()) str_index = 0;
        // update ball's next y coordinate
        yPrev = y;
        y = goingDown ? y + fallDist : y - fallDist;
        if (y > 550) goingDown = false;
        if (y <= 0) goingDown = true;
      } catch (Exception e) {}
      finally {
        post_semaphore.release();
      }
    }
  }

  public void draw(Graphics g) {
    g.setColor(Color.white);
    g.fillOval(x, yPrev, 50, 50);
    g.setFont(new Font("Rome", Font.BOLD, 50));
    g.drawString(substr, x, y+30);

    g.setColor(c);
    g.fillOval(x, y, 50, 50);

    g.setColor(Color.black);      
    g.setFont(new Font("Monospaced", Font.BOLD, 50));
    g.drawString(substr, x+10, y+40);
  }
}

class MyBallWindow extends JFrame{
  MyBall myBall[];

  public MyBallWindow(){
    Semaphore sem1 = new Semaphore(2);
    Semaphore sem2 = new Semaphore(0);
    Semaphore sem3 = new Semaphore(0);

    /*
    Semaphore sem4 = new Semaphore(0);
    Semaphore sem5 = new Semaphore(0);
    Semaphore sem6 = new Semaphore(0);
    */

    int sleepTime = 500;  // any faster, and the balls sometime visually drops together
    myBall = new MyBall[6];

    // original ball order
    /*
    myBall[0] = new MyBall(100, 0, 300, this, Color.red, sem6, sem1);
    myBall[1] = new MyBall(200, 0, 300, this, Color.blue, sem3, sem4);
    myBall[2] = new MyBall(300, 0, 300, this, Color.yellow, sem4, sem5); 
    myBall[3] = new MyBall(400, 0, 300, this, Color.green, sem2, sem3);
    myBall[4] = new MyBall(500, 0, 300, this, Color.orange, sem5, sem6);
    myBall[5] = new MyBall(600, 0, 300, this, Color.pink, sem1, sem2);
    */

    // project 5 ball order
    myBall[0] = new MyBall(100, 0, sleepTime, this, Color.pink, sem1, sem2);
    myBall[1] = new MyBall(200, 0, sleepTime, this, Color.green, sem1, sem2);
    myBall[2] = new MyBall(300, 0, sleepTime, this, Color.blue, sem2, sem3); 
    myBall[3] = new MyBall(400, 0, sleepTime, this, Color.yellow, sem2, sem3);
    myBall[4] = new MyBall(500, 0, sleepTime, this, Color.orange, sem3, sem1);
    myBall[5] = new MyBall(600, 0, sleepTime, this, Color.red, sem3, sem1);

    for (int i = 0; i < 6; i++){ 
      myBall[i].start();   
    }

    setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
    setSize(800, 800);
    setVisible(true);
  }   

  public void paint(Graphics g){
    for (int i = 0; i < 6; i++){
      myBall[i].draw(g);
    }
  }
}

public class ThreadingBall {
  public static void main(String[] args) {
    new MyBallWindow();
  }
}
