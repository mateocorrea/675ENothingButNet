#define flyEncLeft nMotorEncoder[topLeftLauncher]
#define flyEncRight nMotorEncoder[topRightLauncher]
#define driveEncRight nMotorEncoder[rightfront]
#define driveEncLeft nMotorEncoder[leftfront]

#define speedBtn vexRT[Btn5D]
#define intakeBtn vexRT[Btn6U]
#define outtakeBtn vexRT[Btn6D]
#define flyWheelBtn vexRT[Btn7U]
#define lowerBtn vexRT[Btn7R]
#define rollerBtn vexRT[Btn8R]
#define rollerOutBtn vexRT[Btn8D]
#define raiseBtn vexRT[Btn8L]
#define intakeSpeedBtn vexRT[Btn8U]

//            VARIABLES AND FUNCTIONS                  //
/////////////////////////////////////////////////////////
float rpmGoal = rpmMid;
float rpmLeft = 0.0;
float rpmRight = 0.0;
float averageLeftError = 0.0;
float averageRightError = 0.0;
float flySpeedLeft = 0.0;
float flySpeedRight = 0.0;
int lowSpeed = 40;
int midSpeed = 55;
int highSpeed = 70;
/////////////////////////////////////////////////////////
bool softBalls = true;
bool lastRaiseBtn = false;
bool lastLowerBtn = false;
bool lastSpeedBtn = false;
bool lastRollerBtn = false;
bool lastIntakeBtn = false;
bool flyWheelOn = false;
bool rollerOn = true;
bool redoSpeed = true;
//bool rpmMode = true;
bool manualControl = false;
bool autoTuning = false;
float launcherRatio = 10.2;
float oldLeftError = 0.0;
float oldRightError = 0.0;
float integralRight = 0.0;
float integralLeft = 0.0;
float lastTime = 0;
float oldFilter = 0.0;
int lastFlyWheelTime = 0;
const int MAX_POWER = 120;
const int MIN_POWER = 35;
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

/////////////////////////////////////////////////////////
int voltageCorrection(int rpm);
task flyWheelControl();
task flyWheelPower();
task autoPIDTuner();
task intake();
void initFlyWheel();
void turnOn(int color);
void flyWheelMotors(float left, float right);
void pidChange(int goal);
void averageRPMError(float left, float right);
void normalizeFlyPower();
void setPIDConstants();
void slowStart();
void resetFlyWheel();
int powerBias();
void powerBias(int change);
/////////////////////////////////////////////////////////

task flyWheelPower() {
	while(true) {
		if(flyWheelOn) {
			wait1Msec(encoderTimer);
			setPIDConstants();
			pidChange(rpmGoal + powerBias(rpmGoal));
			normalizeFlyPower();
			flyWheelMotors(flySpeedLeft, flySpeedRight);
		} else {
			flyWheelMotors(0,0);
		}
	}
}

task flyWheelControl() {
	lastFlyWheelTime = nSysTime;
	bool justSwitchedFlywheel = false;
	bool lastIntakeSpeedBtn = false;
	initFlyWheel();
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

        /*if(intakeSpeedBtn && !lastIntakeSpeedBtn)
        {
            //intakeSpeed -= 5;
            //if(intakeSpeed < 57)
                //intakeSpeed = 127;
            manualControl = !manualControl;
            lastIntakeSpeedBtn = true;
        } else if (intakeSpeedBtn == 0) {
            lastIntakeSpeedBtn = false;
 		}*/



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
			powerBias(5);
			lastRaiseBtn = true;
		} else if (raiseBtn == 0) {
			lastRaiseBtn = false;
		}
		if(lowerBtn == 1 && lastLowerBtn == false) {
			powerBias(-5);
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
	while(true) {
		/* Intake Power */
        if(rpmGoal == rpmHigh)
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
	float deltaTime = abs(nSysTime - lastTime);
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

    if((manualControl == true) && (!autoTuning))
    {
    	if(rpmGoal == rpmHigh)
    	{
    		flySpeedLeft = 85 + (powerBias()/5);
    		flySpeedRight = 85 + (powerBias()/5);
    	} else if(rpmGoal == rpmMid)
    	{
    		flySpeedLeft = 55 + (powerBias()/5);
    		flySpeedRight = 55 + (powerBias()/5);
    	} else {
    		flySpeedLeft = 40;
    		flySpeedRight = 40;
    	}
    }


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
	flyWheelMotors(15.0, 15.0);
	wait1Msec(100);
	flyWheelMotors(30.0, 30.0);
	wait1Msec(100);
	flyWheelMotors(45.0, 45.0);
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
}

int powerBias()
{
    if(power == rpmLow)
        return lowPowerBias;
    if(power == rpmMid)
        return midPowerBias;
    return highPowerBias;
}

void powerBias(int change)
{
    if(power == rpmLow)
        lowPowerBias += change;
    else if(power == rpmMid)
        midPowerBias += change;
    else
        highPowerBias += change;
}

void setPIDConstants()
{
	if(softBalls)
	{
		if(rpmGoal == rpmLow) {
			KpL = KpLowSoft;
			KpR = KpLowSoft;
			KiL = KiLowSoft;
			KiR = KiLowSoft;
			KdL = KdLowSoft;
			KdR = KdLowSoft;
			if(intakeBtn == 1)
			{
				KpL = KpLowShootingSoft;
				KpR = KpLowShootingSoft;
				KiL = KiLowShootingSoft;
				KiR = KiLowShootingSoft;
				KdL = KdLowShootingSoft;
				KdR = KdLowShootingSoft;
			}
		} else if(rpmGoal == rpmMid) {
			KpL = KpMidSoft;
			KpR = KpMidSoft;
			KiL = KiMidSoft;
			KiR = KiMidSoft;
			KdL = KdMidSoft;
			KdR = KdMidSoft;
		} else {
			KpL = KpHighLSoft;
			KpR = KpHighRSoft;
			KiL = KiHighLSoft;
			KiR = KiHighRSoft;
			KdL = KdHighLSoft;
			KdR = KdHighRSoft;
			if(intakeBtn == 1)
			{
				KpL = KpHighLSSoft;
				KpR = KpHighRSSoft;
				KiL = KiHighLSSoft;
				KiR = KiHighRSSoft;
				KdL = KdHighLSSoft;
				KdR = KdHighRSSoft;
			}
		}
	}
	else
	{
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
	}
}

task autoPIDTuner() {
		rpmGoal = rpmMid;
    autoTuning = true;
    int timeWithValues = 3000;

    float bestP = 0.0;
    float bestI = 0.0;
    float bestD = 0.0;

    float pStep = 0.0010;
    float iStep = 0.000001;
    float dStep = 0.000001;

    float minP = 0.009;
    float minI = 0.0;
    float minD = 0.0;

    float maxP = 0.04;
    float maxI = 0.0004;
    float maxD = 0.0004;


    KpL = (minP == 0.0) ? pStep : minP;
    KpR = KpL;
    KiL = (minI == 0.0) ? iStep : minI;
    KiR = KiL;
    KdL = (minD == 0.0) ? dStep : minD;
    KdR = KdL;

    flyWheelOn = true;
    rpmGoal = rpmHigh;

    flyWheelMotors(80, 80);
    wait1Msec(2000);
    float bestError = 100000;

    while(KpL <= maxP) {
        clearTimer(T3);
        bool resetError = false;
        while(time1[T3] < timeWithValues)
        {
            wait1Msec(encoderTimer);
            pidChange(rpmGoal + powerBias());
            normalizeFlyPower();
            flyWheelMotors(flySpeedLeft, flySpeedRight);
            if(!resetError)
            {
            		leftValues = 0;
            		rightValues = 0;
                averageLeftError = 0;
                averageRightError = 0;
                resetError = true;
            }
        }

        float error = ((averageLeftError + averageRightError) / 2.0);

        writeDebugStreamLine("KpL: %f", KpL);
        writeDebugStreamLine("error: %f", error);

        if(error < bestError)
        {
            bestP = KpL;
        }
        KpL += pStep;
        flyWheelMotors(80, 80);
        wait1Msec(2000);
    }
    writeDebugStreamLine("BEST P IS ");
    writeDebugStreamLine("%f", bestP);
    KpL = bestP;

    /*******************/
   /* while(KiL <= maxI) {
        clearTimer(T3);
        bool resetError = false;
        while(time1[T3] < timeWithValues)
        {
            wait1Msec(encoderTimer);
            pidChange(rpmGoal + powerBias);
            normalizeFlyPower();
            flyWheelMotors(flySpeedLeft, flySpeedRight);
            if(!resetError)
            {
                averageLeftError = 0;
                averageRightError = 0;
                resetError = true;
            }
        }

        float error = ((averageLeftError + averageRightError) / 2.0)

        writeDebugStreamLine("%f", KiL:);
        writeDebugStreamLine("%f", error);

        if(error < bestError)
        {
            bestI = KpI;
        }
        KpI += iStep;
        flyWheelMotors(80, 80);
        wait1Msec(2000);
    }
    writeDebugStreamLine("BEST I IS ");
    writeDebugStreamLine("%f", bestI);
    KiL = bestI;
    /*******************/
/*
    while(KdL <= maxD) {
        clearTimer(T3);
        bool resetError = false;
        while(time1[T3] < timeWithValues)
        {
            wait1Msec(encoderTimer);
            pidChange(rpmGoal + powerBias);
            normalizeFlyPower();
            flyWheelMotors(flySpeedLeft, flySpeedRight);
            if(!resetError)
            {
                averageLeftError = 0;
                averageRightError = 0;
                resetError = true;
            }
        }

        float error = ((averageLeftError + averageRightError) / 2.0)

        writeDebugStreamLine("%f", KdL:);
        writeDebugStreamLine("%f", error);

        if(error < bestError)
        {
            bestD = KpD;
        }
        KpD += dStep;
        flyWheelMotors(80, 80);
        wait1Msec(2000);
    }
    writeDebugStreamLine("BEST D IS ");
    writeDebugStreamLine("%f", bestD);
    KdL = bestD;*/

    writeDebugStreamLine("****************************");
    writeDebugStreamLine("BEST COMBO IS");
    writeDebugStreamLine("%f", bestP);
    writeDebugStreamLine("%f", bestI);
    writeDebugStreamLine("%f", bestD);
  }
