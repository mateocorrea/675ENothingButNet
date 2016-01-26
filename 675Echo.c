#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, in1,    gyroSensor,     sensorGyro)
#pragma config(Sensor, in4,    flyBattery,     sensorAnalog)
#pragma config(Sensor, dgtl1,  brakePistons,   sensorDigitalOut)
#pragma config(Sensor, dgtl2,  lockPistons,    sensorDigitalOut)
#pragma config(Sensor, dgtl3,  deployerPiston, sensorDigitalOut)
#pragma config(Sensor, dgtl4,  laser,          sensorDigitalOut)
#pragma config(Sensor, dgtl5,  ,               sensorDigitalOut)
#pragma config(Sensor, dgtl6,  ,               sensorDigitalOut)
#pragma config(Sensor, dgtl7,  yellowLED,      sensorDigitalOut)
#pragma config(Sensor, dgtl8,  greenLED,       sensorDigitalOut)
#pragma config(Sensor, dgtl9,  redLED,         sensorDigitalOut)
#pragma config(Sensor, dgtl10, intakeTouchSensor, sensorTouch)
#pragma config(Sensor, I2C_1,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_2,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_3,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_4,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Motor,  port1,           leftback,      tmotorVex393HighSpeed_HBridge, openLoop)
#pragma config(Motor,  port2,           topLeftLauncher, tmotorVex393HighSpeed_MC29, openLoop, reversed, encoderPort, I2C_4)
#pragma config(Motor,  port3,           bottomLeftLauncher, tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port4,           topRightLauncher, tmotorVex393HighSpeed_MC29, openLoop, encoderPort, I2C_3)
#pragma config(Motor,  port5,           bottomRightLauncher, tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port6,           roller,        tmotorVex393TurboSpeed_MC29, openLoop)
#pragma config(Motor,  port7,           chain,         tmotorVex393TurboSpeed_MC29, openLoop)
#pragma config(Motor,  port8,           leftfront,     tmotorVex393HighSpeed_MC29, openLoop, encoderPort, I2C_2)
#pragma config(Motor,  port9,           rightfront,    tmotorVex393HighSpeed_MC29, openLoop, reversed, encoderPort, I2C_1)
#pragma config(Motor,  port10,          rightback,     tmotorVex393HighSpeed_HBridge, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#pragma platform(VEX)
#pragma competitionControl(Competition)
#include "cool.c"
#include "FlyWheelAndIntake.h"
#include "jpearmanTBH.h"
#include "SpeakerAndLCD.h"
#include "DriveAndPneumatics.h"

bool usePID = true;

void runAuto(int chosen);
void redShoot();
void redSide();
void blueBot();
void blueSide();
void defense();
void progSkills();

int warmupTime = 1500;
int initialShootTime = 2900;
int autoShootSpeed = 72;
int sideShot = 1440;

void pre_auton()
{
    SensorValue[laser] = 1;
		bStopTasksBetweenModes = false;
		SensorType[in1] = sensorNone;
  	wait1Msec(1000);
  	SensorType[in1] = sensorGyro;
  	wait1Msec(1000);
		startTask(LCD);
}

task autonomous()
{
    SensorValue[laser] = 0;
	runAuto(chosenAuto);
}

task usercontrol()
{
	SensorValue[laser] = 0;
	rpmGoal = rpmHigh;
	clearDebugStream();
	if(getTaskState(LCD) == taskStateStopped)
		startTask(LCD);
	if(usePID) {
		if(getTaskState(flyWheelPower) == taskStateStopped)
			startTask(flyWheelPower);
		if(getTaskState(flyWheelControl) == taskStateStopped)
			startTask(flyWheelControl);
	} else {
		if(getTaskState(tbh) == taskStateStopped)
			startTask(tbh);
	}
	if(getTaskState(drive) == taskStateStopped)
		startTask(drive);
	if(getTaskState(pneumatics) == taskStateStopped)
		startTask(pneumatics);
	if(getTaskState(speaker) == taskStateStopped)
		startTask(speaker);
	if(getTaskState(intake) == taskStateStopped)
		startTask(intake);
}


void runAuto(int chosen) {
    startTask(flyWheelPower);
    flyWheelOn = true;
    SensorType[in1] = sensorNone;
    motor[roller] = 127;
    wait1Msec(warmupTime);
    SensorType[in1] = sensorGyro;
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
	int carry = 340;
	int separation = 2500;
	for(int x = 0; x < 10; x++)
	{
		motor[chain] = 127;
		wait1Msec(carry);
		motor[chain] = 0;
		wait1Msec(separation);
	}
	flyWheelOn = false;
}

void redSide()
{

  	SensorType[in1] = sensorGyro; // reset the gyro
    motor[chain] = autoShootSpeed; // set the speed of the chain to auto speed
    wait1Msec(initialShootTime); // shoot the balls/
    motor[chain] = 0; // stop the chain
    //motor[roller] = 0;
    rpmGoal = sideShot; // set up the rpm for the side pile shot
    driveDistance(-1500);
    gyroTurn(358); // turn towards pile
    wait1Msec(500);
    driveDistance(1070); // go to pile
    //motor[roller] = 127;
    motor[chain] = 70;
    wait1Msec(800);
    motor[chain] = 0;
    /*motor[chain] = 0;
    driveDistance(-250);
    wait1Msec(500);
    driveDistance(290);
    motor[chain] = 35;
    wait1Msec(500);*/
    driveDistance(-250);
    wait1Msec(500);
    gyroTurn(-300); // turn to goal
    motor[chain] = autoShootSpeed; // shooot the second pile
    wait1Msec(4000);
}

void blueBot()
{
	redShoot();
}

void blueSide()
{
    motor[chain] = autoShootSpeed;
    wait1Msec(initialShootTime);
    motor[chain] = 0;
    //motor[roller] = 0;
    rpmGoal = sideShot;
    driveDistance(-1500);
    gyroTurn(-358); // turn towards pile
    wait1Msec(500);
    driveDistance(1070); // go to pile
    //motor[roller] = 127;
    motor[chain] = 70;
    wait1Msec(800);
    motor[chain] = 0;
    /*motor[chain] = 0;
    driveDistance(-250);
    wait1Msec(500);
    driveDistance(290);
    motor[chain] = 35;
    wait1Msec(500);*/
    driveDistance(-250);
    wait1Msec(500);
    gyroTurn(300); // turn to goal
    motor[chain] = autoShootSpeed; // shooot the second pile
    wait1Msec(4000);
}

void defense()
{
	redShoot();
}

void progSkills()
{
    clearTimer(T1);
    motor[chain] = autoShootSpeed;
	while(time1[T1] < 17500){
	}
    motor[chain] = 0;
    driveDistance(-200); // move away from
    wait1Msec(200);
    gyroTurn(-1134);
    driveDistance(2300);
    drivePower(100, 100);
    wait1Msec(800);
    drivePower(0,0);
    wait1Msec(500);
    driveDistance(-350); // set up position to shoot
    wait1Msec(500);
    gyroTurn(385); // turn to goal
    motor[chain] = autoShootSpeed;
}
