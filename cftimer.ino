// What are we changing in the menu

#define BUZZERSHORTBEEP   500
#define BUZZERLONGBEEP    1000
#define BUZZERFREQUENCY   5000
#define ONESECOND         1000 


class Button {
  const byte pin;
  int state;
  unsigned long buttonDownMs;

  protected:
    virtual void shortClick() = 0;
    virtual void longClick() = 0;

  public:
    Button(byte attachTo) :
      pin(attachTo)
    {
    }

		void setup() {
      pinMode(pin, INPUT_PULLUP);
      state = HIGH;
    }

		void loop() {
			int prevState = state;
			state = digitalRead(pin);
			if (prevState == HIGH && state == LOW) {
				buttonDownMs = millis();
			}
			else if (prevState == LOW && state == HIGH) {
				if (millis() - buttonDownMs < 50) {
				// ignore this for debounce
				}
				else if (millis() - buttonDownMs < 250) {
					shortClick();
				}
				else  {
					longClick();
				}
			}
		}

};

/*Buttons:
 * Power / Start Button: Press to start, Hold to Wake from Deep sleep
 * Menu button: press to change mode
 * + button: plus time / menu
 * - button: minus time / menu
 */
// subclass of button, for each button
class PowerStartControlButton: public Button {
	public:
		PowerStartControlButton(byte attachTo) :
			Button(attachTo) 
		{
		}

	protected:
		void shortClick() {
		}

		void longClick() {
		}
};



//class menuChangeButton: public Button {

class MenuOption {
	char* displayName; //2 green characters
	int	startTimeInterval1Sec;
	int startTimeInterval2Sec;
	int nrOfRounds;
	bool countDirectionUp;

	public:
		MenuOption(char* name, int time1, int time2, int rounds, bool countUp) :
			displayName(name),
			startTimeInterval1Sec(time1),
			startTimeInterval2Sec(time2),
			nrOfRounds(rounds),
			countDirectionUp(countUp)
		{
		}

    char* getDisplayName(){
      return displayName;
    }
    int getStartTime1(){
      return startTimeInterval1Sec;
    }
    int getStartTime2(){
      return startTimeInterval2Sec;
    }
    int getNrOfRounds(){
      return nrOfRounds;
    }
    bool getCountDirectionUp(){
      return countDirectionUp;
    }
	
		void changeRounds(int change){
			nrOfRounds += change;
			//todo: Check for boundaries
		}
		void changeInterval1( int changeSec){
			startTimeInterval1Sec += changeSec;
			//todo: Check for boundaries
		}
		void changeInterval2( int changeSec){
			startTimeInterval2Sec += changeSec;
			//todo: Check for boundaries
		}
	
};


/*
 * Handles active clock countdown/up
 */
class TimerClock {
  MenuOption &activeOption;
  unsigned long startTimeMs;
  bool preCountDownOn;
  int activeSecond;
  int activeRound;
  static char displayText[6];
  
//Todo: Add reference to display

  private:

    void beepAtTheEnd() {
      if (activeSecond == 0) {
            //longBeep on 0
      }
      else {
        if (preCountDownOn){
        // Beep short short long on last 3 seconds 
          if (activeSecond < 4) {
            // shortBeep
          }
        }
      }
    }

    void preCountDown() {
      // Count down 10-0
      sprintf(displayText,"    %02d", 11-activeSecond);
      // Beep short short long on last 3 seconds
      if (activeSecond == 11) {
        preCountDownOn = false; //exit
      }
    }

    void countDown() {
      if (activeOption.getStartTime2() != 0){
        //Intervals
        int passedSec = activeSecond-11 - (((2*activeOption.getNrOfRounds() - activeRound)/2) * (activeOption.getStartTime1()+activeOption.getStartTime2()));
        if (activeRound%2) { //Interval 1
          sprintf(displayText,"%02d%02d%02d", activeRound, (activeOption.getStartTime1()-passedSec/60), (activeOption.getStartTime1()-passedSec)%60);
        }
        else { //Interval 2
          passedSec += activeOption.getStartTime1(); //add time of timer1, because this has additionally passed
          sprintf(displayText,"%02d%02d%02d", activeRound, (activeOption.getStartTime2()-passedSec/60), (activeOption.getStartTime2()-passedSec)%60);
        }
      }
      else { // No Intervals
        int passedSec = activeSecond-11 - ((activeOption.getNrOfRounds() - activeRound) * activeOption.getStartTime1());
        sprintf(displayText,"%02d%02d%02d", activeRound, (activeOption.getStartTime1()-passedSec/60), (activeOption.getStartTime1()-passedSec)%60);
      }
    }

    void countUp() {
      if ((activeSecond-11)/60 < 60) { //first hour, show "UP"
        sprintf(displayText,"UP%02d%02d", (activeSecond-11)/60, (activeSecond-11)%60);
      }
      else { //passed the hour
        sprintf(displayText,"%02d%02d%02d", (activeSecond-11)/3600, ((activeSecond-11)/60)%60, (activeSecond-11)%60);
      } 
    }

  public:
    TimerClock(MenuOption &optionToActivate) :
      activeOption(optionToActivate) 
    {
    }

    void startClock() {
      // public to skip precountdown
      
    }

    void setup() {
      startTimeMs = millis();
      preCountDownOn = true;
      activeSecond = 1;
      if (activeOption.getStartTime2() != 0){
        activeRound = 2*activeOption.getNrOfRounds(); //Show intervals
      }
      else {
        activeRound = activeOption.getNrOfRounds();
      }
    }

    void loop() {
      // Execute code
      
      if ( (millis() - startTimeMs) >= activeSecond*1000 ) {
        // one seconds has passed since the last second
        if (preCountDownOn){
          preCountDown();
        }
        else {
          // Timer running
          if (activeOption.getCountDirectionUp()) {
            countUp();
          }
          else { //Counting Down
            countDown();
          }  
        }
        //displayText on display
      }
      activeSecond++;
    }
};

/*
 * Menu implementation
 */
class TimerMenu {
	bool timerRunning; //false = menu displayed
  
	
	//objects for each menuOption
	
	//pass menuOption data to timerClock
	//pass display instance to timerclock

};

 /* Global data
  *  - Menu Mode (1:modechange, 2:minute10, 3:minute1, 4:seconds10, 5:seconds1, 0 = exitmenu)
  *  - Real Time Clock (todo)
  *  - Clock modus (up, dn, int, emom)
  *  - # of Rounds
  *  - Down Reset Start Time
  *  - EMOM Reset Start Time
  *  - Interval 1 Reset Start Time (60s)
  *  - Interval 2 Reset Start Time (60s)
  *  - Time when timer started (millis)
  *  - Direction
  */

void setup() {
  // put your setup code here, to run once:

}

/* Display time:
 *  Calculate from start time and reset time, rounds etc.
 *  example: 5rounds of 1:30 minute:
 *  5 * 90 seconds / 90 = rounds
 *  5 * 90 seconds modulo 60 = seconds
 *  etc
 */
void displayTime(){
  // (millis() - timerStarted) / 1000 = time elapsed seconds
  //downResetStartTime
}

void loop() {
  // put your main code here, to run repeatedly:

}



/* Display:
 *  6 digits:
 *  |UP 10:59| UP
 *  |dn 00:45| DOWN
 *  |10 00:59| EMOM
 *  |24 00:30| INT
 */




   /* Display Menu
    *  blink digits?
    *  + / - menu
    *  + / - Minutes
    *  + / - Seconds
    *  + / - menu etc.
    */

  /* Buzzer
   *  beep, beep, Beeeeep on start with 10seconds countdown
   *  Beeeep on finish
   *  Beep on interval / EMOM rounds (every time round = 0)
   */

  /* Start sequence, 10 seconds and beep when starting
   *  
   */
