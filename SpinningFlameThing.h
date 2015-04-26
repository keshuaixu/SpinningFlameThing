#ifndef SpinningFlameThing_h
#define SpinningFlameThing_h
#include "Arduino.h"
#include "AccelStepper.h"
#define FLAME_MAX_ARRAY_SIZE 800
class SpinningFlameThing{
public:
	SpinningFlameThing(AccelStepper *stepper, int initialAngle, int finalAngle, unsigned int stepsPerRevolution, int pinLeft, int pinRight, int pinSwitch);
	void scan(int beginAngle, int endAngle, float speed);
	void run();
	void zero();
	boolean isDone();
	void getFlamePosition(int *high, int *low, int *r, int *theta);
	enum State {
		MOVING_TO_START,
		SCANNING,
		IDLE,
		ZEROING
	};


	State state = IDLE;

private:
	int angleToStep(int angle);
	int stepToIndex(int step);
	float indexToRadians(int index);
	void processFlameData(uint16_t a[], int start, int end, int *maxPosition, uint16_t *maxValue, uint16_t *minValue);
	float mapfloat(float x, float in_min, float in_max, float out_min, float out_max);
	const int STEP_PER_INDEX=1;
	AccelStepper *stepper;

	int initialAngle;
	int finalAngle;
	unsigned int stepsPerRevolution;

	byte pinLeft;
	byte pinRight;
	byte pinSwitch;

	uint16_t leftData[FLAME_MAX_ARRAY_SIZE];
	uint16_t rightData[FLAME_MAX_ARRAY_SIZE]; 

	const float rapidSpeed = 500;
	float scanSpeed; 
	int lastPosition = -1;
	int plateauSize;

	int scanEndAngle;

	int lastScanIndexStart;
	int lastScanIndexEnd;
	
};
#endif
