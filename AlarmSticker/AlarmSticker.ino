#include <TM1637.h>
#define CLK 2
#define DIO 3
#define FNB 11  // Function Button
#define UPB 12  // Up Button
#define DNB 13  // Down Button
#define CAM 1  // change alarm mode
#define CTM 0  // change time mode
#define VIB 7  // vibration moter pin
TM1637 tm(CLK,DIO);

int state_FN, state_UP, state_DN, mode;
double time = 0;
int alarm_time = -1;
const int maxTime = 60*60*24;

void setup() {
  pinMode(FNB, INPUT);
  pinMode(UPB, INPUT);  
  pinMode(DNB, INPUT);
  pinMode(VIB, OUTPUT);
  Serial.begin(9600);
  tm.init();
  tm.set(2);
}

void loop() {
  display();
  timeDelay();
  if (digitalRead(FNB) == HIGH) {
    if (!state_FN) {
      timeDelay(); 
      state_FN = 1; 
    }
  } else {
    if (state_FN) setting();
  }
}

void setting() {
  tm.clearDisplay();
  timeDelay();
  state_FN = 0;
  while (1) {
    timeDelay();
    if (digitalRead(UPB) == HIGH) {
      if (!state_UP) { timeDelay(); state_UP = 1; }
    } else {
      if (state_UP) {mode = !(mode); timeDelay(); state_UP = 0; }
    }
    if (digitalRead(DNB) == HIGH) {
      if (!state_DN) { timeDelay(); state_DN = 1; }
    } else {
      if (state_DN) {mode = !(mode); timeDelay(); state_DN = 0; }
    }
    if (digitalRead(FNB) == HIGH) {
      if (!state_FN) { timeDelay(); state_FN = 1; }
    } else {
      if (state_FN) { timeDelay(); state_FN = 0; break; }
    }

    tm.display(0, 15);
    tm.display(1, mode);
  }
  
  int tmp_time[] = {0, 0, 0, 0};
  int digit_limit[] = {2, 0, 5, 9};
  for (int i = 0; i < 4; i++) {
    while (1) {
      timeDelay();
      if (digitalRead(UPB) == HIGH) {
        if (!state_UP) { timeDelay(); state_UP = 1; }
      } else {
        if (state_UP) { tmp_time[i] = (tmp_time[i] + 1) % (digit_limit[i]+1); timeDelay(); state_UP = 0; }
      }
      if (digitalRead(DNB) == HIGH) {
        if (!state_DN) { timeDelay(); state_DN = 1; }
      } else {
        if (state_DN) { tmp_time[i] = (tmp_time[i] == 0 ? 0 : tmp_time[i]-1) % (digit_limit[i]+1); timeDelay(); state_DN = 0; }
      }
      if (digitalRead(FNB) == HIGH) {
        if (!state_FN) { timeDelay(); state_FN = 1; }
      } else {
        if (state_FN) { timeDelay(); state_FN = 0; break; }
      }

      for (int j = 0; j < 4; j++) tm.display(j, tmp_time[j]);
    }
    if (!i) digit_limit[1] = (tmp_time[0] == 2 ? 3 : 9);
  }
  switch (mode) {
    case CAM:
    alarm_time = tmp_time[3] + 10 * tmp_time[2] + 60 * tmp_time[1] + 600 * tmp_time[0];
    break;
    case CTM:
    time = tmp_time[3] + 10 * tmp_time[2] + 60 * tmp_time[1] + 600 * tmp_time[0];    
  }
}

void display() {
  int hour = (int)(time)/(60*60);
  int minute = (int)(time)%(60*60)/60;
  int second = (int)(time)%60;
  // tm.display(0, hour/10);
  // tm.display(1, hour%10);
  // tm.display(2, minute/10);
  // tm.display(3, minute%10);
  tm.display(0, minute/10);
  tm.display(1, minute%10);
  tm.display(2, second/10);
  tm.display(3, second%10);
}

void timeDelay() {
  if ((int)(time) == alarm_time)
    alarm();    
  delay(10);
  time = (time+0.01 == maxTime ? 0 : time+0.01);
}

void timeDelayWithoutAlarm() {
  delay(10);
  time = (time+0.01 == maxTime ? 0 : time+0.01);
}


void alarm() {
  tm.display(0, 10);
  tm.display(1, 11);
  tm.display(2, 12);
  tm.display(3, 13);
  digitalWrite(VIB, HIGH);
  state_FN = 0;

  while (1) {
    timeDelayWithoutAlarm();
    if (digitalRead(FNB) == HIGH) {
      if (!state_FN) {
        timeDelayWithoutAlarm(); 
        state_FN = 1; 
      }
    } else {
      if (state_FN) break;
    }
  }
  digitalWrite(VIB, LOW);
  Serial.println("TEST");
  state_FN = 0;
}