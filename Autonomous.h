bool useGyroTurn = false;
int warmupTime = 1500;
int initialShootTime = 2900;
int autoShootSpeed = 72;
int sideShot = 1460;

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
    stopTask(drive);
    
    /* Warm-up the flywheel and reset the gyro meanwhile */
    rpmGoal = rpmHigh;
    flyWheelOn = true;
    SensorType[in1] = sensorNone;
    motor[roller] = 127;
    actuateBrake();
    wait1Msec(warmupTime);
    SensorType[in1] = sensorGyro;
    
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
    /* Set the chain speed and shoot for (initialShootTime) seconds */
    motor[chain] = autoShootSpeed;
    wait1Msec(initialShootTime);

    /* Stop shooting & set the flywheel rpm to the sideshot rpm */
    motor[chain] = 0;
    rpmGoal = sideShot;
    
    /* Turn on intake & turn towards the pile of balls */
    motor[chain] = 70;
    if(useGyroTurn)
        gyroTurn(-100);
    else
        encoderTurn(-85); // turn towards pile
    wait1Msec(500);
    
    /* Drive towards the piles and pick them up */
    driveDistance(500);
    wait1Msec(400);
    motor[chain] = 0;
    
    /* Turn towards the goal */
    if(useGyroTurn)
        gyroTurn(200);
    else
        encoderTurn(183); // turn to goal
    
    /* Shoot the balls at the goal */
    motor[chain] = autoShootSpeed;
}

void redSide()
{
    /* Set the chain speed and shoot for (initialShootTime) seconds */
    motor[chain] = autoShootSpeed;
    wait1Msec(initialShootTime);
    
    /* Stop shooting & set the flywheel rpm to the sideshot rpm */
    motor[chain] = 0;
    rpmGoal = sideShot;
    
    /* Drive away from tile and turn towards the pile of balls */
    driveDistance(-1500);
    wait1Msec(500);
    if(useGyroTurn)
        gyroTurn(358);
    else
        encoderTurn(235);
    wait1Msec(500);
    
    /* Drive towards the piles and pick them up */
    driveDistance(1070);
    motor[chain] = 70;
    wait1Msec(850);
    motor[chain] = 0;
    
    /* Drive away from the wall and turn towards the goal */
    driveDistance(-250);
    wait1Msec(500);
    if(useGyroTurn)
        gyroTurn(-300)
    else
        encoderTurn(-223);
    
    /* Shoot the balls at the goal */
    motor[chain] = autoShootSpeed;
}

void blueBot()
{
    /* Set the chain speed and shoot for (initialShootTime) seconds */
    motor[chain] = autoShootSpeed;
    wait1Msec(initialShootTime);
    
    /* Stop shooting & set the flywheel rpm to the sideshot rpm */
    motor[chain] = 0;
    rpmGoal = sideShot;
    
    /* Turn on intake & turn towards the pile of balls */
    motor[chain] = 70;
    if(useGyroTurn)
        gyroTurn(100);
    else
        encoderTurn(85); // turn towards pile
    wait1Msec(500);
    
    /* Drive towards the piles and pick them up */
    driveDistance(500);
    wait1Msec(400);
    motor[chain] = 0;
    
    /* Turn towards the goal */
    if(useGyroTurn)
        gyroTurn(-200);
    else
        encoderTurn(-183); // turn to goal
    
    /* Shoot the balls at the goal */
    motor[chain] = autoShootSpeed;
}

void blueSide()
{
    /* Set the chain speed and shoot for (initialShootTime) seconds */
    motor[chain] = autoShootSpeed;
    wait1Msec(initialShootTime);
    
    /* Stop shooting & set the flywheel rpm to the sideshot rpm */
    motor[chain] = 0;
    rpmGoal = sideShot;
    
    /* Drive away from tile and turn towards the pile of balls */
    driveDistance(-1470);
    wait1Msec(500);
    if(useGyroTurn)
        gyroTurn(-358);
    else
        encoderTurn(-235); // turn towards pile
    wait1Msec(500);
    
    /* Drive towards the piles and pick them up */
    driveDistance(1070);
    motor[chain] = 70;
    wait1Msec(850);
    motor[chain] = 0;
    
    /* Drive away from the wall and turn towards the goal */
    driveDistance(-250);
    wait1Msec(500);
    if(useGyroTurn)
        gyroTurn(240);
    else
        encoderTurn(223); // turn to goal
    
    /* Shoot the balls at the goal */
    motor[chain] = autoShootSpeed;
}

void defense()
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

void progSkills()
{
    useGyroTurn = true;
    
    /* Shoot the first batch of preloads for 17.5 seconds */
    clearTimer(T1);
    motor[chain] = autoShootSpeed;
    while(time1[T1] < 17500){
    }
    motor[chain] = 0;
    
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
    motor[chain] = autoShootSpeed;
}




