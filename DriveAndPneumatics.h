#define lock SensorValue[lockPistons]
#define deployer SensorValue[deployerPiston]
#define brake SensorValue[brakePistons]

#define rampBtn vexRT[Btn7D]
#define rampBtn2 vexRT[Btn7L]
#define brakeBtn vexRT[Btn5U]
bool lastRampBtn = false;

task drive();
task pneumatics();
void drivePower(int left, int right);
void turn(int degrees);
void releaseBrake();
void actuateBrake();
void releaseLift();
void lockLift();
void deploy();

const int threshold = 12;
int liftCount = 0;
bool braking = false;

task drive()
{
	while(true)
		drivePower(vexRT(Ch3), vexRT(Ch2));
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
			if(liftCount == 0)
			{
				deploy();
				actuateBrake();
				liftCount++;
			}
			else if(liftCount == 1)
			{
				releaseLift();
				liftCount++;
				PlayMario();
			}
			else {
				lockLift();
				liftCount = 0;
			}
			lastRampBtn = true;
		} else if (rampBtn == 0) {
			lastRampBtn = false;
		}

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

void turn(int degrees)
{
  //While the absolute value of the gyro is less than the desired rotation - 100...
  while(abs(SensorValue[in8]) < degrees - 100)
  {
    drivePower(-90, 90);
  }
  //Brief brake to eliminate some drift
  drivePower(5, -5);
  wait1Msec(100);
  //Second while loop to move more slowly toward the goal
  while(abs(SensorValue[in8]) != degrees)
  {
    if(abs(SensorValue[in8]) > degrees)
    {
      drivePower(30, -30);
    }
    else
    {
      drivePower(-30, 30);
    }
  }
  //Stop
  drivePower(0,0);
}


void drivePower(int left, int right)
{
	if(abs(left) < threshold)
		left = 0;
	if(abs(right) < threshold)
		right = 0;
	motor[rightback] = right;
	motor[rightfront] = right;
	motor[leftback] = left;
	motor[leftfront] = left;
}
