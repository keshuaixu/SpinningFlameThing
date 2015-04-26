
#include <AccelStepper.h>
#include "SpinningFlameThing.h"

AccelStepper stepper(AccelStepper::DRIVER,53,52); 

//stepper, CW max angle, CCW max angle(endstop), number of steps, left sensor, right sensor, switch  
SpinningFlameThing flame(&stepper, -55, 220, 200*4, A1, A2, 28);

void setup()
{  
	Serial.begin(115200);
	
	stepper.setAcceleration(1500);
	
	//non-blocking zeroing. flame.isDone() returns true when finished
	flame.zero();
}

void loop(){
	//call this as often as possible
	flame.run();

	if (flame.isDone()){
		int high;
		int low;
		int r;
		int theta;

		// call this when scanning is done
		flame.getFlamePosition(&high, &low, &r, &theta);

		//first set is garbage value because it was zeroing not scanning
		Serial.println(r);
		Serial.println(theta);
		Serial.println(high);
		Serial.println(low);
		Serial.println("");
		// use high-low to determine if fire exist 

		//initiate a new scan.
		//from angle, to angle, speed
		flame.scan(-50, 50, 50);
	}
}