#define flyEncLeft nMotorEncoder[topLeftLauncher]
#define flyEncRight nMotorEncoder[topRightLauncher]
#define driveEncRight nMotorEncoder[rightfront]
#define driveEncLeft nMotorEncoder[leftfront]
#define intakeSensor SensorValue[intakeTouchSensor]
#define shootSensor SensorValue[launcherLimitSwitch]

#define speedBtn vexRT[Btn5D]
#define intakeBtn vexRT[Btn6U]
#define outtakeBtn vexRT[Btn6D]
#define flyWheelBtn vexRT[Btn7U]
#define lowerBtn vexRT[Btn7R]
#define modeBtn vexRT[Btn8U]
#define rollerBtn vexRT[Btn8R]
#define rollerOutBtn vexRT[Btn8D]
#define raiseBtn vexRT[Btn8L]
float rpmLeft = 0;
float rpmRight = 0;

float flySpeedLeft = 0;
float flySpeedRight = 0;

// PID Code //
float oldLeftError = 0;
float oldRightError = 0;

float integralRight = 0;
float integralLeft = 0;

float timeSinceShot = 0;

int encoderTimer = 20;
int ticksPerTurnSpeed = 372;
float launcherRatio = 10.2;
//////////////

bool lastModeBtn = false;
bool lastRaiseBtn = false;
bool lastLowerBtn = false;
bool lastSpeedBtn = false;
bool lastFlyWheelBtn = false;
bool lastRollerBtn = false;
bool flyWheelOn = false;
bool rollerOn = true;
bool rpmMode = true;
bool justChangedToRPM = false;

task flyWheelControl();
task flyWheelPower();
task intake();
void turnOn(int color);
void flyWheelMotors(float left, float right);
void pidChange(int rpmGoal);
void voltageCorrection(int rpm);
bool notShooting();
bool doneShooting();

/////////////////////////////////////////////////////////
int lowSpeed = 37;
int midSpeed = 43;
int highSpeed = 85;//80 soft balls
int flyWheelSpeed = highSpeed;


const int rpmMid = 1500;



float KpL = 1.0;
float KpR = 1.0;
float KiL = 1.0;
float KiR = 1.0;
float KdL = 1.0;
float KdR = 1.0;

float KpLow  = 0.000900500;
float KiLow  = 0.000001200;
float KdLow  = 0.0000000;//0.000000250;
float KpLowShooting  = 0.000900500;
float KiLowShooting  = 0.000002150;
float KdLowShooting  = 0.000000020;
const int rpmLow = 1210;

float KpMid  = 0.000086500;
float KiMid  = 0.000000000;
float KdMid  = 0.000000000;

float KpHighL = 0.0080;
float KiHighL = 0.0000200;//2200;
float KdHighL = 0.000000;//250;

float KpHighRS = 0.0094190000000;
float KiHighRS = 0.0000830;
float KdHighRS  = 0.000002000;

/*
good values
float KpHighRS = 0.0094190000000;
float KiHighRS = 0.0000700;
float KdHighRS  = 0.000002000;
*/
//////////////////////////////
float KpHighLS = KpHighRS + 0.000015;
float KiHighLS = KiHighRS;
float KdHighLS  = KdHighRS;


float KpHighR = KpHighL;
float KiHighR = KiHighL;
float KdHighR = KdHighL;
const int rpmHigh = 1802;
float rpmGoal = rpmHigh;
/////////////////////////////////////////////////////////

const int red = 0;
const int green = 1;
const int yellow = 2;

int batteryValues = 0;
int averageBattery = 0;

task flyWheelPower() {
	int values = 0;
	int averageBattery = 0;
	flyWheelMotors(20.0, 20.0);
	wait1Msec(70);
	flyWheelMotors(40.0, 40.0);
	wait1Msec(70);
	flyWheelMotors(80.0, 80.0);
	while(true)
	{
		if(flyWheelOn) {
			if(!rpmMode) {
				if((flyWheelSpeed == highSpeed) && (SensorValue[greenLED] == 1))
					turnOn(green);
				if((flyWheelSpeed == midSpeed) && (SensorValue[yellowLED] == 1))
					turnOn(yellow);
				wait1Msec(encoderTimer);
				if(flyWheelSpeed == lowSpeed)	{
					KpL = KpLow;
        	KpR = KpLow;
        	KiL = KiLow;
        	KiR = KiLow;
        	KdL = KdLow;
        	KdR = KdLow;
        	if(intakeBtn == 1)
        	{
        		KpL = KpLowShooting;
        		KpR = KpLowShooting;
	        	KiL = KiLowShooting;
	        	KiR = KiLowShooting;
	        	KdL = KdLowShooting;
	        	KdR = KdLowShooting;
	        }
	        pidChange(rpmGoal);
				} else {
						float factor = ( ( launcherRatio * 60000.0 ) / encoderTimer ) / ticksPerTurnSpeed;
				    rpmLeft = abs(flyEncLeft * factor);
				    rpmRight = abs(flyEncRight * factor);
				    flyEncLeft = 0;
				    flyEncRight = 0;
				    flySpeedLeft = flyWheelSpeed;
					flySpeedRight = flyWheelSpeed;
				}

			} else {
				if(justChangedToRPM && (rpmGoal == rpmHigh)) {
					flyWheelMotors(20.0, 20.0);
					wait1Msec(70);
					flyWheelMotors(40.0, 40.0);
					wait1Msec(70);
					flyWheelMotors(80.0, 80.0);
					justChangedToRPM = false;
				}
				wait1Msec(encoderTimer);

				//delta_ms = nSysTime - nSysTime_last;

				timeSinceShot += encoderTimer;

				if(shootSensor) {
					timeSinceShot = 0;
				}
				if(rpmGoal == rpmLow) {
		        	KpL = KpLow;
		        	KpR = KpLow;
		        	KiL = KiLow;
		        	KiR = KiLow;
		        	KdL = KdLow;
		        	KdR = KdLow;
		        	if(intakeBtn == 1)
		        	{
		        		KpL = KpLowShooting;
		        		KpR = KpLowShooting;
			        	KiL = KiLowShooting;
			        	KiR = KiLowShooting;
			        	KdL = KdLowShooting;
			        	KdR = KdLowShooting;
			        }
				} else if(rpmGoal == rpmMid) {
		        	KpL = KpMid;
		        	KpR = KpMid;
		        	KiL = KiMid;
		        	KiR = KiMid;
		        	KdL = KdMid;
		        	KdR = KdMid;
		        } else {
		        	KpL = KpHighL;
		        	KpR = KpHighR;
		        	KiL = KiHighL;
		        	KiR = KiHighR;
		        	KdL = KdHighL;
		        	KdR = KdHighR;
		        	if(intakeBtn == 1)
		        	{
		        		KpL = KpHighLS;
		        		KpR = KpHighRS;
			        	KiL = KiHighLS;
			        	KiR = KiHighRS;
			        	KdL = KdHighLS;
			        	KdR = KdHighRS;
			        }
		      	}

		        pidChange(rpmGoal);
	    	}
	    	if(flySpeedLeft < 30)
	    		flySpeedLeft = 30;
	    	if(flySpeedRight < 30)
	    		flySpeedRight = 30;
	    	flyWheelMotors(flySpeedLeft, flySpeedRight);
	  	} else {
			flyWheelMotors(0,0);
		}
	}
}

task flyWheelControl() {
	if(rpmGoal == rpmHigh)
		flyWheelSpeed = highSpeed;
	if(rpmGoal == rpmMid)
		flyWheelSpeed = midSpeed;
	if(rpmGoal == rpmLow)
		flyWheelSpeed = lowSpeed;

	while(true) {

		// Flywheel On/Off //
		if(flyWheelBtn == 1 && lastFlyWheelBtn == false) {
			flyWheelOn = !flyWheelOn;
			if((flyWheelOn == true) && (rpmMode == true))
				justChangedToRPM = true;
			lastFlyWheelBtn = true;
		} else if (flyWheelBtn == 0) {
			lastFlyWheelBtn = false;
		}

		// Flywheel speed selection //
		if(speedBtn && !lastSpeedBtn) {
			if(flyWheelSpeed == lowSpeed) {
				flyWheelSpeed = midSpeed;
				rpmGoal = rpmMid;
				turnOn(yellow);
			} else if (flyWheelSpeed == midSpeed) {
				flyWheelSpeed = highSpeed;
				flySpeedLeft = 80;
				flySpeedRight = 80;
				rpmGoal = rpmHigh;
				turnOn(green);
			} else {
				flyWheelSpeed = lowSpeed;
				rpmGoal = rpmLow;
				turnOn(red);
			}
			lastSpeedBtn = true;
		} else if ( speedBtn == 0 ) {
			lastSpeedBtn = false;
		}

		if(modeBtn == 1 && lastModeBtn == false) {
			rpmMode = !rpmMode;
			if(rpmMode == true)
				justChangedToRPM = true;
			lastModeBtn = true;
		} else if (modeBtn == 0) {
			lastModeBtn = false;
		}

		if(raiseBtn == 1 && lastRaiseBtn == false) {
			highSpeed += 2;
			midSpeed += 2;

			if(flyWheelSpeed == (highSpeed-2))
				flyWheelSpeed = highSpeed;
			else if(flyWheelSpeed == (midSpeed-2))
				flyWheelSpeed = midSpeed;
			lastRaiseBtn = true;
		} else if (raiseBtn == 0) {
			lastRaiseBtn = false;
		}

		if(lowerBtn == 1 && lastLowerBtn == false) {
			highSpeed -= 2;
			midSpeed -= 2;

			if(flyWheelSpeed == (highSpeed+2))
				flyWheelSpeed = highSpeed;
			else if(flyWheelSpeed == (midSpeed+2))
				flyWheelSpeed = midSpeed;

			lastLowerBtn = true;
		} else if (lowerBtn == 0) {
			lastLowerBtn = false;
		}
	}
}

task intake()
{
	while(true) {

		/* Intake Power */
		if(rpmMode == true) {
			motor[chain] = 127 * (intakeBtn - outtakeBtn);
		} else {
			motor[chain] = 127 * (intakeBtn - outtakeBtn);
		}

		/* Roller Power */
		if(rollerBtn == 1 && lastRollerBtn == false) {
			rollerOn = !rollerOn;
			lastRollerBtn = true;
		} else if (rollerBtn == 0) {
			lastRollerBtn = false;
		}
		if(rollerOutBtn)
			motor[roller] = -127;
		else
			motor[roller] = 127 * rollerOn;
	}
}

void turnOn(int color) {
	SensorValue[redLED] = 1;
	SensorValue[greenLED] = 1;
	SensorValue[yellowLED] = 1;

	if(color == red) {
		SensorValue[redLED] = 0;
	} else if (color == green) {
		SensorValue[greenLED] = 0;
	} else if (color == yellow) {
		SensorValue[yellowLED] = 0;
	}
}

bool notShooting()
{
	if(timeSinceShot > 5000)
		return true;
	return false;
}

bool doneShooting() {
	if((timeSinceShot > 2000) && (timeSinceShot < 5000))
		return true;
	return false;
}

void flyWheelMotors(float left, float right)
{
	int l = round(left);
	int r = round(right);
	if(l < 0)
		l *= -1;
	if(r < 0)
		r *= -1;
	if(l > 118)
		l = 118;
	if(r > 118)
		r = 118;
	motor[topRightLauncher] = r;
	motor[topLeftLauncher] = l;
	motor[bottomRightLauncher] = r;
	motor[bottomLeftLauncher] = l;
}

void pidChange(int rpmGoal)
{
	float factor = ( ( launcherRatio * 60000 ) / encoderTimer ) / ticksPerTurnSpeed;
	rpmLeft = abs(flyEncLeft * factor);
	rpmRight = abs(flyEncRight * factor);

	float leftError = voltageCorrection(rpmGoal) - rpmLeft;
	float rightError = voltageCorrection(rpmGoal) - rpmRight;

	////// Proportional /////

	float pLeft = KpL * leftError;
	float pRight = KpR * rightError;


	////// Integral //////
	integralRight = integralRight + (rightError * encoderTimer);
	integralLeft = integralLeft + (leftError * encoderTimer);
	if( ((integralRight > 0) && (rightError < 0)) || ((integralRight < 0) && (rightError > 0)) )
		integralRight = (rightError * encoderTimer);
	if( ((integralLeft > 0) && (leftError < 0)) || ((integralLeft < 0) && (leftError > 0)) )
		integralLeft = (leftError * encoderTimer);
	float iLeft = KiL * integralLeft;
	float iRight = KiR * integralRight;

	////// Derivative /////
	float derivativeLeft = (leftError - oldLeftError) / encoderTimer;
	float derivativeRight = (rightError - oldRightError) / encoderTimer;
	float dLeft = KdL * derivativeLeft;
	float dRight = KdR * derivativeRight;

	// PID //
	float leftChange = pLeft + iLeft + dLeft;
	float rightChange = pRight + iRight + dRight;

	// Adjust Speed //
  flySpeedLeft += leftChange;
  flySpeedRight += rightChange;

  // Reset the errors and the encoders
  oldLeftError = leftError;
  oldRightError = rightError;
  flyEncLeft = 0;
  flyEncRight = 0;
}

int voltageCorrection(int rpm)
{
    int sum = (batteryValues * averageBattery) + flyBattery; //flyBattery is the sensor value
    batteryValues++;
    averageBattery = sum / batteryValues;
    if(batteryValues == 1000)
        batteryValues = 100;
    
    if(rpm == rpmHigh)
        return 5 * averageBattery + 10  //mx + b
    else
        return rpm
}

