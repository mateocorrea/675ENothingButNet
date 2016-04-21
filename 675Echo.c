#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, in1,    accelerometerX, sensorAccelerometer)
#pragma config(Sensor, in2,    accelerometerY, sensorAccelerometer)
#pragma config(Sensor, in3,    gyroSensor,     sensorGyro)
#pragma config(Sensor, in4,    flyBattery,     sensorAnalog)
#pragma config(Sensor, in5,    rightLine,      sensorReflection)
#pragma config(Sensor, in6,    midLine,        sensorReflection)
#pragma config(Sensor, in7,    leftLine,       sensorReflection)
#pragma config(Sensor, dgtl1,  brakePistons,   sensorDigitalOut)
#pragma config(Sensor, dgtl2,  transPiston,    sensorDigitalOut)
#pragma config(Sensor, dgtl3,  deployerPistons, sensorDigitalOut)
#pragma config(Sensor, dgtl4,  lockPistons,    sensorDigitalOut)
#pragma config(Sensor, dgtl5,  yellowLED,      sensorDigitalOut)
#pragma config(Sensor, dgtl6,  greenLED,       sensorDigitalOut)
#pragma config(Sensor, dgtl7,  redLED,         sensorDigitalOut)
#pragma config(Sensor, dgtl8,  sonar,          sensorSONAR_cm)
#pragma config(Sensor, dgtl10, limitSwitch,    sensorTouch)
#pragma config(Sensor, dgtl12, hexLight,       sensorDigitalOut)
#pragma config(Sensor, I2C_1,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_2,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_3,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Motor,  port1,           rightDrive,    tmotorVex393HighSpeed_HBridge, openLoop, encoderPort, I2C_1)
#pragma config(Motor,  port2,           topLeftTrans,  tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port3,           topRightTrans, tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port4,           botLeftTrans,  tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port5,           botRightTrans, tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port6,           rightFlywheel, tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port7,           leftFlywheel,  tmotorVex393HighSpeed_MC29, openLoop, reversed, encoderPort, I2C_3)
#pragma config(Motor,  port8,           conveyor,      tmotorVex393TurboSpeed_MC29, openLoop)
#pragma config(Motor,  port9,           roller,        tmotorVex393TurboSpeed_MC29, openLoop)
#pragma config(Motor,  port10,          leftDrive,     tmotorVex393HighSpeed_HBridge, openLoop, encoderPort, I2C_2)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

bool userControl = false;
bool competitionMode = false;
#pragma platform(VEX)
#pragma competitionControl(Competition)
#include "cool.c"
#include "DriveAndPneumatics.h"
#include "FlyWheelAndIntake.h"
#include "Autonomous.h"
#include "SpeakerAndLCD.h"

void pre_auton()
{
	bStopTasksBetweenModes = false;
	SensorType[in3] = sensorNone;
	wait1Msec(1000);
	SensorType[in3] = sensorGyro;
	wait1Msec(1000);
	startTask(LCD);
	transmission = 1;
}

task autonomous()
{
	userControl = false;
	competitionMode = true;
	runAuto(chosenAuto);
}

task usercontrol()
{
	if(getTaskState(autonomous) == taskStateRunning)
		stopTask(autonomous);
	if(getTaskState(flyWheelPower) == taskStateRunning)
		stopTask(flyWheelPower);
	userControl = true;

	clearDebugStream();
	if(getTaskState(LCD) == taskStateStopped)
		startTask(LCD);
	if(getTaskState(flyWheelPower) == taskStateStopped)
		startTask(flyWheelPower);
	if(getTaskState(flyWheelControl) == taskStateStopped)
		startTask(flyWheelControl);
	if(getTaskState(drive) == taskStateStopped)
		startTask(drive);
	if(getTaskState(pneumatics) == taskStateStopped)
		startTask(pneumatics);
	if(getTaskState(speaker) == taskStateStopped)
		startTask(speaker);
	if(getTaskState(intake) == taskStateStopped)
		startTask(intake);
	if(getTaskState(led) == taskStateStopped)
		startTask(led);
}
