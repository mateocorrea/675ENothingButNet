bool useGyroTurn = false;
int autoShootSpeed = 127;

int redShootNum = 0;
int autoTwo = 1;
int autoThree = 2;
int autoFour = 3;
int autoFive = 4;
int autoSix = 5;
int innerRedBarShot = 6;
int innerBlueBarShot = 7;
int outerRedBarShot = 8;
int outerBlueBarShot = 9;
int blueHoard = 10;
int redHoard = 11;

void runAuto(int chosen);
void redShoot();
void redSide();
void blueBot();
void blueSide();
void defense();
void progSkills();
void IRBS();
void IBBS();
void ORBS();
void OBBS();
void BH();
void RH();

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
	else if(chosen == innerBlueBarShot)
		IBBS();
	else if(chosen == innerRedBarShot)
		IRBS();
	else if(chosen == outerBlueBarShot)
		OBBS();
	else if(chosen == outerRedBarShot)
		ORBS();
	else if(chosen == blueHoard)
		BH();
	else if(chosen == redHoard)
		RH();
	else
		progSkills();
}

void redShoot()
{


	flyWheelOn = false;
	leftDriveEnc = 0;
	drivePower(127, 127);
	while(abs(leftDriveEnc) < 3000)
	{
	}
	drivePower(0,0);
	//flyWheelOn = true;
	//rpmGoal = rpmMid;
	//flyWheelOn = false;
	//followLineForClicks(110, 2000);
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
	drivePower(70, 70);
	wait1Msec(350);
	drivePower(0,0);
	wait1Msec(4000);
	autoShooting = true;
	wait1Msec(3000);
	for(int i = 0; i < 4; i++) {
		motor[conveyor] = 127;
		wait1Msec(330);
		motor[conveyor] = 0;
		wait1Msec(1700*(i!=3) + 400*(i==3));
	}
	rpmGoal = rpmLow;
	motor[conveyor] = 80;
	encoderTurn(-110);
	driveDistance(500);
	/*
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
	motor[conveyor] = 127;*/
}

void blueBot()
{
    flyWheelOn = true;
    motor[roller] = 127;
    rpmGoal = rpmHigh;
    wait1Msec(3000);

    for(int i = 0; i < 4; i++) {
        while(intakeLimit == 1) {
            motor[conveyor] = 100;
        }
        motor[conveyor] = 0;
        wait1Msec(1000);
    }
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
	rpmGoal = rpmLow;
	// go straight, score 4
	driveDistance(3000);
	autoShooting = true;
	motor[conveyor] = 127;
	wait1Msec(3000);
	motor[conveyor] = 0;
	autoShooting = false;
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

void IRBS()
{
	rpmGoal = rpmLow;
	motor[roller] = 127;
	// go straight, score 4
	drivePower(127, 127);
	leftDriveEnc = 0;
	while(abs(leftDriveEnc) < 3320){}
	drivePower(-20, -20);
	wait1Msec(40);
	drivePower(0,0);
	wait1Msec(400);
	drivePower(-100, 100);
	rightDriveEnc = 0;
	wait1Msec(90);
	drivePower(0,0);
	wait1Msec(1000);
	autoShooting = true;
	motor[conveyor] = 127;
	wait1Msec(3000);
	motor[conveyor] = 0;
	autoShooting = false;
}

void ORBS()
{
	rpmGoal = rpmLow;
	motor[roller] = 127;
	// go straight, score 4
	drivePower(127, 127);
	leftDriveEnc = 0;
	while(abs(leftDriveEnc) < 3320){}
	drivePower(-20, -20);
	wait1Msec(40);
	drivePower(0,0);
	wait1Msec(400);
	drivePower(100, -100);
	leftDriveEnc = 0;
	wait1Msec(200);
	drivePower(0,0);
	wait1Msec(1000);
	autoShooting = true;
	motor[conveyor] = 127;
	wait1Msec(3000);
	motor[conveyor] = 0;
	autoShooting = false;

	leftDriveEnc = 0;
	drivePower(100, -100);
	while(abs(leftDriveEnc) < 400){}
	drivePower(0,0);
	wait1Msec(300);

	leftDriveEnc = 0;
	drivePower(120, 120);
	while(abs(leftDriveEnc) < 700){}
	drivePower(-25, -25);
	wait1Msec(40);
	drivePower(0,0);


}

// pick this
void IBBS()
{
	rpmGoal = rpmLow;
	motor[roller] = 127;
	// go straight, score 4

	drivePower(127, 127);
	leftDriveEnc = 0;
	while(abs(leftDriveEnc) < 3350){}
	drivePower(-20, -20);
	wait1Msec(40);
	drivePower(0,0);
	//drivePowerForClicks(127, 3320);

	wait1Msec(2500);
	autoShooting = true;
	wait1Msec(100);
	motor[conveyor] = 127;
	wait1Msec(3000);
	motor[conveyor] = 0;
	rpmGoal = rpmLow;
	autoShooting = false;
}

void OBBS()
{
	IRBS();
}

void BH()
{
}

void RH()
{
}
