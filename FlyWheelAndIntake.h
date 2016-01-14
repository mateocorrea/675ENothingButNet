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

//            VARIABLES AND FUNCTIONS                  //
/////////////////////////////////////////////////////////
int rpmLow = 1080;
int rpmMid = 1200;
int rpmHigh = 1600;
float rpmGoal = rpmHigh;
float rpmLeft = 0.0;
float rpmRight = 0.0;
float averageLeftError = 0.0;
float averageRightError = 0.0;
float flySpeedLeft = 0.0;
float flySpeedRight = 0.0;
int lowSpeed = 40;
int midSpeed = 50;
int highSpeed = 70;
int flyWheelSpeed = highSpeed;
/////////////////////////////////////////////////////////
bool lastRaiseBtn = false;
bool lastLowerBtn = false;
bool lastSpeedBtn = false;
bool lastRollerBtn = false;
bool lastIntakeBtn = false;
bool flyWheelOn = false;
bool rollerOn = true;
bool rpmMode = true;
float launcherRatio = 10.2;
float oldLeftError = 0.0;
float oldRightError = 0.0;
float integralRight = 0.0;
float integralLeft = 0.0;
float lastTime = 0;
int flyWheelOnOffTime = 0;
int lastFlyWheelTime = 0;
int maxPower = 118;
int minPower = 35;
int encoderTimer = 25;
int ticksPerTurnSpeed = 372;
int batteryValues = 0;
int averageBattery = 0;
int rightValues = 0;


int leftValues = 0;

int powerBias = 0;
int red = 0;
int green = 1;
int yellow = 2;
int initialTime = 0;
/////////////////////////////////////////////////////////
int voltageCorrection(int rpm);
task flyWheelControl();
task flyWheelPower();
task intake();
void initFlyWheel();
void turnOn(int color);
void flyWheelMotors(float left, float right);
void pidChange(int rpmGoal);
void averageRPMError(float left, float right);
void normalizeFlyPower();
void setPIDConstants();
void slowStart();
/////////////////////////////////////////////////////////


//                 PID CONSTANTS                       //
/////////////////////////////////////////////////////////
float KpL = 1.0;
float KpR = 1.0;
float KiL = 1.0;
float KiR = 1.0;
float KdL = 1.0;
float KdR = 1.0;
/////////////////////////////////////////////////////////
float KpLow = 0.000900500;
float KiLow = 0.000000200;
float KdLow = 0.005000000;
float KpLowShooting = 0.01152500;
float KiLowShooting = 0.00000000;
float KdLowShooting = 0.92000000;
/////////////////////////////////////////////////////////
float KpMid = 0.01150000;
float KiMid = 0.01220000;
float KdMid = 0.002100000;
/////////////////////////////////////////////////////////
float KpHighRS = 0.030000000;
float KiHighRS = 0.0015;
float KdHighRS  = 0.012;//0.0025900;

/*
float KpHighRS = 0.030000000;
float KiHighRS = 0.0008;
float KdHighRS  = 0.012;//0.0025900;
*/
float KpHighLS = KpHighRS;
float KiHighLS = KiHighRS;
float KdHighLS = KdHighRS;
float KpHighL = KpHighRS;
float KiHighL = KiHighRS;
float KdHighL = KdHighRS;
float KpHighR = KpHighRS;
float KiHighR = KiHighRS;
float KdHighR = KdHighRS;
/////////////////////////////////////////////////////////

task flyWheelPower() {
	while(true)
	{
		if(flyWheelOn) {
			/*if(flySpeedLeft == 0)
				slowStart();*/
			wait1Msec(encoderTimer);
			setPIDConstants();
			pidChange(rpmGoal + powerBias);
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
	initFlyWheel();
	while(true) {

		// Flywheel On/Off //
		if(flyWheelBtn == 1) {
			flyWheelOnOffTime += nSysTime - lastFlyWheelTime;
		} else {
			flyWheelOnOffTime = 0;
			justSwitchedFlywheel = false;
		}
		if((flyWheelOnOffTime > 3000) && (!justSwitchedFlywheel))
		{
			flyWheelOn = !flyWheelOn;
			justSwitchedFlywheel = true;
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

		if(raiseBtn == 1 && lastRaiseBtn == false) {
			powerBias += 5;
			lastRaiseBtn = true;
		} else if (raiseBtn == 0) {
			lastRaiseBtn = false;
		}

		if(lowerBtn == 1 && lastLowerBtn == false) {
			powerBias -= 5;
			lastLowerBtn = true;
		} else if (lowerBtn == 0) {
			lastLowerBtn = false;
		}

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
		motor[chain] = 127 * (intakeBtn - outtakeBtn);

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

void flyWheelMotors(float left, float right)
{
	int l = round(left);
	int r = round(right);
	motor[topRightLauncher] = r;
	motor[topLeftLauncher] = l;
	motor[bottomRightLauncher] = r;
	motor[bottomLeftLauncher] = l;
}

void pidChange(int rpmGoal)
{
	float deltaTime = abs(nSysTime - lastTime);
	float factor = ( ( launcherRatio * 60000 ) / deltaTime ) / ticksPerTurnSpeed;
	rpmLeft = abs(flyEncLeft * factor);
	rpmRight = abs(flyEncRight * factor);
	float leftError = voltageCorrection(rpmGoal) - rpmLeft;
	float rightError = voltageCorrection(rpmGoal) - rpmRight;
	if(vexRT[Btn6U] && !vexRT[Btn8U])
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
	flySpeedLeft += leftChange;
	flySpeedRight += rightChange;

	//writeDebugStreamLine("%f", leftChange);
	//writeDebugStreamLine("%f", rightChange);

	// Reset the errors and the encoders
	oldLeftError = leftError;
	oldRightError = rightError;
	flyEncLeft = 0;
	flyEncRight = 0;
	lastTime = nSysTime;

	writeDebugStreamLine("%d, %f", nSysTime-initialTime, rpmLeft);
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

void setPIDConstants()
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

void normalizeFlyPower()
{
	if(flySpeedLeft < 0)
		flySpeedLeft *= -1;
	if(flySpeedRight < 0)
		flySpeedRight *= -1;
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

void initFlyWheel()
{
	initialTime = nSysTime;
	if(rpmGoal == rpmHigh)
		flyWheelSpeed = highSpeed;
	if(rpmGoal == rpmMid)
		flyWheelSpeed = midSpeed;
	if(rpmGoal == rpmLow)
		flyWheelSpeed = lowSpeed;
	flyWheelOn = true;
}
