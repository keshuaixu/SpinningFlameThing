#include "SpinningFlameThing.h"

SpinningFlameThing::SpinningFlameThing(AccelStepper *stepper, int initialAngle, int finalAngle, unsigned int stepsPerRevolution, int pinLeft, int pinRight){
	this->stepper = stepper;
	this->initialAngle = initialAngle;
	this->finalAngle = finalAngle;
	this->stepsPerRevolution = stepsPerRevolution;
	this->pinLeft = pinLeft;
	this->pinRight = pinRight;
}

void SpinningFlameThing::scan(int beginAngle, int endAngle, float speed){
	stepper->setMaxSpeed(rapidSpeed);
	scanSpeed = speed;
	stepper->moveTo(angleToStep(beginAngle));	
	scanEndAngle = endAngle;

	lastScanIndexStart = stepToIndex(angleToStep(beginAngle));
	lastScanIndexEnd = stepToIndex(angleToStep(endAngle));

	state = MOVING_TO_START;
}

int SpinningFlameThing::angleToStep(int angle){
	return map(angle, initialAngle, finalAngle, 0, (long)(finalAngle - initialAngle)*stepsPerRevolution/360);
}

float SpinningFlameThing::indexToRadians(int index){
	return mapfloat(index,0.0, (long)(finalAngle - initialAngle)*stepsPerRevolution/(360*STEP_PER_INDEX), initialAngle * 3.14 / 180, finalAngle * 3.14 / 180);
}

int SpinningFlameThing::stepToIndex(int step){
	return step / STEP_PER_INDEX;
}

void SpinningFlameThing::run(){
	stepper->run();
	switch (state){
		case MOVING_TO_START:
{			if (stepper->distanceToGo() == 0){
				stepper->setMaxSpeed(scanSpeed);
				stepper->moveTo(angleToStep(scanEndAngle));
				state = SCANNING;				
			}}
		break;
		case SCANNING:
{			int thisPosition = stepper->currentPosition() / STEP_PER_INDEX;
			if (thisPosition < 0 /*|| thisPosition > */){
				Serial.println("stepper pos error");
			}
			if ( thisPosition != lastPosition ){
				leftData[thisPosition] = 1023 - analogRead(pinLeft);
				rightData[thisPosition] = 1023 - analogRead(pinRight);
				lastPosition = thisPosition;
			}


			if (stepper->currentPosition() == angleToStep(scanEndAngle)){
				state = IDLE;				
			}}
		break;
		case IDLE:
		break;
		default:
		break;
	}
} 

boolean SpinningFlameThing::isDone(){
	return (state == IDLE);
}

void SpinningFlameThing::getFlamePosition(int *high, int *low, int *r, int *theta){
	int leftMaxPos;
	uint16_t leftMaxVal;
	uint16_t leftMinVal;
	int rightMaxPos;
	uint16_t rightMaxVal;
	uint16_t rightMinVal;

	processFlameData(leftData, lastScanIndexStart, lastScanIndexEnd, &leftMaxPos, &leftMaxVal, &leftMinVal);
	processFlameData(rightData, lastScanIndexStart, lastScanIndexEnd, &rightMaxPos, &rightMaxVal, &rightMinVal);
	*high = max(leftMaxVal, rightMaxVal);
	*low = min(leftMinVal, rightMinVal);
	float leftAngle = indexToRadians(leftMaxPos);
	float rightAngle = indexToRadians(rightMaxPos);
	*theta = (leftAngle + rightAngle) * 180 / 2.0 / 3.14;
	*r = 100.0 / cos((3.14 + leftAngle - rightAngle) / 2.0); 
}

void SpinningFlameThing::processFlameData(uint16_t a[], int start, int end, int *maxPosition, uint16_t *maxValue, uint16_t *minValue){
	plateauSize = 0;
	int i;
	int max = -32000;
	int min = 32000;
	int _maxPosition;
	int _start;
	int _end;
	if (start >= end){
		_start = end;
		_end = start;
	} else {
		_start = start;
		_end = end;
	}
	for (i = _start; i <= _end; i++){
		
		if ((int)a[i] > max){
			max = a[i];
			_maxPosition = i;
			plateauSize = 1;
		} else if ((int)a[i] == max){
			plateauSize++;
		}
		if ((int)a[i] < min){
			min = a[i];
		}
	}

	*maxValue = max;
	*maxPosition = _maxPosition;
	*minValue = min;
}

float SpinningFlameThing::mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
 return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}