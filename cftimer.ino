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
	const char* displayName; //2 green characters
	int	startTimeInterval1Sec;
	int startTimeInterval2Sec;
	int nrOfRounds;
	bool countDirectionUp;
  bool includesInterval;

	public:
		MenuOption(const char* name, int time1, int time2, int rounds, bool countUp, bool interval) :
			displayName(name),
			startTimeInterval1Sec(time1),
			startTimeInterval2Sec(time2),
			nrOfRounds(rounds),
			countDirectionUp(countUp),
      includesInterval(interval)
		{
		}

    const char* getDisplayName(){
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
    bool isInterval(){
      return includesInterval;
    }
	
		void changeRounds(int change){
			nrOfRounds += change;
			//todo: Check for boundaries
		}
		void changeInterval(int interval, int changeSec){
      switch (interval){
        case 1:
          startTimeInterval1Sec += changeSec;
          break;
        case 2:
          startTimeInterval2Sec += changeSec;
          break; 
      }
			startTimeInterval1Sec += changeSec;
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
    }

    void countDown() {
      if (activeOption.isInterval()){ //Intervals
        int passedSec = activeSecond - (((2*activeOption.getNrOfRounds() - activeRound)/2) * (activeOption.getStartTime1()+activeOption.getStartTime2()));
        if (activeRound%2) { //Interval 1
          sprintf(displayText,"%02d%02d%02d", activeRound, (activeOption.getStartTime1()-passedSec/60), (activeOption.getStartTime1()-passedSec)%60);
        }
        else { //Interval 2
          passedSec += activeOption.getStartTime1(); //add time of timer1, because this has additionally passed
          sprintf(displayText,"%02d%02d%02d", activeRound, (activeOption.getStartTime2()-passedSec/60), (activeOption.getStartTime2()-passedSec)%60);
        }
      }
      else { // No Intervals
        int passedSec = activeSecond - ((activeOption.getNrOfRounds() - activeRound) * activeOption.getStartTime1());
        sprintf(displayText,"%02d%02d%02d", activeRound, (activeOption.getStartTime1()-passedSec/60), (activeOption.getStartTime1()-passedSec)%60);
      }
    }

    void countUp() {
      //Todo: intervals support
      if ((activeSecond)/60 < 60) { //first hour, show "UP"
        sprintf(displayText,"%2s%02d%02d", activeOption.getDisplayName(), (activeSecond)/60, (activeSecond)%60);
      }
      else { //passed the hour
        sprintf(displayText,"%02d%02d%02d", (activeSecond)/3600, ((activeSecond)/60)%60, (activeSecond)%60);
      } 
    }
    
  public:
    TimerClock(MenuOption &optionToActivate) :
      activeOption(optionToActivate)
    {
    }

    void startClock() {
      // public to skip precountdown
      startTimeMs = millis();
      preCountDownOn = false;
      activeSecond = 0;
    }

    void setup(MenuOption &optionToAttach) {
      activeOption = optionToAttach;
      startTimeMs = millis();
      preCountDownOn = true;
      activeSecond = 1;
      if (activeOption.isInterval()){
        activeRound = 2*activeOption.getNrOfRounds(); //Show intervals
      }
      else {
        activeRound = activeOption.getNrOfRounds();
      }
    }

    void loop() {
      if ( (millis() - startTimeMs) >= activeSecond*1000 ) {
        if (preCountDownOn){
          preCountDown();
          if (activeSecond - 11 == 0) {
            startClock();
          }
        }
        else {
          if (activeOption.getCountDirectionUp()) {
            countUp();
          }
          else {
            countDown();
          }  
        }
        //displayText on display
        activeSecond++;
      }
    }
  
};

/*
 * Menu implementation
 */
class TimerMenu {
	bool timerRunning; //false = menu displayed
  int activeMenu;
  int changeDigit;
  int changeInterval;
  TimerClock activeTimer;
  //const char* name, int time1, int time2, int rounds, bool countUp, bool isInterval
  MenuOption menuOptions[3] = {MenuOption("UP", 0, 0, 0, true, false), 
                              MenuOption("dn", 10*60, 0, 0, false, false), 
                              MenuOption("nt", 60, 30, 5, false, true)};
  //pass display instance to timerclock
  
  public:
    void setup() {
      activeMenu = 0;
      changeDigit = 0; //|UP 01:23|
    }
  
    void loop() {
      
    }
  
    void startTheTimer(){
      activeTimer.setup(menuOptions[activeMenu]);
    }
  
    void changeTimerMode() {
      //cycle between MenuOptions
      activeMenu = (activeMenu+1)%sizeof(menuOptions);
      changeDigit = 0;
    }

    void changeChangeMode() {
      //cycle between minutes, seconds
      if(menuOptions[activeMenu].isInterval() && changeDigit == 3){
        changeInterval = (changeInterval+1)%2;
      }
      changeDigit = (changeDigit+1)%4;
    }
  
    void incrementOption() {
      switch(changeDigit){
        case 0: //minute tens
          menuOptions[activeMenu].changeInterval(changeInterval, 10*10*60);
          break;
        case 1:
          menuOptions[activeMenu].changeInterval(changeInterval, 10*60);
          break;
        case 2:
          menuOptions[activeMenu].changeInterval(changeInterval, 60);
          break;
        case 3:
          menuOptions[activeMenu].changeInterval(changeInterval, 1);
          break;       
      }
    }
  
    void decrementOption() {
      switch(changeDigit){
        case 0: //minute tens
          menuOptions[activeMenu].changeInterval(changeInterval, -10*10*60);
          break;
        case 1:
          menuOptions[activeMenu].changeInterval(changeInterval, -10*60);
          break;
        case 2:
          menuOptions[activeMenu].changeInterval(changeInterval, -60);
          break;
        case 3:
          menuOptions[activeMenu].changeInterval(changeInterval, -1);
          break;       
      }
    }
  	

};

void setup() {
  // put your setup code here, to run once:

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
