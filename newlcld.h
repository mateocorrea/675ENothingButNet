task LCD();
task speaker();
void playTune(string music);
void playMario();
void playJingleBells();
void playSong(int song);
void playDeathMarch();

bool lastLazerBtn = false;

int battery = 0;
int name = 1;
int autonomous = 2;
int music = 3;
int games = 4;
int stats = 5;
int bias = 6;
int MAX_SCREEN = 6;
int screen = name;
int chosenAuto = autoTwo;
int holdSwitch = 1000;

int autoPage = 0;
int biasScreen = 0;

/* 0:	No buttons pressed 						1:	Left button is pressed 							2:	Center button is pressed
   3:	Left and Center buttons are pressed		4:	Right button is pressed 						5:	Left and Right buttons are pressed
   6:	Center and Right buttons are pressed	7:	Left, Center, and Right buttons are pressed */
task LCD()
{
	int holdTime = 0;
	int song = 0;
	int stat = 0;
	
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
		if(nLCDButtons != 2)
			lastLazerBtn = false;
		while(nLCDButtons != 0) {}

		clearLCDLine(0);
		clearLCDLine(1);

		if(screen == battery) {
			//displayLCDString(0, 0, "Primary: ");
			//sprintf(mainBattery, "%1.2f%c", nImmediateBatteryLevel/1000.0,'V');
			//displayNextLCDString(mainBattery);
			display("Gyro: ", SensorValue[gyroSensor]);
			display("PowerExp: ", flyBattery);
		} else if(screen == name) {
			display("The Real", "Slim Shady");
		} else if(screen == music) {
			if(song == 0)
				display("Song:", "Mario")
			else if(song == 1)
				display("Song:", "Imperial March");
			else
				display("Song:", "Jingle Bells");
		} else if(screen == bias) {
			displayLCDCenteredString(0, "Power Bias");
			if(biasScreen == 0)
				display("Low Bias: ", lowPowerBias);
			else if(biasScreen == 1) {
				display("Mid Bias: ", midPowerBias);
			else
				display("High Bias: ", highPowerBias);
		} else if(screen == stats) {
			displayLCDCenteredString(0, "Stats");
			if(stat == 0)
				display("Distance: ", distanceX);
			else if (stat == 2)
				display("AvgSpeed: ", 0);
			else if (stat == 3)
				display("TopSpeed: ", maxSpeed);
			else if (stat == 4)
				display("LowShots: ", lowShots);
			else if (stat == 5)
				display("MidShots: ", midShots);
			else if (stat == 6)
				display("HighShots: ", highShots);
			else if (stat == 7)
				display("Collisions", collisions);
			else if (stat == 8)
				display("BrakesDep", brakeCount);
		} else if(screen == games) {
			display("Games", "Coming Soon");
		} else {
			if(autoPage == redShootNum) {
				displayLCDCenteredString(0, "PickupInsideRed");
			} else if(autoPage == autoTwo) {
				displayLCDCenteredString(0, "Red Side");
			} else if(autoPage == autoThree) {
				displayLCDCenteredString(0, "Shoot 4"); // blue bottom
			} else if(autoPage == autoFour) {
				displayLCDCenteredString(0, "Blue Side");
			} else if(autoPage == autoFive) {
				displayLCDCenteredString(0, "Defense >:D");
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
	playSoundFile("imp.wav");
	while(bSoundActive){}
	playSoundFile("imp2.wav");
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

void display(char *x, int y)
{
	displayLCDString(1, 0, x);
	displayNextLCDNumber(y);
}

void display(char *x, char *y)
{
	displayLCDCenteredString(0, x);
	displayLCDCenteredString(1, y);
}

void centerHold()
{
	if(screen == music)
		playSong(song);
	else if(sreen == bias) {
		biasScreen++;
		if(biasScreen > 2)
			biasScreen = 0;
		clearLCDLine(1);
		displayLCDCenteredString(1, "Switching");
	} else if(screen == autonomous)
	{
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
		if(biasScreen == lowBiasScreen)
			lowPowerBias = (lowPowerBias > 8 ) ? -10 : (lowPowerBias+2);
		else if(biasScreen == midBiasScreen)
			midPowerBias = (midPowerBias > 8 ) ? -10 : (midPowerBias+2);
		else
			highPowerBias = (highPowerBias > 8) ? -10 : (highPowerBias+2);
	} else if (screen == name) {
		if(lastLazerBtn == false) {
			SensorValue[laser] = !SensorValue[laser];
			lastLazerBtn = true;
		}
	} else if(screen == stats) {
		stat++;
		if(stat > 8)
			stat = 0;
	} else if(screen == autonomous) {
		chosenAuto = autoPage;
	}
}