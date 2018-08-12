
// Output pins for light1 and light 2 - order red, yellow, green
int pins[][3] = { { 4, 3, 2 }, { 10, 16, 14 } };

// Input pins for priority at light 1 and light2
int priorityPins[2] = { 8, 9 };

// Onboard RX LED is on pin 17
const int RX_LED = 17;

// Sequence: red, yellow, green
int lightSeq[][3] = { { HIGH, LOW, LOW }, { HIGH, HIGH, LOW }, { LOW, LOW, HIGH }, { LOW, HIGH, LOW } };

int greenTime = 10000;
int switchTime = 2000;

int state[2] = { 0, 0 };

void step(int green) {
  for(int seq=0; seq < 2; seq++) {
    state[green] = (state[green]+1)%4;
    setLight(pins[green], lightSeq[state[green]]);
    delay(switchTime);
  }
}

void setLight(int pins[], int signals[]) {
  for(int i=0; i<3; i++) {
    digitalWrite(pins[i], signals[i]);
  }
}

void interruptibleDelay(int green, int greenTime) {
  const int sleepTime = 100;
  int totalDelay = 0;
  int prioGreen = 0;
  int prioRed = 0;
  int stateRxLed = 0;
  
  // Continue delay while total delay has not elapsed or there is a green priority
  while(totalDelay < greenTime || prioGreen) {
    prioGreen = digitalRead(priorityPins[green]);
    prioRed = digitalRead(priorityPins[green ^ 1]);
  
    if(prioRed == HIGH) {
      // Got priority on red side - stop waiting so direction can be changed, if there is no current green priority
      totalDelay = greenTime;
    }
    if(prioGreen) {
      // Blink RX LED while priority signal active
      digitalWrite(RX_LED, stateRxLed ? HIGH : LOW);
      stateRxLed ^= 1;
    }
    delay(sleepTime);
    totalDelay += sleepTime;
  }
  // Ensure RX LED is cleared
  digitalWrite(RX_LED, LOW);
}

void setup() {
  pinMode(RX_LED, OUTPUT);
  for(int l=0; l<2; l++) {
    for(int c=0; c<3; c++) {
      pinMode(pins[l][c], OUTPUT);
    }
  }
  pinMode(priorityPins[0], INPUT);
  pinMode(priorityPins[1], INPUT);
  
  setLight(pins[0], lightSeq[state[0]]);
  setLight(pins[1], lightSeq[state[1]]);
}

int green = 0;

void loop() {
  TXLED1;
  step(green);
  TXLED0;
  interruptibleDelay(green, greenTime);
  TXLED1;
  step(green);
  TXLED0;

  // Switch direction for next iteration
  green ^= 1;
}


