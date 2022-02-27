#include <TM1637.h>
#define CLK 2
#define DIO 3

TM1637 tm(CLK,DIO);

int time = 0;
const int maxTime = 60*60*24;

void setup() {
  tm.init();
  tm.set(2);
}

void loop() {
  display();
  delay(1000);
  time = (time+1)%maxTime;
}

void display() {
  int hour = time/(60*60);
  int minute = time%(60*60)/60;
  tm.display(0, hour/10);
  tm.display(1, hour%10);
  tm.display(2, minute/10);
  tm.display(3, minute%10);
}