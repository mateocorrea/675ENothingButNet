#define flywheelEncoder nMotorEncoder[leftFlywheel]
#define speedBtn vexRT[Btn5D]
#define intakeBtn vexRT[Btn6U]
#define outtakeBtn vexRT[Btn6D]
#define flyWheelBtn vexRT[Btn7U]
#define lowerBtn vexRT[Btn7R]
#define raiseBtn vexRT[Btn8L]
#define rollerBtn vexRT[Btn8R]
#define rollerOutBtn vexRT[Btn8D]

///////////////// FLYWHEEL CONSTANTS ////////////////////
int rpmLow = 102;
int rpmMid = 120;
int rpmHigh = 158;
int lowSpeed = 40;
int midSpeed = 55;
int highSpeed = 80;
/////////////////////////////////////////////////////////
float Kp = 1.0;
float Ki = 1.0;
float Kd = 1.0;
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
float rpm = 0.0;
float averageError = 0.0;
float flySpeed = 0.0;
/////////////////////////////////////////////////////////
bool flyWheelOn = false;
bool rollerOn = true;
bool startup = true; // If false, flywheel is running, else flywheel is off
float oldError = 0.0;
float integral = 0.0;
float lastTime = 0;
float oldFilter = 0.0;
int lastFlyWheelTime = 0;
int removableTime = 0;
int MAX_POWER = 120;
int MIN_POWER = 35;
int maxPower = MAX_POWER;
int minPower = MIN_POWER;
int encoderTimer = 25;
int ticksPerTurnSpeed = 372;
int batteryValues = 0;
int averageBattery = 0;
int errorValues = 0;
int intakeSpeed = 127;
int lowPowerBias = 0;
int midPowerBias = 0;
int highPowerBias = 0;
int red = 0;
int green = 1;
int yellow = 2;
/////////////////////////////////////////////////////////
int powerBias();
int voltageCorrection(int rpm);
task flyWheelControl();
task flyWheelPower();
task intake();
void turnOn(int color);
void flyWheelMotors(float power);
void pidChange(int goal);
void averageRPMError(float error);
void setPIDConstants();
void slowStart();
void slowStop();
void resetFlyWheel();
void powerBias(int change);
/////////////////////////////////////////////////////////

task flyWheelPower() {
	resetFlyWheel();
	while(true)
	{
		if(flyWheelOn)
		{
			if(startup)
				slowStart();
			startup = false;

			wait1Msec(encoderTimer);
			setPIDConstants();
			pidChange(rpmGoal + powerBias());
			flyWheelMotors(flySpeed);
		}
		else
		{
			if(!startup)
				slowStop();
			startup = true;

			flyWheelMotors(0);
		}
	}
}

task flyWheelControl()
{
	lastFlyWheelTime = nSysTime;
	bool justSwitchedFlywheel = false;
	bool lastRaiseBtn = false;
	bool lastLowerBtn = false;
	bool lastSpeedBtn = false;

	while(true)
	{
		if(!flyWheelBtn)
		{
			clearTimer(T2);
			justSwitchedFlywheel = false;
		}
		if((time1[T2] >= 1000) && !justSwitchedFlywheel)
		{
			flyWheelOn = !flyWheelOn;
			justSwitchedFlywheel = true;
			if(flyWheelOn == true)
				resetFlyWheel();
		}

		// Flywheel speed selection //
		if(speedBtn && !lastSpeedBtn)
		{
			if(rpmGoal == rpmLow) {
				flySpeed = midSpeed;
				rpmGoal = rpmMid;
			} else if (rpmGoal == rpmMid) {
				flySpeed = highSpeed;
				rpmGoal = rpmHigh;
			} else {
				flySpeed = lowSpeed;
				rpmGoal = rpmLow;
			}
			lastSpeedBtn = true;
		} else if ( speedBtn == 0 )
			lastSpeedBtn = false;

		/* Power Bias */
		if(raiseBtn == 1 && lastRaiseBtn == false) {
			powerBias(2);
			lastRaiseBtn = true;
		} else if (raiseBtn == 0)
			lastRaiseBtn = false;
		if(lowerBtn == 1 && lastLowerBtn == false) {
			powerBias(-2);
			lastLowerBtn = true;
		} else if (lowerBtn == 0)
			lastLowerBtn = false;

		lastFlyWheelTime = nSysTime;
	}
}

task intake()
{
	bool lastRollerBtn = false;
	while(true) {
		/* Intake Power */
		motor[conveyor] = intakeSpeed * (intakeBtn - outtakeBtn);

		/* Roller Power */
		if(rollerBtn == 1 && lastRollerBtn == false)
		{
			rollerOn = !rollerOn;
			lastRollerBtn = true;
		} else if (rollerBtn == 0)
			lastRollerBtn = false;
		motor[roller] = (rollerOutBtn) ? -127 : (127 * rollerOn);
	}
}

void turnOn(int color) {
	SensorValue[redLED] = 1;
	SensorValue[greenLED] = 1;
	SensorValue[yellowLED] = 1;
	if(color == red)
		SensorValue[redLED] = 0;
	else if (color == green)
		SensorValue[greenLED] = 0;
	else if (color == yellow)
		SensorValue[yellowLED] = 0;
}
void flyWheelMotors(float power)
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
	if(power < minPower)
		power = minPower;
	if(flySpeed > maxPower)
		power = maxPower;
	int x = round(power);
	motor[rightFlywheel] = x;
	motor[leftFlywheel] = x;
}
void pidChange(int goal)
{
	float deltaTime = ((nSysTime - lastTime) > 0) ? abs(nSysTime - lastTime - removableTime) : encoderTimer;
	removableTime = 0;
	if(deltaTime < 1)
		deltaTime = 1;
	float factor = ( 60000 / deltaTime ) / ticksPerTurnSpeed;
	rpm = abs(flywheelEncoder * factor);
	float error = voltageCorrection(goal) - rpm;
	if((vexRT[Btn6U] && !vexRT[Btn8U]))
		averageRPMError(abs(error));

	////// Integral //////
	integral = integral + (error * deltaTime);
	if( ((integral > 0) && (error < 0)) || ((integral < 0) && (error > 0)) )
		integral = error * deltaTime;

	////// Derivative /////
	float derivative = (error - oldError) / deltaTime;

	// PID //
	float change = (Kp * error) + (Ki * integral) + (Kd * derivative);

	// Adjust Speed //
	flySpeed = (goal == 0) ? (flySpeed * oldFilter + ((1.0 - oldFilter) * change)) : flySpeed + change;

	// Reset the errors and the encoders
	oldError = error;
	flywheelEncoder = 0;
	lastTime = nSysTime;
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

void averageRPMError(float error)
{
	float sum = (errorValues * averageError) + error;
	errorValues++;
	averageError = sum / errorValues;
}

void slowStart()
{
	for(int i = 1; i < 46; i++)
	{
		flyWheelMotors(i * 1.0);
		wait1Msec(10);
	}
	if (rpmGoal == rpmMid) {
		flySpeed = midSpeed;
		} else if(rpmGoal == rpmLow) {
		flySpeed = lowSpeed;
		} else {
		rpmGoal = rpmHigh;
		flySpeed = highSpeed;
	}
	flyWheelMotors(flySpeed);
	wait1Msec(50);
	flywheelEncoder = 0;

	removableTime = 500;
}

void slowStop()
{
	int i = 45;
	if(rpmGoal == rpmLow)
		i = MIN_POWER;
	while(i >= 0) {
		flyWheelMotors(i * 1.0);
		wait1Msec(10);
		i--;
	}
}

void resetFlyWheel()
{
	if (rpmGoal == rpmMid)
		flySpeed = midSpeed;
	else if(rpmGoal == rpmLow)
		flySpeed = lowSpeed;
	else {
		rpmGoal = rpmHigh;
		flySpeed = highSpeed;
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
