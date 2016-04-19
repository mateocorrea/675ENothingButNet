#define lock SensorValue[lockPistons]
#define deployer SensorValue[deployerPistons]
#define brake SensorValue[brakePistons]
#define gyro SensorValue[gyroSensor]
#define transmission SensorValue[transPiston]
#define leftDriveEnc nMotorEncoder[leftDrive]
#define rightDriveEnc nMotorEncoder[rightDrive]
#define accelX SensorValue[accelerometerX]
#define accelY SensorValue[accelerometerY]
#define rampBtn vexRT[Btn7D]
#define rampBtn2 vexRT[Btn7L]
#define brakeBtn vexRT[Btn5U]
#define transBtn vexRT[Btn8U]
#define puncherBtn vexRT[Btn6U]
#define puncherDelayBtn vexRT[Btn5D]

bool lifted = false;
bool braking = false;
bool punchersActivated = false;
bool punchersOn = false;
int threshold = 15;
int liftCount = 0;
int oldRight = 0;
int oldLeft = 0;

task drive();
task pneumatics();
void drivePower(int left, int right);
void releaseBrake();
void actuateBrake();
void shiftTransmission();
void deploy();
void encoderTurn(int goal);
void gyroTurn(int goal);
void gyroTurnTo(int goal, int direction);
void transPower(int left, int right);
void driveDistance(int goal);

task drive()
{
	bool lastPuncherBtn = false;
	while(true) {
		drivePower(vexRT(Ch3), vexRT(Ch2));

		if(punchersActivated) {
			if(puncherBtn && !lastPuncherBtn) {
				punchersOn = !punchersOn;
				lastPuncherBtn = true;
			}	else if(!puncherBtn)
				lastPuncherBtn = false;
			transPower(127 * punchersOn, 127 * punchersOn);
		}
	}
}

void drivePowerForClicks(int power, int goal)
{
	leftDriveEnc = 0;
	rightDriveEnc = 0;

	int straightGyro = gyro;

	while(!(abs(leftDriveEnc) > (abs(goal))) || !(abs(rightDriveEnc) > (abs(goal)))) {
		int leftPower = power;
		int rightPower = power;

		int dir = (goal < 0) ? -1 : 1;

		if(gyro < (straightGyro))
			leftPower -= (abs(straightGyro - gyro)/4) * dir;
		else if (gyro > (straightGyro))
			rightPower -= (abs(straightGyro - gyro)/4) * dir;

		leftPower = (goal < 0) ? -leftPower : leftPower;
		rightPower = (goal < 0) ? -rightPower : rightPower;
		drivePower(leftPower, rightPower);
	}
}

task pneumatics()
{
	bool lastBrakeBtn = false;
	bool lastRampBtn = false;
	bool lastTransBtn = false;
	punchersActivated = false;
	transmission = 1;
	deployer = 0;
	lock = 0; // lock lift
	while(true) {
		/* Ramp */
		if(rampBtn == 1 && rampBtn2 == 1 && lastRampBtn == false) {
			if(liftCount == 0) {
				deploy();
				liftCount++;
			} else if(liftCount == 1) {
				lock = 1; // release lift
				lifted = true;
				liftCount++;
			} else {
				lock = 0; // lock lift
				liftCount = 0;
			}
			lastRampBtn = true;
		} else if (rampBtn == 0) {
			lastRampBtn = false;
		}

		/* Brakes */
		if(brakeBtn == 1 && lastBrakeBtn == false) {
			if(braking)
				releaseBrake();
			else
				actuateBrake();
			lastBrakeBtn = true;
		} else if (brakeBtn == 0) {
			lastBrakeBtn = false;
		}

		/* Puncher Activation */
		if(transBtn == 1 && !lastTransBtn) {
			shiftTransmission();
			lastTransBtn = true;
		}	else if(!transBtn)
			lastTransBtn = false;
		}
}

void shiftTransmission()
{
	//transmission = (punchersActivated) ? 1 : 0;
	//punchersActivated = !punchersActivated;
	if(punchersActivated)	{
		transmission = 1;
		punchersActivated = false;
	} else {
		transmission = 0;
		punchersOn = false;
		punchersActivated = true;
	}
}

void deploy()
{
	deployer = 1;
	wait1Msec(400);
	deployer = 0;
}

void actuateBrake()
{
	brake = 1;
	braking = true;
}

void releaseBrake()
{
	brake = 0;
	braking = false;
}

void driveDistance(int goal)
{
	float driveKp = 0.45;
	int leftDriveError;
	int rightDriveError;
	int maxPower = 120;
	leftDriveEnc = 0;
	rightDriveEnc = 0;
	int lastValue = 0;
	clearTimer(T1);

	int straightGyro = gyro;

	while(!(abs(leftDriveEnc) > abs(goal)) || !(abs(rightDriveEnc) > abs(goal))) {
		/*if(userControl)
			break;*/
		/*if(time1[T1] > abs(goal) * 5)
			break;*/

		// Proportional
		leftDriveError = (abs(goal) - abs(leftDriveEnc));
		rightDriveError = (abs(goal) - abs(rightDriveEnc));

		// PID
		int leftPidDrive = round(driveKp * leftDriveError);
		int rightPidDrive = round(driveKp * rightDriveError);

		int bias = 0;
		if(lastValue == (abs(leftDriveEnc) + abs(rightDriveEnc)))
			bias = 20;
		leftPidDrive = (abs(leftPidDrive) > maxPower) ? maxPower : leftPidDrive+bias; // limit to a maxPower
		rightPidDrive = (abs(rightPidDrive) > maxPower) ? maxPower : rightPidDrive+bias;

		if(gyro < (straightGyro))
			leftPidDrive -= (abs(straightGyro - gyro)/3);
		else if (gyro > (straightGyro))
			rightPidDrive -= (abs(straightGyro - gyro)/3);

		leftPidDrive = (goal < 0) ? -leftPidDrive : leftPidDrive;
		rightPidDrive = (goal < 0) ? -rightPidDrive : rightPidDrive;
		drivePower(leftPidDrive, rightPidDrive);
		lastValue = (abs(leftDriveEnc) + abs(rightDriveEnc));
	}
	int brakePower = 20;
	int brakeTime = 50;
	if(goal < 0)
		drivePower(brakePower, brakePower);
	else
		drivePower(-brakePower, -brakePower);
	wait1Msec(brakeTime);
	drivePower(0, 0);
}

void encoderTurn(int goal)
{
	float driveKp = 0.80;
	int leftDriveError;
	int rightDriveError;
	int maxPower = 110;
	leftDriveEnc = 0;
	rightDriveEnc = 0;
	clearTimer(T1);
	int lastValue = 0;

	while(!(abs(leftDriveEnc) > (abs(goal))) || !(abs(rightDriveEnc) > (abs(goal)))){
		lastValue = abs(leftDriveEnc) + abs(rightDriveEnc);

		// Proportional
		leftDriveError = (abs(goal) - abs(leftDriveEnc));
		rightDriveError = (abs(goal) - abs(rightDriveEnc));

		// PID
		int leftPidDrive = round(driveKp * leftDriveError);
		int rightPidDrive = round(driveKp * rightDriveError);
		leftPidDrive = (abs(leftPidDrive) > maxPower) ? maxPower : leftPidDrive; // limit to a maxPower
		rightPidDrive = (abs(rightPidDrive) > maxPower) ? maxPower : rightPidDrive;

		int bias = 0;
		if(lastValue == (abs(leftDriveEnc) + abs(rightDriveEnc)))
			bias = 20;
		rightPidDrive = (goal < 0) ? -rightPidDrive-bias : rightPidDrive+bias;
		leftPidDrive = (goal < 0) ? leftPidDrive+bias : -leftPidDrive-bias;

		drivePower(leftPidDrive, rightPidDrive);
		lastValue = (abs(leftDriveEnc) + abs(rightDriveEnc));
	}
	int brakePower = 25;
	int brakeTime = 40;
	if(goal > 0)
		drivePower(brakePower, -brakePower);
	else
		drivePower(-brakePower, brakePower);
	wait1Msec(brakeTime);
	drivePower(0,0);
}

void drivePower(int left, int right)
{
	int leftPower = (abs(left) < threshold) ? 0 : left;
	int rightPower = (abs(right) < threshold) ? 0 : right;

	if((rightPower != 0) || (leftPower != 0)) {
		releaseBrake();
	}

	if(userControl) {
		int maxDif = 3;
		if(abs(left - oldLeft) > maxDif) {
			if(left > oldLeft)
				leftPower = oldLeft + maxDif;
			else
				leftPower = oldLeft - maxDif;
		}
		if(abs(right - oldRight) > maxDif) {
			if(right > oldRight)
				rightPower = oldRight + maxDif;
			else
				rightPower = oldRight - maxDif;
		}
	}

	motor[leftDrive] = leftPower;
	motor[rightDrive] = rightPower;
	if(!punchersActivated)
		transPower(leftPower, rightPower);

	oldLeft = leftPower;
	oldRight = rightPower;
}

void gyroTurnTo(int goal, int direction)
{
	int difference = goal - SensorValue[gyroSensor];
	if(direction < 0)
		gyroTurn(-difference);
	else
		gyroTurn(difference);
}

void transPower(int left, int right)
{
  int dir = (punchersActivated) ? -1 : 1;
	motor[topLeftTrans] = left * dir;
	motor[botLeftTrans] = left * dir;
	motor[topRightTrans] = right * dir;
	motor[botRightTrans] = right * dir;
}

void gyroTurn(int goal)
{
	gyro = 0;
	while(abs(gyro) < abs(goal))
	{
		if(userControl)
			break;
		int difference = abs(goal) - abs(gyro);
		int power = difference / 2;
		if(goal > 0)
			drivePower(-power, power);
		else
			drivePower(power, -power);
	}
	int brakePower = 40;
	int brakeTime = 50;
	if(goal > 0)
		drivePower(brakePower, -brakePower);
	else
		drivePower(-brakePower, brakePower);
	wait1Msec(brakeTime);
	drivePower(0,0);
}

void followLineForClicks(int power, int goal)
{
	int dir = (goal < 0) ? -1 : 1;
	int black = 2820;
	leftDriveEnc = 0;
	rightDriveEnc = 0;
	int leftIntegral = 0;
	int rightIntegral = 0;
	int oldLeftError = 0;
	int oldRightError = 0;
	float lineP = 0.2;
	float lineI = 0.00;
	float lineD = 0.0;
	bool lastRight = false;

	while(abs(leftDriveEnc) < abs(goal)) {
		int leftPower = power;
		int rightPower = power;
		int leftError = (SensorValue[leftLine] < black) ? abs(SensorValue[leftLine] - black) : 0; // how much white it has
		int rightError = (SensorValue[rightLine] < black) ? abs(SensorValue[rightLine] - black) : 0; // how much white it has
		int midError = (SensorValue[midLine] < black) ? abs(SensorValue[midLine] - black) : 0;

		if(leftError == 0)
			leftIntegral = 0;
		if(rightError == 0)
			rightIntegral = 0;

		if(rightError > leftError)
			lastRight = true;
		else
			lastRight = false;

		if( (leftError == 0) && (rightError == 0) && (midError == 0) ) { // ironically, no error is huge error
			if(lastRight)
				rightPower -= 50 * dir;
			else
				leftPower -= 50 * dir;
		}	else if(!((leftError == 0) && (rightError == 0))) { // right or left has some white
			rightIntegral += rightError;
			leftIntegral += leftError;
			leftPower -= (leftError*lineP + leftIntegral*lineI + (leftError-oldLeftError)*lineD) * dir;
			rightPower -= (rightError*lineP + rightIntegral*lineI + (rightError-oldRightError)*lineD) * dir;
		}
		drivePower(leftPower*dir, rightPower*dir);
		oldLeftError = leftError;
		oldRightError = rightError;
	}
}
