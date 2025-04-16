import java.awt.*;
import javax.swing.*;

class MyBall extends Thread{
  int x;
  int y;
  int sleeptime;
  int str_index;
  MyBallWindow mBw;
  Color c;
  String str, substr;
  boolean goingDown;

  public MyBall(int xx, int yy, int sleepTime, MyBallWindow ssbb, Color cc){
    str = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    str_index = 0;
    substr = "A";
    goingDown = true;
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
      if (str_index >= str.length()) str_index = 0;

      if (goingDown){
        y += 10;
        if (y > 550) goingDown = false;
      }
      else{
        y -= 10;
        if (y <= 0) goingDown = true;
      }

      try{
        Thread.sleep(sleeptime);
      } catch (Exception e){}
    }
  }

  public void draw(Graphics g) {
    g.setColor(Color.white);
    if (goingDown) g.fillOval(x, y-10, 50, 50);
    else g.fillOval(x, y+10, 50, 50);
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
    myBall[1] = new MyBall(200, 0, 300, this, Color.blue);
    myBall[2] = new MyBall(300, 0, 300, this, Color.yellow); 
    myBall[3] = new MyBall(400, 0, 300, this, Color.green);
    myBall[4] = new MyBall(500, 0, 300, this, Color.orange);
    myBall[5] = new MyBall(600, 0, 300, this, Color.pink);

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
