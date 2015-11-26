#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, dgtl1,  brakePistons,   sensorDigitalOut)
#pragma config(Sensor, dgtl2,  lockPistons,    sensorDigitalOut)
#pragma config(Sensor, dgtl3,  deployerPiston, sensorDigitalOut)
#pragma config(Sensor, dgtl4,  launcherLimitSwitch, sensorTouch)
#pragma config(Sensor, dgtl5,  ,               sensorDigitalOut)
#pragma config(Sensor, dgtl6,  ,               sensorDigitalOut)
#pragma config(Sensor, dgtl7,  yellowLED,      sensorDigitalOut)
#pragma config(Sensor, dgtl8,  greenLED,       sensorDigitalOut)
#pragma config(Sensor, dgtl9,  redLED,         sensorDigitalOut)
#pragma config(Sensor, dgtl10, intakeTouchSensor, sensorTouch)
#pragma config(Sensor, I2C_1,  bottomleftencoder, sensorQuadEncoderOnI2CPort,    , AutoAssign)
#pragma config(Sensor, I2C_2,  bottomrightencoder, sensorQuadEncoderOnI2CPort,    , AutoAssign)
#pragma config(Sensor, I2C_3,  toprightencoder, sensorNone)
#pragma config(Sensor, I2C_4,  topleftencoder, sensorNone)
#pragma config(Motor,  port1,           leftback,      tmotorVex393HighSpeed_HBridge, openLoop)
#pragma config(Motor,  port2,           toprightlauncher, tmotorVex393HighSpeed_MC29, openLoop, reversed, encoderPort, I2C_1)
#pragma config(Motor,  port3,           bottomrightlauncher, tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port4,           topleftlauncher, tmotorVex393HighSpeed_MC29, openLoop, encoderPort, I2C_2)
#pragma config(Motor,  port5,           bottomleftlauncher, tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port6,           roller,        tmotorVex393TurboSpeed_MC29, openLoop)
#pragma config(Motor,  port7,           chain,         tmotorVex393TurboSpeed_MC29, openLoop)
#pragma config(Motor,  port8,           leftfront,     tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port9,           rightfront,    tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port10,          rightback,     tmotorVex393HighSpeed_HBridge, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#pragma platform(VEX)
//#pragma systemFile //disables annoying warnings
#pragma competitionControl(Competition)
#pragma autonomousDuration(15)

void init();
#pragma userControlDuration(120)
#include "cool.c"
#include "FlyWheelAndIntake.h"
#include "SpeakerAndLCD.h"
#include "DriveAndPneumatics.h"

void runAuto(int chosen);
void redShoot();
void redSide();
void blueBot();
void blueSide();
void defense();
void progSkills();
int richie = 0;

void pre_auton()
{


	bStopTasksBetweenModes = false;
	SensorType[in1] = sensorNone;
  	wait1Msec(1000);
  	SensorType[in1] = sensorGyro;
  	wait1Msec(1000);
	startTask(LCD);
}

task autonomous()
{
	init();
	startTask(flyWheelPower);
	runAuto(chosenAuto);
}

task usercontrol()
{
	if(getTaskState(LCD) == taskStateStopped)
		startTask(LCD);
	if(getTaskState(flyWheelPower) == taskStateStopped)
		startTask(flyWheelPower);
	if(getTaskState(drive) == taskStateStopped)
		startTask(drive);
	if(getTaskState(flyWheelControl) == taskStateStopped)
		startTask(flyWheelControl);
	if(getTaskState(pneumatics) == taskStateStopped)
		startTask(pneumatics);
	if(getTaskState(speaker) == taskStateStopped)
		startTask(speaker);
	if(getTaskState(intake) == taskStateStopped)
		startTask(intake);
}


void init()
{
}

void runAuto(int chosen) {
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

	flyWheelOn = true;
	wait1Msec(2000);
	motor[roller] = 100;
	for(int x = 0; x < 200; x++)
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
	redShoot();
}

void blueBot()
{
	redShoot();
}

void blueSide()
{
	redShoot();
}

void defense()
{
	redShoot();
}

void progSkills()
{
	while(true){
		redShoot();
	}
}
