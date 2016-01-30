#define lock SensorValue[lockPistons]
#define deployer SensorValue[deployerPiston]
#define brake SensorValue[brakePistons]
#define gyro SensorValue[gyroSensor]
#define leftDrive nMotorEncoder[leftfront]
#define rightDrive nMotorEncoder[rightfront]

#define rampBtn vexRT[Btn7D]
#define rampBtn2 vexRT[Btn7L]
#define brakeBtn vexRT[Btn5U]

bool braking = false;
bool cubicMapping = false;
bool lastRampBtn = false;
bool autoBrake = true;
int threshold = 12;
int liftCount = 0;
int stillTime = 0;
int autoBrakeTime = 3000;
int brakePower = 25;
int brakeTime = 35;

void encoderTurn(int goal);

int mapped(int x);
task drive();
task pneumatics();
void drivePower(int left, int right);
void driveBrake(int direction);
void releaseBrake();
void actuateBrake();
void releaseLift();
void lockLift();
void deploy();

task drive()
{
	int lastTime = nSysTime;
	while(true) {
		stillTime += nSysTime - lastTime;
		lastTime = nSysTime;
		drivePower(vexRT(Ch3), vexRT(Ch2));
	}
}

task pneumatics()
{
	bool lastBrakeBtn = false;
	deployer = 0;
	lockLift();
	actuateBrake();
	while(true)
	{
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
		if((abs(vexRT[Ch2]) > threshold) || (abs(vexRT[Ch3]) > threshold))
			releaseBrake();
		if((stillTime > autoBrakeTime) && (autoBrake == true))
			actuateBrake();
	}
}

void deploy()
{
	deployer = 1;
	wait1Msec(500);
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

        if(goal < 0) {
            leftPidDrive *= -1;
            rightPidDrive *= -1;
        }
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

        if(goal < 0) {
            rightPidDrive *= -1;
        } else {
        		leftPidDrive *= -1;
        }

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

	if(abs(left) < threshold)
		left = 0;
	if(abs(right) < threshold)
		right = 0;
	if((abs(right) >= threshold) || (abs(left) >= threshold))
		stillTime = 0;
    if (cubicMapping) {
        left = mapped(left);
        right = mapped(right);
    }
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


task straightControl() {
    while(true) {
        //check if both the motors are running on max power
        // if leaning, then make sure to correct itself
    }
}







