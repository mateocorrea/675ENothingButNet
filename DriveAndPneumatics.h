#define lock SensorValue[lockPistons]
#define deployer SensorValue[deployerPiston]
#define brake SensorValue[brakePistons]
#define gyro SensorValue[gyroSensor]
#define leftDrive nMotorEncoder[leftfront]
#define rightDrive nMotorEncoder[rightfront]
#define accelX SensorValue[accelerometerX]
#define accelY SensorValue[accelerometerY]

#define leftVertStick vexRT[Ch3]
#define rightVertStick vexRT[Ch2]

#define rampBtn vexRT[Btn7D]
#define rampBtn2 vexRT[Btn7L]
#define brakeBtn vexRT[Btn5U]

bool braking = false;
bool cubicMapping = false;
bool lastRampBtn = false;
bool autoBrake = false;
const int X = 0;
const int Y = 1;
int threshold = 15;
int liftCount = 0;
int stillTime = 0;
int autoBrakeTime = 3000;
int brakePower = 25;
int brakeTime = 35;
int accelXBias = 0;
int accelYBias = 0;

int mapped(int x);
task drive();
task calculateAccelBiases();
task pneumatics();
task straightControl();
void drivePower(int left, int right);
void driveBrake(int direction);
void releaseBrake();
void actuateBrake();
void releaseLift();
void lockLift();
void deploy();
void encoderTurn(int goal);
task automaticBrakingSystem();
int tolerableAccel(int direction);

float initialPosX = 0.0;
float initialPosY = 0.0;
float positionX = initialPosX;
float positionY = initialPosY;

float maxSpeed = 0.0;
float distanceX = 0.0;
float distanceY = 0.0;

bool firstBrakeCheck = true;

task drive()
{
	int lastTime = nSysTime;
	while(true) {

		drivePower(vexRT(Ch3), vexRT(Ch2));
	}
}

task pneumatics()
{
	bool lastBrakeBtn = false;
	deployer = 0;
	lockLift();
	while(true) {
		/* Ramp */
		if(rampBtn == 1 && rampBtn2 == 1 && lastRampBtn == false) {
			if(liftCount == 0) {
				deploy();
				liftCount++;
			} else if(liftCount == 1) {
				releaseLift();
				liftCount++;
			} else {
				lockLift();
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

	}
}

void deploy()
{
	deployer = 1;
	wait1Msec(400);
	deployer = 0;
}

void lockLift()
{
	lock = 0;
}

void releaseLift()
{
	lock = 1;
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
	firstBrakeCheck = true;
}


/*void gyroTurn(int goal){
    float gyroKp = 0.6;
    float gyroKd = 0.2;

    int gyroError;
    int gyroLastError;
    int gyroDerivative;
    int maxPower = 115;
    int allowableError = 0;
    int pidDrive;
    while(true){
    		// Proportional
        gyroError = abs(goal - gyro);
        // Derivative
        gyroDerivative = gyroError - gyroLastError;
        gyroLastError = gyroError;
        // PID
        pidDrive = round(((gyroKp * gyroError) + (gyroKd * gyroDerivative)));
        pidDrive = (abs(pidDrive) > maxPower) ? maxPower : pidDrive; // limit to a maxPower
    		if(goal > 0)
    		{
    			if(gyro > goal - allowableError)
    				break;
    			drivePower(-pidDrive, pidDrive);
    		}
    		else
    		{
    			if(gyro < goal + allowableError)
    				break;
    			drivePower(pidDrive, -pidDrive);
    		}
    }
    int tempBrakePower = (goal > 0) ? brakePower : -brakePower;
    drivePower(tempBrakePower, -tempBrakePower);
    wait1Msec(brakeTime);
    drivePower(0, 0);

}*/

/*void gyroTurn(int goal){
    float gyroKp = 0.6;
    float gyroKd = 0.0;

    int gyroError;
    int gyroLastError;
    int gyroDerivative;
    int maxPower = 115;
    int pidDrive;
    SensorValue[gyro] = 0;

    while(abs(SensorValue[gyro]) < abs(goal))
    {
    	// Proportional
        gyroError = abs(goal) - abs(gyro);
        // Derivative
        gyroDerivative = gyroError - gyroLastError;
        gyroLastError = gyroError;
        // PID
        pidDrive = round(((gyroKp * gyroError) + (gyroKd * gyroDerivative)));
        pidDrive = (abs(pidDrive) > maxPower) ? maxPower : pidDrive; // limit to a maxPower
        //pidDrive = 99;
    		if(goal > 0)
    		{
    			drivePower(-pidDrive, pidDrive);
    		}
    		else
    		{
    			drivePower(pidDrive, -pidDrive);
    		}
    		wait1Msec(5);
    }
    if(goal > 0)
    	drivePower(brakePower, -brakePower);
   	else
   		drivePower(-brakePower, brakePower);

    wait1Msec(brakeTime);
    drivePower(0, 0);

}*/

void gyroTurn(int goal)
{
	gyro = 0;
	while(abs(gyro) < abs(goal))
    {
        int difference = abs(goal) - abs(gyro);
        int power = difference / 2;
        if(goal > 0) {
            drivePower(-power, power);
        } else {
            drivePower(power, -power);
        }
    }

    if(goal > 0) {
        drivePower(brakePower, -brakePower);
    } else {
  		drivePower(-brakePower, brakePower);
    }
    wait1Msec(brakeTime);
    drivePower(0,0);
}

void driveDistance(int goal)
{
    float driveKp = 0.35;
    float driveKd = 0.0;

    int leftDriveError;
    int leftDriveLastError;
    int leftDriveDerivative;
    int rightDriveError;
    int rightDriveLastError;
    int rightDriveDerivative;
    int maxPower = 105;
    int allowableError = 0;

    leftDrive = 0;
    rightDrive = 0;
    clearTimer(T1);

    while(!(abs(leftDrive) > abs(goal) - allowableError) || !(abs(rightDrive) > abs(goal) - allowableError)){
        if(time1[T1] > abs(goal) * 3)
            break;

        // Proportional
        leftDriveError = (abs(goal) - abs(leftDrive));
        rightDriveError = (abs(goal) - abs(rightDrive));

        // Derivative
        leftDriveDerivative = leftDriveError - leftDriveLastError;
        rightDriveDerivative = rightDriveError - rightDriveLastError;
        leftDriveLastError = leftDriveError;
        rightDriveLastError = rightDriveError;

        // PID
        int leftPidDrive = round(((driveKp * leftDriveError) + (driveKd * leftDriveDerivative)));
        int rightPidDrive = round(((driveKp * rightDriveError) + (driveKd * rightDriveDerivative)));
        leftPidDrive = (abs(leftPidDrive) > maxPower) ? maxPower : leftPidDrive; // limit to a maxPower
        rightPidDrive = (abs(rightPidDrive) > maxPower) ? maxPower : rightPidDrive;

        leftPidDrive = (goal < 0) ? -leftPidDrive : leftPidDrive;
        rightPidDrive = (goal < 0) ? -rightPidDrive : rightPidDrive;

        drivePower(leftPidDrive, rightPidDrive);
    }
    driveBrake(goal);

    // check for overshoot


}

void encoderTurn(int goal)
{
    float driveKp = 0.38;
    float driveKd = 0.0;

    int leftDriveError;
    int leftDriveLastError;
    int leftDriveDerivative;
    int rightDriveError;
    int rightDriveLastError;
    int rightDriveDerivative;
    int maxPower = 105;
    int allowableError = 0;

    leftDrive = 0;
    rightDrive = 0;
    clearTimer(T1);

    while(!(abs(leftDrive) > abs(goal) - allowableError) || !(abs(rightDrive) > abs(goal) - allowableError)){
        if(time1[T1] > abs(goal) * 3)
            break;

        // Proportional
        leftDriveError = (abs(goal) - abs(leftDrive));
        rightDriveError = (abs(goal) - abs(rightDrive));

        // Derivative
        leftDriveDerivative = leftDriveError - leftDriveLastError;
        rightDriveDerivative = rightDriveError - rightDriveLastError;
        leftDriveLastError = leftDriveError;
        rightDriveLastError = rightDriveError;

        // PID
        int leftPidDrive = round(((driveKp * leftDriveError) + (driveKd * leftDriveDerivative)));
        int rightPidDrive = round(((driveKp * rightDriveError) + (driveKd * rightDriveDerivative)));
        leftPidDrive = (abs(leftPidDrive) > maxPower) ? maxPower : leftPidDrive; // limit to a maxPower
        rightPidDrive = (abs(rightPidDrive) > maxPower) ? maxPower : rightPidDrive;

        rightPidDrive = (goal < 0) ? -rightPidDrive : rightPidDrive;
        leftPidDrive = (goal < 0) ? leftPidDrive : -leftPidDrive;

        drivePower(leftPidDrive, rightPidDrive);
    }

    if(goal > 0) {
  		drivePower(brakePower, -brakePower);
    } else {
        drivePower(-brakePower, brakePower);
    }
    wait1Msec(brakeTime);
    drivePower(0,0);
}


void drivePower(int left, int right)
{
    left = (abs(left) < threshold) ? 0 : left;
    right = (abs(right) < threshold) ? 0 : right;

    if((abs(right) >= threshold) || (abs(left) >= threshold))
    	clearTimer(T4);

    if((abs(vexRT[Ch2]) >= threshold) || (abs(vexRT[Ch3]) >= threshold))
    {
        releaseBrake();
      	firstBrakeCheck == true;
   	}

    left = (cubicMapping) ? (mapped(left)) : left;
    right = (cubicMapping) ? (mapped(right)) : right;

	motor[rightback] = right;
	motor[rightfront] = right;
	motor[leftback] = left;
	motor[leftfront] = left;
}

// if direction < 0, should brake by moving motors forward
void driveBrake(int direction) {
    if(direction < 0)
        drivePower(brakePower, brakePower);
    else
        drivePower(-brakePower, -brakePower);
    wait1Msec(brakeTime);
    drivePower(0, 0);
}

int mapped(int x)
{
    return round(0.0001*x*x*x - 0.0095*x*x + 0.4605*x - 0.6284);
}

task automaticBrakingSystem() {
	while(true) {
		if(firstBrakeCheck)
			wait1Msec(1200);
    if((time1(T4) > autoBrakeTime) && (autoBrake == true))
        actuateBrake();
    if((abs(leftVertStick) < threshold) && (abs(rightVertStick) < threshold))
    {
        if((abs(accelX) > abs(tolerableAccel(X))) || (abs(accelY) > abs(tolerableAccel(Y))))
            actuateBrake();
    }
    firstBrakeCheck == false;
  }
}

int tolerableAccel(int direction)
{
    int allowableX = 50;
    int allowableY = 50;

    if(direction == X) {
        if(accelX > accelXBias)
            return accelXBias + allowableX;
        else
            return accelXBias - allowableX;
    } else {
        if(accelX > accelYBias)
            return accelYBias + allowableY;
        else
            return accelYBias - allowableY;
    }
}

task calculateAccelBiases()
{
    int xValues = 0;
    int yValues = 0;
    int xSum = 0;
    int ySum = 0;

    while(true) {
        xSum = (xValues * accelXBias) + accelX;
        ySum = (yValues * accelYBias) + accelY;

        accelXBias = round((xSum * 1.0) / (xValues+1));
        accelYBias = round((ySum * 1.0) / (yValues+1));

        xValues = (xValues >= 2000) ? 1000 : (xValues+1);
        yValues = (yValues >= 2000) ? 1000 : (yValues+1);

        wait1Msec(10);
    }
}


task straightControl() {
	int qer = 0;
    while(qer < 5) {
        //check if both the motors are running on max power
        // if leaning, then make sure to correct itself
    		qer++;
    }
}

task positionTracker() {
    float timeLapse = 25;
    float conversionFactor = 3.0;
    float oldSpeedX = 0.0;
    float oldSpeedY = 0.0;
    float oldAccelX = 0.0;
    float oldAccelY = 0.0;
	while(true) {
        float deltaTime = timeLapse;// FIX

		float trueAccelX = accelX * conversionFactor;
		float trueAccelY = accelY * conversionFactor;

		float speedX = oldSpeedX + ((trueAccelX + oldAccelX)/2.0 * deltaTime/1000);
		float speedY = oldSpeedY + ((trueAccelY + oldAccelY)/2.0 * deltaTime/1000);
		float speed = sqrt(pow(speedX,2) + pow(speedY,2));

        if(abs(speed) > maxSpeed)
            maxSpeed = abs(speed);

		float trueSpeedX = cos(gyro) * speed;
		float trueSpeedY = sin(gyro) * speed;

		float deltaX = ((trueSpeedX + oldSpeedX)/2.0) * (deltaTime/1000);
        float deltaY = ((trueSpeedY + oldSpeedY)/2.0) * (deltaTime/1000);

		positionX += deltaX;
		positionY += deltaY;
        distanceX += abs(deltaX);
        distanceY += abs(deltaY);

        oldAccelX = trueAccelX;
        oldAccelY = trueAccelY;
        oldSpeedX = trueSpeedX;
        oldSpeedY = trueSpeedY;

        wait1Msec(timeLapse);
	}
}

void turnToGoal() {
	float goalX = 0;
	float goalY = 12;
	float vectorX = positionX - goalX;
	float vectorY = positionY - goalY;
	float vectorMag = sqrt(pow(vectorX,2) + pow(vectorY, 2));

	float theta = asin(vectorY/vectorMag);

	while(gyro != theta) {
		drivePower(90, -90);
	}
	drivePower(0, 0);
}
