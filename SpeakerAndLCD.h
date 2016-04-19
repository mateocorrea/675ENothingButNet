task LCD();
task speaker();
task led();
void playTune(string music);
void playMario();
void playJingleBells();
void playSong(int song);
void playDeathMarch();
void displayNum(char *x, int y);
void displayStrings(char *x, char *y);
void centerClick();
void centerHold();

int name = 0;
int auto = 1;
int sensors = 2;
int music = 3;
int bias = 4;
int games = 5;
int battery = 6;
int MAX_SCREEN = 6;
int screen = name;
int chosenAuto = 1;
int holdSwitch = 1000;

int sensorPage = 0;
int autoPage = 0;
int biasScreen = 0;
int song = 0;

/* 0:	No buttons pressed 						1:	Left button is pressed 							2:	Center button is pressed
   3:	Left and Center buttons are pressed		4:	Right button is pressed 						5:	Left and Right buttons are pressed
   6:	Center and Right buttons are pressed	7:	Left, Center, and Right buttons are pressed */
task LCD()
{
	int holdTime = 0;

	bLCDBacklight = true;
	string mainBattery, backupBattery;
	while(true)
	{
		int flyBattery = SensorValue[in4];
		if(flyBattery < 900) {
			playSound(soundException);
			while(bSoundActive){}
		}

		if(nLCDButtons == 0) { //nothing pressed
			holdTime = 0;
		} else if(nLCDButtons == 4) { //right button pressed
			screen++;
			if(screen > MAX_SCREEN)
				screen = 0;
		} else if(nLCDButtons == 1) { // left button
			screen--;
			if(screen < 0)
				screen = MAX_SCREEN;
		} else if(nLCDButtons == 2) { //center pressed
			int baseTime = nSysTime;
			int time = 0;
			while((nLCDButtons == 2) && (time - baseTime < holdSwitch)) {
				time = nSysTime;
			}
			if((time - baseTime) >= holdSwitch) {
				centerHold();
			} else {
				centerClick();
			}
		}
		while(nLCDButtons != 0) {} // wait till you let go of button

		clearLCDLine(0);
		clearLCDLine(1);

        if(screen == battery) {
            displayLCDString(0, 0, "Primary: ");
            sprintf(mainBattery, "%1.2f%c", nImmediateBatteryLevel/1000.0,'V');
            displayNextLCDString(mainBattery);
            displayNum("PowerExp: ", flyBattery);
        } else if(screen == sensors) {
            displayLCDCenteredString(0, "Sensors");
						if(sensorPage == 0)
                displayNum("Gyro: ", SensorValue[gyroSensor]);
            else if (sensorPage == 1)
                displayNum("AccelX: ", SensorValue[accelerometerX]);
            else if (sensorPage == 2)
                displayNum("AccelY: ", SensorValue[accelerometerY]);
            else if (sensorPage == 3)
                displayNum("RightEnc: ", rightDriveEnc);
            else if (sensorPage == 4)
                displayNum("LeftEnc: ", leftDriveEnc);
            else if (sensorPage == 5)
                displayNum("FlyEnc: ", flywheelEncoder);
		} else if(screen == name) {
			displayStrings("The Real", "Slim Shady");
		} else if(screen == music) {
			if(song == 0)
				displayStrings("Song:", "Mario");
			else if(song == 1)
				displayStrings("Song:", "Imperial March");
			else
				displayStrings("Song:", "Jingle Bells");
		} else if(screen == bias) {
			displayLCDCenteredString(0, "Power Bias");
			if(biasScreen == 0)
				displayNum("Low Bias: ", lowPowerBias);
			else if(biasScreen == 1)
				displayNum("Mid Bias: ", midPowerBias);
			else
				displayNum("High Bias: ", highPowerBias);
		} else if(screen == games) {
			displayStrings("Games", "Coming Soon");
		} else {
			if(autoPage == redShootNum) {
				displayLCDCenteredString(0, "Defense Red");
			} else if(autoPage == autoTwo) {
				displayLCDCenteredString(0, "Red Side");
			} else if(autoPage == autoThree) {
				displayLCDCenteredString(0, "Shoot 4"); // blue bottom
			} else if(autoPage == autoFour) {
				displayLCDCenteredString(0, "Blue Side");
			} else if(autoPage == autoFive) {
				displayLCDCenteredString(0, "Defense Blue");
			} else if(autoPage == autoSix) {
				displayLCDCenteredString(0, "Prog Skills");
			}
			if(chosenAuto == autoPage)
				displayLCDCenteredString(1, "Selected");
		}
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


		//Short delay for the LCD refresh rate
		wait1Msec(100);
		holdTime += 100;
	}
}

task speaker()
{
	bool lastPlayBtn = false;
	while(true)
	{
		if(vexRT[Btn8U] && !lastPlayBtn) {
			lastPlayBtn = true;
			if(!bSoundActive)
				playDeathMarch();
		} else if(!vexRT[Btn8U]) {
			lastPlayBtn = false;
		}
		if(lifted && competitionMode && !bSoundActive) {
			flyWheelOn = false;
			rollerOn = false;
			playDeathMarch();
		}
	}
}

task led()
{
	while(true)
	{
		if(vexRT[Btn5U] || autoShooting) {
			SensorValue[hexLight] = !SensorValue[hexLight];
		} else {
			SensorValue[hexLight] = 0;
		}
		wait1Msec(45);
	}
}

void playTune(string music)
{
	const int NaturalNotes[7]    = { 14080, 15804, 8372, 9397, 10548, 11175, 12544 }; /* A9, B9, C9, D9, E9, F9, G9 */
	const int AccidentalNotes[7] = { 14917,     0, 8870, 9956,     0, 11840, 13290 }; /* A#9, x, C#9, D#9, x, F#9, G#9  */
	#define WHOLE_NOTE (300)
	byte cursor = 0;
	int duration = WHOLE_NOTE >> 4;  // Default to sixteenth-note
	byte octave = 4; // Defaults to 4th octave, where middle C is
	char c;
	do {
	    c = stringGetChar(music, cursor);
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

void playMario()
{
	string Mario1 = "5AA.A.FA6C-.5C-";
	string Mario2 = "5F.C.4A.5DEdD.";
	string Mario3 = "5CA6CD.5a6C5AFGE.";
	string Mario4 = "6C5BagA.cDF.DFG.";
	string Mario5 = "6C5BagA.6E.EE-";
	string Mario6 = "r5g.G.sF";
	playTune(Mario1);
	playTune(Mario2);
	playTune(Mario3);
	playTune(Mario2);
	playTune(Mario3);
	playTune(Mario4);
	playTune(Mario5);
	playTune(Mario4);
	playTune(Mario6);
}

void playJingleBells()
{
	string JingleBells = "4rEEqErEEqErE5G4CD";//"";
	string JingleBells2 = "oEr4D5BA4GpDrDDrD5BA4GpE";
	playTune(JingleBells);
	playTune(JingleBells2);
}

void playDeathMarch()
{
	//playSoundFile("imp.wav");
	while(bSoundActive){}
	//playSoundFile("imp2.wav");
}
void playSong(int song)
{
	if(song == 0)
		playMario();
	else if(song == 1)
		playDeathMarch();
	else
		playJingleBells();
}

void displayNum(char *x, int y)
{
	displayLCDString(1, 0, x);
	displayNextLCDNumber(y);
}

void displayStrings(char *x, char *y)
{
	displayLCDCenteredString(0, x);
	displayLCDCenteredString(1, y);
}

void centerHold()
{
	if(screen == music)
		playSong(song);
	else if(screen == bias) {
		biasScreen++;
		if(biasScreen > 2)
			biasScreen = 0;
		clearLCDLine(1);
		displayLCDCenteredString(1, "Switching");
	} else if(screen == auto) {
		autoPage++;
		if(autoPage > 5)
			autoPage = 0;
		clearLCDLine(1);
		displayLCDCenteredString(1, "Switching");
	}
}

void centerClick()
{
	if(screen == music) {
		song++;
		if(song > 2)
			song = 0;
	} else if(screen == bias) {
		if(biasScreen == 0)
			lowPowerBias = (lowPowerBias > 8 ) ? -10 : (lowPowerBias+2);
		else if(biasScreen == 1)
			midPowerBias = (midPowerBias > 8 ) ? -10 : (midPowerBias+2);
		else
			highPowerBias = (highPowerBias > 8) ? -10 : (highPowerBias+2);
	} else if(screen == auto) {
		chosenAuto = autoPage;
        clearLCDLine(1);
        displayLCDCenteredString(1, "Selected");
    } else if(screen == sensors) {
        sensorPage++;
        if(sensorPage > 5)
            sensorPage = 0;
    } else if(screen == 10000) {
		displayStatusAndTime();
		gyroTurnTo(2000, -1);
	}
}
