#include <CheapStepper.h>

// #defines for the stepper connections:
#define IN1 11
#define IN2 10
#define IN3 9
#define IN4 8

CheapStepper stepper(IN1, IN2, IN3, IN4); // We create a CheapStepper object

bool moveClockwise = true;  // We want it to move clockwise

void setup() {
  Serial.begin(9600);
  Serial.println("Ready to start moving!");
}

void loop() {
  
  // Our Stepper has 4096 steps in a full rotation
  // A for()-loop runs a preset number of times, here 4096, controlled with the variable s:
  for (int s=0; s < 4096; s++){
    stepper.step(moveClockwise); // stepper.step CW or CCW, depending on a boolean true for CW, or false for CCW
                                  // Otherwise, we can use stepper.stepCW(), or stepper.stepCCW()

    int nStep = stepper.getStep(); // We can also read the current position with stepper.getStep()

    // Every 64th step, we print the position:
    if (nStep % 64 == 0){
      Serial.print("Current step position: "); Serial.print(nStep);
      Serial.println();
    }
  }
  
  delay(1000);
  moveClockwise = !moveClockwise; // Reverse by toggling moveClockwise
}
