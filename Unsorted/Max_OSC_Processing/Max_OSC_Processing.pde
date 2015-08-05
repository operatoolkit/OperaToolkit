import oscP5.*;
import netP5.*;

float time1, time2;
float dt1, dt2;
float val1, val2;

ArrayList<Integer> notes;

OscP5 oscP5;
NetAddress myRemoteLocation;

void setup() {
  size(400,400);
  frameRate(25);

  oscP5 = new OscP5(this,12000);
  myRemoteLocation = new NetAddress("127.0.0.1",12000);
  
  notes = new ArrayList<Integer>();
  notes.add(60);
  notes.add(62);
  notes.add(64);
  notes.add(65);
  notes.add(67);
  notes.add(69);
  notes.add(71);
  notes.add(72);
  
  dt1 = 1.0;
  dt2 = 1.0;
}

void draw() {
  background(0);  
  
  
  ArrayList<Integer> sampledNotes = new ArrayList<Integer>();
  
  int numTimes = (int) val2;
  for (int i=0; i<numTimes; i++) {
    sampledNotes.add(notes.get((int) random(notes.size())));
  }
  
  println(sampledNotes);
  
  
  time1 += dt1;
  time2 += dt2;
  
  float x = map(time1, 0, 10000, 0, width) % width;
  float y = map(time2, 0, 10000, 0, width) % width;
  
  strokeWeight(3);
  stroke(255);
  line(x, 0, x, height);
  
  stroke(255);
  line(0, y, width, y);
  
}



void mousePressed() {
}

void oscEvent(OscMessage theOscMessage) {
  val1 = theOscMessage.get(0).floatValue();
  val2 = theOscMessage.get(1).floatValue();
  
  dt1 = val1;
  dt2 = val2;
}
