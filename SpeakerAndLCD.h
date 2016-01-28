task LCD();
task speaker();
void PlayTune(string music);
void PlayMario();
void PlayJingleBells();
void playSong(int song);

void runAuto(int chosen);
void redShoot();
void redSide();
void blueBot();
void blueSide();
void defense();
void progSkills();

const int NaturalNotes[7]    = { 14080, 15804, 8372, 9397, 10548, 11175, 12544 }; /* A9, B9, C9, D9, E9, F9, G9 */
const int AccidentalNotes[7] = { 14917,     0, 8870, 9956,     0, 11840, 13290 }; /* A#9, x, C#9, D#9, x, F#9, G#9  */

bool lastLazerBtn = false;
int redShootNum = 0;
int autoTwo = 1;
int autoThree = 2;
int autoFour = 3;
int autoFive = 4;
int autoSix = 5;
int battery = 6;
int name = 7;
int music = 8;
int screen = name;
int chosenAuto = redShootNum;
int holdTime = 1500;


int warmupTime = 1500;
int initialShootTime = 2900;
int autoShootSpeed = 72;
int sideShot = 1460;


task LCD()
{
	int holdTime = 0;
	bool switched = false;
	bool lastPlay = false;
	int song = 0;
	/*
	0:	No buttons pressed 						1:	Left button is pressed 							2:	Center button is pressed
	3:	Left and Center buttons are pressed		4:	Right button is pressed 						5:	Left and Right buttons are pressed
	6:	Center and Right buttons are pressed	7:	Left, Center, and Right buttons are pressed
	*/
	bLCDBacklight = true;
	string mainBattery, backupBattery;
	while(true)
	{


		int flyBattery = SensorValue[in4];

		if(flyBattery < 900) {
			playSound(soundException);
			while(bSoundActive){}
		}

		if(nLCDButtons == 0) {
			holdTime = 0;
		}
		if(nLCDButtons == 4) {
			screen++;
			if(screen == 9) {
				screen = 0;
			}
		} else if(nLCDButtons == 1) {
			screen--;
			if(screen < 0) {
				screen = 8;
			}
		} else if(nLCDButtons == 2) {
			if(screen == music) {
				int baseTime = nSysTime;
				int time = 0;
				while((nLCDButtons == 2) && (time - baseTime < holdTime)) {
					time = nSysTime;
				}
				if((time - baseTime) >= holdTime) {
					playSong(song);
				} else {
					if(screen == music)
						song++;
					if(song > 1)
						song = 0;
				}

			} else if (screen == name) {
				if(lastLazerBtn == false) {
					SensorValue[laser] = !SensorValue[laser];
					lastLazerBtn = true;
				}
			} else if(screen == battery) {

			} else {
				chosenAuto = screen;
				if(chosenAuto > 5) {
					chosenAuto = 0;
				}
				if(userControl) {
					stopTask(intake);
					wait1Msec(2000);
					runAuto(chosenAuto);
					startTask(drive);
					startTask(intake);
				}
			}
		}
		if(nLCDButtons != 2)
			lastLazerBtn = false;
		while(nLCDButtons != 0) {}

		clearLCDLine(0);
		clearLCDLine(1);

		if(screen == redShootNum) {
			displayLCDCenteredString(0, "Red Shoot");
		} else if(screen == autoTwo) {
			displayLCDCenteredString(0, "Red Side");
		} else if(screen == autoThree) {
			displayLCDCenteredString(0, "Blue Bottom");
		} else if(screen == autoFour) {
			displayLCDCenteredString(0, "Blue Side");
		} else if(screen == autoFive) {
			displayLCDCenteredString(0, "Defense >:D");
		} else if(screen == autoSix) {
			displayLCDCenteredString(0, "Prog Skills");
		} else if(screen == battery) {
			//displayLCDString(0, 0, "Primary: ");
			//sprintf(mainBattery, "%1.2f%c", nImmediateBatteryLevel/1000.0,'V');
			//displayNextLCDString(mainBattery);
			displayLCDString(0, 0, "Gyro: ");
			displayNextLCDNumber(SensorValue[gyroSensor]);

			displayLCDString(1, 0, "PowerExp: ");
			displayNextLCDNumber(flyBattery);
		} else if(screen == name) {
			displayLCDCenteredString(0, "The Real");
			displayLCDCenteredString(1, "Slim Shady");
		} else if(screen == music) {
			displayLCDCenteredString(0, "Song");
			if(song == 0)
				displayLCDCenteredString(1, "Mario");
			else
				displayLCDCenteredString(1, "Jingle Bells");
		}

		if((screen < 6) && (screen == chosenAuto)) {
			displayLCDCenteredString(1, "Selected");
		}
		if(rpmMode) {
			if(SensorValue[greenLED] == 1 && (rpmGoal == rpmHigh))
				turnOn(green);
			else
				SensorValue[greenLED] = 1;
			if(SensorValue[yellowLED] == 1 && (rpmGoal == rpmMid))
				turnOn(yellow);
			else
				SensorValue[yellowLED] = 1;
			if(SensorValue[redLED] == 1 && (rpmGoal == rpmLow))
				turnOn(red);
			else
				SensorValue[redLED] = 1;
			wait1Msec(100);
			holdTime += 100;
		}
		//Short delay for the LCD refresh rate
		wait1Msec(100);
		holdTime += 100;
	}
}

task speaker()
{

	while(bSoundActive){}
}

void PlayTune(string music)
{
  #define WHOLE_NOTE (300)
  byte cursor = 0;
  int duration = WHOLE_NOTE >> 4;  // Default to sixteenth-note
  byte octave = 4; // Defaults to 4th octave, where middle C is
  char c;

  do {
    c = StringGetChar(music, cursor);
    cursor++;
    while(bSoundActive) { } ; // wait for sound idle
    if (c>='0' && c<='9') {
      octave = c-'0'; // set the octave
    } else if (c>='o' && c<='t') {
      duration = WHOLE_NOTE >> (c-'o'); // set note duration
    } else if (c>='*' && c<='/') {
      wait10Msec(WHOLE_NOTE >> (c-'*')); // rest
    } else if (c>='A' && c<='G') {
      playTone(NaturalNotes[c-'A']>>(9-octave), duration);
    } else if (c>='a' && c<='g') {
      playTone(AccidentalNotes[c-'a']>>(9-octave), duration);
    }
  } while (c);
}

void PlayMario()
{
	string Mario1 = "5AA.A.FA6C-.5C-";
	string Mario2 = "5F.C.4A.5DEdD.";
	string Mario3 = "5CA6CD.5a6C5AFGE.";
	string Mario4 = "6C5BagA.cDF.DFG.";
	string Mario5 = "6C5BagA.6E.EE-";
	string Mario6 = "r5g.G.sF";
	PlayTune(Mario1);
	PlayTune(Mario2);
	PlayTune(Mario3);
	PlayTune(Mario2);
	PlayTune(Mario3);
	PlayTune(Mario4);
	PlayTune(Mario5);
	PlayTune(Mario4);
	PlayTune(Mario6);
}

void PlayJingleBells()
{
	string JingleBells = "4rEEqErEEqErE5G4CD";//"";
	string JingleBells2 = "oEr4D5BA4GpDrDDrD5BA4GpE";
	PlayTune(JingleBells);
	PlayTune(JingleBells2);
}

void playSong(int song)
{
	if(song == 0)
		PlayMario();
	else
		PlayJingleBells();
}

void runAuto(int chosen) {
   startTask(flyWheelPower);
   stopTask(drive);
   rpmGoal = rpmHigh;
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
    encoderTurn(235);
    //gyroTurn(358); // turn towards pile
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
    //gyroTurn(-300); // turn to goal
    encoderTurn(-223);
    motor[chain] = autoShootSpeed; // shooot the second pile
    wait1Msec(4000);
}

void blueBot()
{
	redShoot();
}

void blueSide()
{

		/*
		GYRO VERSION
    motor[chain] = autoShootSpeed;
    wait1Msec(initialShootTime);
    motor[chain] = 0;
    //motor[roller] = 0;
    rpmGoal = sideShot;
    driveDistance(-1500);
    wait1Msec(500);
    gyroTurn(-358); // turn towards pile
    wait1Msec(500);
    driveDistance(1070); // go to pile
    //motor[roller] = 127;
    motor[chain] = 70;
    wait1Msec(800);
    motor[chain] = 0;
    driveDistance(-250);
    wait1Msec(500);
    gyroTurn(240); // turn to goal
    motor[chain] = autoShootSpeed; // shooot the second pile
    wait1Msec(4000);
    */
    motor[chain] = autoShootSpeed;
    wait1Msec(initialShootTime);
    motor[chain] = 0;
    //motor[roller] = 0;
    rpmGoal = sideShot;
    driveDistance(-1470);
    wait1Msec(500);
    encoderTurn(-235); // turn towards pile
    wait1Msec(500);
    driveDistance(1070); // go to pile
    //motor[roller] = 127;
    motor[chain] = 70;
    wait1Msec(800);
    motor[chain] = 0;
    driveDistance(-250);
    wait1Msec(500);
    encoderTurn(223); // turn to goal
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
    gyroTurn(380); // turn to goal
    motor[chain] = autoShootSpeed;
}
