import java.awt.*;
import javax.swing.*;

class EmptyBall extends MyBall{
  public EmptyBall(int xx, int yy, int sleepTime, MyBallWindow ssbb, Color cc){
    super(xx, yy, sleepTime, ssbb, cc);
    str = "                                                             ";
    str_index = 0;
    substr = " ";
    x = xx;
    y = yy;
    sleeptime = sleepTime;
    mBw = ssbb;
    c = cc;
  }

  public void run(){
    super.run();
  }
}

class MyBall extends Thread{
  int x;
  int y;
  int sleeptime;
  int str_index;
  MyBallWindow mBw;
  Color c;
  String str, substr;

  public MyBall(int xx, int yy, int sleepTime, MyBallWindow ssbb, Color cc){
    str = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    str_index = 0;
    substr = "A";
    x = xx;
    y = yy;
    sleeptime = sleepTime;
    mBw = ssbb;
    c = cc;
  }

  public void run(){
    while (true){
      mBw.repaint();
      substr = str.substring(str_index, str_index+1);
      str_index++;
      if (str_index > 61) str_index = 0;
      y = y + 10;
      if (y > 550) y = 0;
      try{
        Thread.sleep(sleeptime);
      }
      catch (Exception e){
      }
    }
  }

  public void draw(Graphics g) {
    g.setColor(Color.white);
    g.fillOval(x, y-10, 50, 50);
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
    myBall = new MyBall[6];

    myBall[0] = new MyBall(100, 0, 300, this, Color.red);
    myBall[1] = new MyBall(200, 0, 400, this, Color.blue);
    myBall[2] = new MyBall(300, 0, 500, this, Color.yellow); 
    myBall[3] = new MyBall(400, 0, 600, this, Color.green);
    myBall[4] = new MyBall(500, 0, 700, this, Color.orange);
    //myBall[5] = new MyBall(600, 0, 800, this, Color.pink);
    myBall[5] = new EmptyBall(600, 0, 800, this, Color.pink);

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
