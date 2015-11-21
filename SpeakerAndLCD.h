task LCD();
task speaker();
void PlayTune(string music);
void PlayMario();
void PlayJingleBells();

const int NaturalNotes[7]    = { 14080, 15804, 8372, 9397, 10548, 11175, 12544 }; /* A9, B9, C9, D9, E9, F9, G9 */
const int AccidentalNotes[7] = { 14917,     0, 8870, 9956,     0, 11840, 13290 }; /* A#9, x, C#9, D#9, x, F#9, G#9  */

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
			chosenAuto = screen;
			if(chosenAuto > 5) {
				chosenAuto = 0;
			}
			lastPlay = true;
		} else {
			if(lastPlay == false)
			{
				if(screen == music) {
					if(holdTime > 600 && switched == false) {
						song++;
						switched = true;
					}

				}
			}
			lastPlay = false;
		}
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
			displayLCDString(0, 0, "Primary: ");
			sprintf(mainBattery, "%1.2f%c", nImmediateBatteryLevel/1000.0,'V');
			displayNextLCDString(mainBattery);

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
			if(SensorValue[greenLED] == 1 && (flyWheelSpeed == highSpeed))
			{
				turnOn(green);
			} else {
				SensorValue[greenLED] = 1;
			}
			if(SensorValue[yellowLED] == 1 && (flyWheelSpeed == midSpeed))
			{
				turnOn(yellow);
			} else {
				SensorValue[yellowLED] = 1;
			}
			if(SensorValue[redLED] == 1 && (flyWheelSpeed == lowSpeed))
			{
				turnOn(red);
			} else {
				SensorValue[redLED] = 1;
			}
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
      PlayTone(NaturalNotes[c-'A']>>(9-octave), duration);
    } else if (c>='a' && c<='g') {
      PlayTone(AccidentalNotes[c-'a']>>(9-octave), duration);
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
