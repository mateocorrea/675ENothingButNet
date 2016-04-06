bool useGyroTurn = false;
int autoShootSpeed = 127;

int redShootNum = 0;
int autoTwo = 1;
int autoThree = 2;
int autoFour = 3;
int autoFive = 4;
int autoSix = 5;

void runAuto(int chosen);
void redShoot();
void redSide();
void blueBot();
void blueSide();
void defense();
void progSkills();

void runAuto(int chosen) {
	/* Start tasks needed in autonomous and disable unnecessary ones */
	startTask(flyWheelPower);
	/* Run the selected autonomous program */
	if(chosen == 0)
		redShoot();
	else if(chosen == 1)
		redSide();
	else if(chosen == 2)
		blueBot();
	else if(chosen == 3)
		blueSide();
	else if(chosen == 4)
		defense();
	else
		progSkills();
}

void redShoot()
{
	//flyWheelOn = true;
	//rpmGoal = rpmMid;
	flyWheelOn = false;
	followLineForClicks(110, 2000);
	/*fdriveDistance(621);
	wait1Msec(250);
	encoderTurn(120);
	wait1Msec(250);
	drivePowerForClicks(100, 300);
	motor[roller] = 127;
	motor[conveyor] = 127;
	drivePowerForClicks(80, 850);
	motor[conveyor] = 0;
	driveDistance(700);
	autoShooting = true;
	motor[roller] = 127;
	wait1Msec(20);
	motor[conveyor] = 127;
	wait1Msec(2000);

	autoShooting = false;
	rpmGoal = rpmLow;
	wait1Msec(500);
	driveDistance(1100);
	autoShooting = true;
	wait1Msec(2500);
	autoShooting = false;

	driveDistance(-200);
	wait1Msec(100);
	encoderTurn(300);
	wait1Msec(100);
	driveDistance(600);*/
}

void redSide()
{
	rpmGoal = rpmHigh;
	flyWheelOn = true;
	motor[roller] = 127;
	wait1Msec(3000);

	for(int i = 0; i < 4; i++) {
		motor[conveyor] = 60;
		wait1Msec(200);
		motor[conveyor] = 0;
		wait1Msec(500);
	}

	encoderTurn(-100);
	driveDistance(621);
	wait1Msec(250);
	encoderTurn(132);
	wait1Msec(250);
	drivePowerForClicks(100, 300);
	motor[roller] = 127;
	motor[conveyor] = 127;
	drivePowerForClicks(80, 800);
	motor[conveyor] = 0;
	driveDistance(1000);
	motor[conveyor] = 127;
}

void blueBot()
{
}

void blueSide()
{
	motor[roller] = 0;
	/* Drive away from tile and turn towards the pile of balls */
	driveDistance(-420);
	wait1Msec(300);
	if(useGyroTurn)
		gyroTurn(150);
	else
		encoderTurn(230);
	wait1Msec(300);
	driveDistance(-590);
	wait1Msec(300);
	if(useGyroTurn)
		gyroTurn(-400);
	else
		encoderTurn(-345);

	/* Drive towards the piles and pick them up */
	driveDistance(330);
	wait1Msec(100);
	motor[roller] = 127;
	wait1Msec(500);
	motor[conveyor] = 127;
	wait1Msec(300);
	motor[roller] = 0;
	wait1Msec(100);

	drivePower(-80, -80);
	wait1Msec(120);
	motor[roller] = 127;
	drivePower(100,100);
	wait1Msec(260);
	drivePower(0,0);
	wait1Msec(300);
	motor[conveyor] = 0;

	/* Drive away from the wall and turn towards the goal */
	driveDistance(-250);
}

void defense()
{
	driveDistance(-2000);
	encoderTurn(900);
	driveDistance(2000);
}

void progSkills()
{
	useGyroTurn = true;

	/* Shoot the first batch of preloads for 17.5 seconds */
	clearTimer(T1);
	motor[conveyor] = autoShootSpeed;
	while(time1[T1] < 18000){
	}
	motor[conveyor] = 0;

	/* Drive a litle bit away from the tile */
	driveDistance(-200); // move away from
	wait1Msec(200);

	/* Turn towards the blue tile */
	if(useGyroTurn)
		gyroTurn(-1134);
	else
		encoderTurn(-950);

	/* Drive to the other tile */
	driveDistance(2300);

	/* Align with the wall */
	drivePower(100, 100);
	wait1Msec(800);
	drivePower(0,0);
	wait1Msec(300);

	/* Back up and turn towards goal */
	driveDistance(-350);
	wait1Msec(500);
	if(useGyroTurn)
		gyroTurn(380);
	else
		encoderTurn(345);

	/* Shoot */
	motor[conveyor] = autoShootSpeed;
}
