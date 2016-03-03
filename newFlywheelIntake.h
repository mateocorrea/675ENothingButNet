#define leftFlyEnc nMotorEncoder[leftLauncher]
#define rightFlyEnc nMotorEncoder[rightLauncher]
#define shotSwitch SensorValue[ballCounter]

#define speedBtn vexRT[Btn5D]
#define intakeBtn vexRT[Btn6U]
#define outtakeBtn vexRT[Btn6D]
#define flyWheelBtn vexRT[Btn7U]
#define lowerBtn vexRT[Btn7R]
#define rollerBtn vexRT[Btn8R]
#define rollerOutBtn vexRT[Btn8D]
#define raiseBtn vexRT[Btn8L]

int rpmLow = 102;
int rpmMid = 120;
int rpmHigh = 158;
/////////////////////////////////////////////////////////
float KpL = 1.0;
float KpR = 1.0;
float KiL = 1.0;
float KiR = 1.0;
float KdL = 1.0;
float KdR = 1.0;
/////////////////////////////////////////////////////////
float KpStable = 0.01900500;
float KiStable = 0.00000200;
float KdStable = 0.05000000;
/////////////////////////////////////////////////////////
float KpLow = 0.0675;
float KiLow = 0.00675;
float KdLow = 0.6753;
/////////////////////////////////////////////////////////
float KpMid = 0.018;
float KiMid = 0.00350;
float KdMid = 0.4;
/////////////////////////////////////////////////////////
float KpHigh = 0.01600000;
float KiHigh = 0.0024000;
float KdHigh = 0.002000;


//            VARIABLES AND FUNCTIONS                  //
/////////////////////////////////////////////////////////
float rpmGoal = rpmHigh;
float rpmLeft = 0.0;
float rpmRight = 0.0;
float averageLeftError = 0.0;
float averageRightError = 0.0;
float flySpeedLeft = 0.0;
float flySpeedRight = 0.0;
int lowSpeed = 40;
int midSpeed = 55;
int highSpeed = 80;
/////////////////////////////////////////////////////////
bool flyWheelOn = false;
bool rollerOn = true;
float oldLeftError = 0.0;
float oldRightError = 0.0;
float integralRight = 0.0;
float integralLeft = 0.0;
float lastTime = 0;
float oldFilter = 0.0;
int lastFlyWheelTime = 0;
int MAX_POWER = 120;
int MIN_POWER = 35;
int maxPower = MAX_POWER;
int minPower = MIN_POWER;
int encoderTimer = 25;
int ticksPerTurnSpeed = 372;
int batteryValues = 0;
int averageBattery = 0;
int leftValues = 0;
int rightValues = 0;
int intakeSpeed = 127;
int lowPowerBias = 0;
int midPowerBias = 0;
int highPowerBias = 0;
int red = 0;
int green = 1;
int yellow = 2;
int initialTime = 0;

int lowShots = 0;
int midShots = 0;
int highShots = 0;

/////////////////////////////////////////////////////////
int voltageCorrection(int rpm);
task flyWheelControl();
task flyWheelPower();
task autoPIDTuner();
task intake();
void turnOn(int color);
void flyWheelMotors(float left, float right);
void minimalPIDChange(int goal);
void pidChange(int goal);
void averageRPMError(float left, float right);
void normalizeFlyPower();
void setPIDConstants();
void slowStart();
void slowStop();
void resetFlyWheel();
int powerBias();
void powerBias(int change);
task shotTracker();

bool alreadyOff = false;

bool startup = true;
int removableTime = 0;
/////////////////////////////////////////////////////////

task flyWheelPower() {
    resetFlyWheel();

	while(true) {
		if(flyWheelOn) {
            alreadyOff = false;
			if(startup)
				slowStart();
			startup = false;
			wait1Msec(encoderTimer);
			setPIDConstants();
			if(singleShotMode)
				minimalPIDChange(rpmGoal + powerBias());
			else
				pidChange(rpmGoal + powerBias());
			normalizeFlyPower();
			flyWheelMotors(flySpeedLeft, flySpeedRight);
		} else {
            if(!alreadyOff) {
                slowStop();
                alreadyOff = true;
            }
			flyWheelMotors(0,0);
			startup = true;
		}
	}
}

task flyWheelControl() {
	lastFlyWheelTime = nSysTime;
	bool justSwitchedFlywheel = false;
	bool lastRaiseBtn = false;
	bool lastLowerBtn = false;
	bool lastSpeedBtn = false;

	//bool lastIntakeSpeedBtn = false;
	while(true) {

        if(!flyWheelBtn) {
            clearTimer(T2);
            justSwitchedFlywheel = false;
        }
        if((time1[T2] >= 1000) && !justSwitchedFlywheel) {
            flyWheelOn = !flyWheelOn;
            justSwitchedFlywheel = true;
            if(flyWheelOn == true)
                resetFlyWheel();
        }


		// Flywheel speed selection //
		if(speedBtn && !lastSpeedBtn) {
			if(rpmGoal == rpmLow) {
				flySpeedLeft = midSpeed;
				flySpeedRight = midSpeed;
				rpmGoal = rpmMid;
			} else if (rpmGoal == rpmMid) {
				flySpeedLeft = highSpeed; // to speed up recovery
				flySpeedRight = highSpeed; // to speed up recovery
				rpmGoal = rpmHigh;
			} else {
				flySpeedLeft = lowSpeed;
				flySpeedRight = lowSpeed;
				rpmGoal = rpmLow;
			}
			lastSpeedBtn = true;
		} else if ( speedBtn == 0 ) {
			lastSpeedBtn = false;
		}

        /* Power Bias */
		if(raiseBtn == 1 && lastRaiseBtn == false) {
			powerBias(2);
			lastRaiseBtn = true;
		} else if (raiseBtn == 0) {
			lastRaiseBtn = false;
		}
		if(lowerBtn == 1 && lastLowerBtn == false) {
			powerBias(-2);
			lastLowerBtn = true;
		} else if (lowerBtn == 0) {
			lastLowerBtn = false;
		}

        /* Reset time for Excel graph */
		if(intakeBtn && !lastIntakeBtn) {
			initialTime = nSysTime;
			lastIntakeBtn = true;
		} else if (intakeBtn == 0) {
			lastIntakeBtn = false;
		}

		lastFlyWheelTime = nSysTime;
	}
}

task intake()
{
	bool lastRollerBtn = false;
	bool lastIntakeBtn = false;
	while(true) {
		/* Intake Power */
        if(rpmGoal != 2)
        {
            intakeSpeed = (rpmGoal == rpmHigh) ? 127 : 127;
            motor[chain] = intakeSpeed * (intakeBtn - outtakeBtn);
        } else {
            if(intakeBtn)
            {
                motor[chain] = 127;
                wait1Msec(120);
                motor[chain] = 0;
                wait1Msec(78);
            }
        }

		/* Roller Power */
		if(rollerBtn == 1 && lastRollerBtn == false) {
			rollerOn = !rollerOn;
			lastRollerBtn = true;
        } else if (rollerBtn == 0) {
			lastRollerBtn = false;
		}
        motor[roller] = (rollerOutBtn) ? -127 : (127 * rollerOn);
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
void flyWheelMotors(float left, float right)
{
	int l = round(left);
	int r = round(right);
	motor[topRightLauncher] = r;
	motor[topLeftLauncher] = l;
	motor[bottomRightLauncher] = r;
	motor[bottomLeftLauncher] = l;
}
void pidChange(int goal)
{
	float deltaTime = ((nSysTime - lastTime) > 0) ? abs(nSysTime - lastTime - removableTime) : encoderTimer;
    removableTime = 0;
	if(deltaTime < 1)
		deltaTime = 1;
    if(rpmHigh < 400)
      launcherRatio = 1;
	float factor = ( ( launcherRatio * 60000 ) / deltaTime ) / ticksPerTurnSpeed;
	rpmLeft = abs(flyEncLeft * factor);
	rpmRight = abs(flyEncRight * factor);
	float leftError = voltageCorrection(goal) - rpmLeft;
	float rightError = voltageCorrection(goal) - rpmRight;
	if((vexRT[Btn6U] && !vexRT[Btn8U]) || (autoTuning))
		averageRPMError(abs(leftError), abs(rightError));

	////// Proportional /////
	float pLeft = KpL * leftError;
	float pRight = KpR * rightError;

	////// Integral //////
	integralRight = integralRight + (rightError * deltaTime);
	integralLeft = integralLeft + (leftError * deltaTime);
	if( ((integralRight > 0) && (rightError < 0)) || ((integralRight < 0) && (rightError > 0)) )
		integralRight = (rightError * deltaTime);
	if( ((integralLeft > 0) && (leftError < 0)) || ((integralLeft < 0) && (leftError > 0)) )
		integralLeft = (leftError * deltaTime);
	float iLeft = KiL * integralLeft;
	float iRight = KiR * integralRight;

	////// Derivative /////
	float derivativeLeft = (leftError - oldLeftError) / deltaTime;
	float derivativeRight = (rightError - oldRightError) / deltaTime;
	float dLeft = KdL * derivativeLeft;
	float dRight = KdR * derivativeRight;

	// PID //
	float leftChange = pLeft + iLeft + dLeft;
	float rightChange = pRight + iRight + dRight;

	// Adjust Speed //
    flySpeedLeft = (goal == 0) ? (flySpeedLeft * oldFilter + ((1.0 - oldFilter) * leftChange)) : flySpeedLeft + leftChange;
    flySpeedRight = (goal == 0) ? (flySpeedRight * oldFilter + ((1.0 - oldFilter) * rightChange)) : flySpeedRight + rightChange;

	//writeDebugStreamLine("%f", leftChange);
	//writeDebugStreamLine("%f", rightChange);

	// Reset the errors and the encoders
	oldLeftError = leftError;
	oldRightError = rightError;
	flyEncLeft = 0;
	flyEncRight = 0;
	lastTime = nSysTime;

	//writeDebugStreamLine("%d, %f", nSysTime-initialTime, rpmLeft);
}

int voltageCorrection(int rpm)
{
	int flyBat = SensorValue[in4];
	int sum = (batteryValues * averageBattery) + flyBat;
	batteryValues++;
	averageBattery = sum / batteryValues;
	if(batteryValues == 2000)
		batteryValues = 100;
	/*if(rpm == rpmHigh)
		return rpm + (-0.122 * averageBattery + 176.03);
	else*/
	return rpm;
}

void averageRPMError(float left, float right)
{
	float rightSum = (rightValues * averageRightError) + right;
	rightValues = rightValues + 1.0;
	averageRightError = rightSum / rightValues;
	float leftSum = (leftValues * averageLeftError) + left;
	leftValues = leftValues + 1.0;
	averageLeftError = leftSum / leftValues;
}

void normalizeFlyPower()
{
	if(rpmGoal == rpmLow) {
        minPower = MIN_POWER;
        maxPower = 70;
    } else if (rpmGoal == rpmMid) {
        minPower = 40;
        maxPower = 94;
    } else if (rpmGoal == rpmHigh) {
        minPower = 65;
        maxPower = MAX_POWER;
    } else {
        minPower = MIN_POWER;
        maxPower = MAX_POWER;
    }
	if(flySpeedLeft < minPower)
		flySpeedLeft = minPower;
	if(flySpeedRight < minPower)
		flySpeedRight = minPower;
	if(flySpeedLeft > maxPower)
		flySpeedLeft = maxPower;
	if(flySpeedRight > maxPower)
		flySpeedRight = maxPower;
}

void slowStart()
{
	initialTime = nSysTime;
	for(int i = 1; i < 46; i++)
	{
		flyWheelMotors(i * 1.0, i * 1.0);
		wait1Msec(10);
	}
    if (rpmGoal == rpmMid) {
        flySpeedLeft = midSpeed;
        flySpeedRight = midSpeed;
    } else if(rpmGoal == rpmLow) {
        flySpeedLeft = lowSpeed;
        flySpeedRight = lowSpeed;
    } else {
        rpmGoal = rpmHigh;
        flySpeedLeft = highSpeed;
        flySpeedRight = highSpeed;
    }
    flyWheelMotors(flySpeedLeft, flySpeedRight);
    wait1Msec(50);
    flyEncLeft = 0;
    flyEncRight = 0;

    removableTime = 500;
}

void slowStop()
{
	int i = 45;
	if(rpmGoal == rpmLow)
		i = MIN_POWER;
	while(i >= 0)
	{
		flyWheelMotors(i * 1.0, i * 1.0);
		wait1Msec(10);
		i--;
	}
}

void resetFlyWheel()
{
	initialTime = nSysTime;
    if (rpmGoal == rpmMid) {
        flySpeedLeft = midSpeed;
        flySpeedRight = midSpeed;
    } else if(rpmGoal == rpmLow) {
        flySpeedLeft = lowSpeed;
        flySpeedRight = lowSpeed;
    } else {
        rpmGoal = rpmHigh;
        flySpeedLeft = highSpeed;
        flySpeedRight = highSpeed;
    }
    lowPowerBias = 0;
    midPowerBias = 0;
    highPowerBias = 0;
    flyWheelOn = true;
    startup = true;
}

int powerBias()
{
    if(rpmGoal == rpmLow)
        return lowPowerBias;
    if(rpmGoal == rpmMid)
        return midPowerBias;
    return highPowerBias;
}

void powerBias(int change)
{
    if(rpmGoal == rpmLow)
        lowPowerBias += change;
    else if(rpmGoal == rpmMid)
        midPowerBias += change;
    else
        highPowerBias += change;
}

void setPIDConstants()
{
	if(intakeBtn) {
		Kp = KpStable;
		Ki = KiStable;
		Kd = KdStable;
	} else {
		if(rpmGoal == rpmLow) {
			Kp = KpLow;
			Ki = KiLow;
			Kd = KdLow;
		} else if (rpmGoal == rpmLow) {
			Kp = KpMid;
			Ki = KiMid;
			Kd = KdMid;
		} else { 
			Kp = KpHigh;
			Ki = KiHigh;
			Kd = KdHigh;
		}
	}
}

task shotTracker()
{
    bool lastShotSwitch = false;
    while(true)
    {
        if(shotSwitch && !lastShotSwitch) {
            if(rpmGoal == rpmLow)
                lowShots++;
            else if(rpmGoal == rpmHigh)
                highShots++;
            else
                midShots++;
            lastShotSwitch = true;
        } else if (!shotSwitch) {
            lastShotSwitch = false;
        }
    }
}

