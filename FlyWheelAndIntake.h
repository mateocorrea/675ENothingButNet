#define flywheelEncoder nMotorEncoder[leftFlywheel]
#define speedBtn vexRT[Btn5D]
#define intakeBtn vexRT[Btn6U]
#define outtakeBtn vexRT[Btn6D]
#define flyWheelBtn vexRT[Btn7U]
#define lowerBtn vexRT[Btn7R]
#define raiseBtn vexRT[Btn8L]
#define rollerBtn vexRT[Btn8R]
#define rollerOutBtn vexRT[Btn8D]
#define intakeLimit SensorValue[limitSwitch]
//define T2 flywheelTimer
//define T3 longShotTimer

///////////////// FLYWHEEL CONSTANTS ////////////////////
int rpmLow = 70;
int rpmMid = 84;
int rpmHigh = 110;
int lowSpeed = 43;
int midSpeed = 60;
int highSpeed = 50;
/////////////////////////////////////////////////////////
float Kp = 1.0;
float Ki = 1.0;
float Kd = 1.0;
/////////////////////////////////////////////////////////
float KpStable = 0.3300500;
float KiStable = 0.000000003;
float KdStable = 0.2;
/////////////////////////////////////////////////////////
float KpLow = 1.9375; //0.9075
float KiLow = 0.0030;
float KdLow = 0.2;
/////////////////////////////////////////////////////////
float KpMid = 0.95;
float KiMid = 0.090000;
float KdMid = 0.00;
/////////////////////////////////////////////////////////
float KpHigh = 0.400000;
float KiHigh = 0.00000;
float KdHigh = 0.00000;


//            VARIABLES AND FUNCTIONS                  //
/////////////////////////////////////////////////////////
float rpmGoal = rpmMid;
float rpm = 0.0;
float averageError = 0.0;
float flySpeed = 0.0;
/////////////////////////////////////////////////////////
bool flyWheelOn = false;
bool autoShooting = false;
bool rollerOn = true;
bool startup = true; // If false, flywheel is running, else flywheel is off
float oldError = 0.0;
float integral = 0.0;
float lastTime = 0;
float oldFilter = 0.0;
int lastFlyWheelTime = 0;
int removableTime = 0;
int MAX_POWER = 127;
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
	while(true) {
		if(flyWheelOn) {
			if(startup)
				slowStart();
			startup = false;

			wait1Msec(encoderTimer);
			setPIDConstants();
			pidChange(rpmGoal + powerBias());
			flyWheelMotors(flySpeed);
		}
		else {
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
	bool justSwitchedLongShot = false;
	bool lastRaiseBtn = false;
	bool lastLowerBtn = false;
	bool lastSpeedBtn = false;

	while(true)
	{
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
		// SOMETHINB ABOUT BURBINE (2.163)
		// MATEO PRESENTS IN LLLH TO BUZZERD AND ROBBINS
		// ANDREI S., HANVILE Z. TRAN Q.,
		// Flywheel speed selection //
		if(!speedBtn) {
			clearTimer(T3);
			justSwitchedLongShot = false;
		}

		//holding
		if((time1[T3] >= 750) && !justSwitchedLongShot) {
			flySpeed = highSpeed;
			rpmGoal = rpmHigh;
			justSwitchedLongShot = true;
		}
		//pressing
		if((time1[T3] < 750) && speedBtn && !lastSpeedBtn)	{
			if(rpmGoal == rpmLow) {
				flySpeed = midSpeed;
				rpmGoal = rpmMid;
			} else if (rpmGoal == rpmMid) {
				/*flySpeed = highSpeed;
				rpmGoal = rpmHigh;*/
				flySpeed = lowSpeed;
				rpmGoal = rpmLow;
			} else {
				flySpeed = lowSpeed;
				rpmGoal = rpmLow;
			}
			lastSpeedBtn = true;
		} else if ( speedBtn == 0 )
			lastSpeedBtn = false;

		/* Power Bias */
		if(raiseBtn == 1 && lastRaiseBtn == false) {
			powerBias(1);
			lastRaiseBtn = true;
		} else if (raiseBtn == 0)
			lastRaiseBtn = false;
		if(lowerBtn == 1 && lastLowerBtn == false) {
			powerBias(-1);
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
		intakeSpeed = (rpmGoal == rpmMid) ? 80 : 127;
        if(intakeBtn && outtakeBtn)
            motor[conveyor] = intakeSpeed * !punchersActivated;
        else
            motor[conveyor] = intakeSpeed * ((intakeBtn * !intakeLimit) - outtakeBtn) * !punchersActivated;

		/* Roller Power */
		if(rollerBtn == 1 && lastRollerBtn == false)
		{
			rollerOn = !rollerOn;
			lastRollerBtn = true;
		} else if (rollerBtn == 0)
			lastRollerBtn = false;
		motor[roller] = (rollerOutBtn) ? -127 : (127 * rollerOn * !punchersActivated);
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
		maxPower = 126;
	} else if (rpmGoal == rpmMid) {
		minPower = 40;
		maxPower = MAX_POWER;
	} else if (rpmGoal == rpmHigh) {
		minPower = 65;
		maxPower = MAX_POWER;
	} else {
		minPower = MIN_POWER;
		maxPower = MAX_POWER;
	}
	if((power < minPower) && flyWheelOn) {
		power = minPower;
		flySpeed = minPower;
	}
	if(flySpeed > maxPower) {
		power = maxPower;
		flySpeed = maxPower;
	}
	int x = round(power);
	motor[rightFlywheel] = x;
	motor[leftFlywheel] = x;
}
void pidChange(int goal)
{
	int deltaTime = ((nSysTime - lastTime) > 0) ? abs(nSysTime - lastTime - removableTime) : encoderTimer;
	removableTime = 0;
	if(deltaTime < 1)
		deltaTime = 1;
	float factor = ( 60000 / deltaTime ) / ticksPerTurnSpeed;
	rpm = abs(flywheelEncoder * factor);
	float error = voltageCorrection(goal) - rpm;
	if((vexRT[Btn6U] && !vexRT[Btn8U]))
		averageRPMError(abs(error));

	////// Proportional /////
  //float p = (Kp * pow(5, (abs(error)/20))) * error;

	////// Integral //////
	integral = integral + (error * deltaTime);
	if( ((integral > 0) && (error < 0)) || ((integral < 0) && (error > 0)) )
		integral = error * deltaTime;

	////// Derivative /////
	float derivative = (error - oldError) / deltaTime;

	// PID //
	float change = (Kp * error) + (Ki * integral) + (Kd * derivative);
	//float change = p + (Ki * integral) + (Kd * derivative);


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
	if(userControl)
		autoShooting = false;
	if(!intakeBtn && !autoShooting) {
		Kp = KpStable;
		Ki = KiStable;
		Kd = KdStable;
	} else {
		if(rpmGoal == rpmLow) {
			Kp = KpLow;
			Ki = KiLow;
			Kd = KdLow;
		} else if (rpmGoal == rpmMid) {
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
