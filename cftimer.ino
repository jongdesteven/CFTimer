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
    // Start Timer
		}

		void longClick() {
      // Wake from sleep
		}
};

class MenuControlButton: public Button {
  public:
    MenuControlButton(byte attachTo) :
      Button(attachTo) 
    {
    }

  protected:
    void shortClick() {
      //Next changeInterval
    }

    void longClick() {
      // Change menuOption
    }
};

class MinusButton: public Button {
  public:
    MinusButton(byte attachTo) :
      Button(attachTo) 
    {
    }

  protected:
    void shortClick() {
    }

    void longClick() {
    }
};

class PlusButton: public Button {
  public:
    PlusButton(byte attachTo) :
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
      if ( startTimeInterval2Sec != 0 ){
        return nrOfRounds*2;
      }
      else {
        return nrOfRounds ? nrOfRounds : 1;
      }
    }
    
    bool getCountDirectionUp(){
      return countDirectionUp;
    }
    bool hasRounds(){
      return nrOfRounds ? true: false;
    }
    bool hasInterval(){
      return includesInterval;
    }
	
		void changeRounds(int change){
      // do not add rounds if it never had
		  if (nrOfRounds > 0 && nrOfRounds <= 99) {
        nrOfRounds += change;
		  }
		}
   
		void changeTimeInterval(int interval, int changeSec){
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
  enum State {
    STOPPED = 0,
    PRECOUNTDOWN = 1,
    RUNNING = 2
  }state;
  int activeSecond;
  static char displayText[6];
  
//Todo: Add reference to display

    
  private:
    void beepAtTheEnd() {
      if (secondsLeftThisInterval() == 0) {
            //longBeep on 0
      }
      else {
        if (preCountDownOn && secondsLeftThisInterval() < 4){
          // Beep short short long on last 3 seconds 
        }
      }
    }

    void preCountDown() {
      // Count down 10-0
      sprintf(displayText,"    %02d", 11-activeSecond);
      // Beep short short long on last 3 seconds
      beepAtTheEnd();
    }

    void countDown() {
      sprintf(displayText,"%02d%02d%02d", roundsLeft(), secondsLeftThisInterval()/60, secondsLeftThisInterval()%60);
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

    int roundsLeft(){
      int roundTime = (activeOption.getStartTime1() + activeOption.getStartTime2());

      if (activeOption.hasRounds()){
        return ceil(((activeOption.getNrOfRounds()/2 * roundTime) - activeSecond) / roundTime);
      }
      else {
        return ceil(((activeOption.getNrOfRounds() * roundTime) - activeSecond) / roundTime);
      }
    }

    int secondsLeftThisInterval(){
      if (roundsLeft() == 0) return 0; //timer end show 0;
      if (roundsLeft()%2) { //Interval 1
        return activeOption.getStartTime1()-(activeSecond % activeOption.getStartTime1());
      }
      else { //Interval 2
        return activeOption.getStartTime2()-(activeSecond % activeOption.getStartTime1());
      }
    }

    void startClock() {
      // public to skip precountdown
      switch (state) {
          case PRECOUNTDOWN:
            state = RUNNING;
            activeSecond = 1;
            startTimeMs = millis();
            break;
          case RUNNING:
            //activeSecond = 0;
            state = STOPPED;
            break;
          case STOPPED:
            state = PRECOUNTDOWN;
            activeSecond = 1;
            startTimeMs = millis();
            break;
      }
      startTimeMs = millis();
    }

    bool timerRunning(){
      return state ? true : false;
    }

    void setup(MenuOption &optionToAttach) {
      activeOption = optionToAttach;
      state = STOPPED;
    }

    void loop() {
      switch (state) {
        case PRECOUNTDOWN:
          if ( (millis() - startTimeMs) >= activeSecond*1000 ) {
            preCountDown();
            if (activeSecond - 11 == 0) {
              startClock();
            }
            activeSecond++;
          }
          break;
        case RUNNING:
          if ( (millis() - startTimeMs) >= activeSecond*1000 ) {
            if (activeOption.getCountDirectionUp()) {
              countUp();
            }
            else {
              countDown();
            }
            if (roundsLeft() == 0){
              state = STOPPED;
            }
            activeSecond++;
          }
          break;
        case STOPPED:
          // Display Reset mode
          if( activeOption.getCountDirectionUp() ) {
            printf(displayText,"%2s%02d%02d", activeOption.getDisplayName(), (activeSecond)/60, (activeSecond)%60);
          }
          else {
            sprintf(displayText,"%02d%02d%02d", roundsLeft(), secondsLeftThisInterval()/60, secondsLeftThisInterval()%60);
          }
          break; 
      }
      //displayText on display
      beepAtTheEnd(); 
    }
};

/*
 * Menu implementation
 */
class TimerMenu {
  int activeMenu;
  static char displayText[6];
  enum ChangeInterval {
    MENUSTART = 0,
    INTERVAL1 = 1,
    INTERVAL2 = 2,
    NR_OF_ROUNDS = 3,
    TIMER_RUNNING = 4
  }changeInterval;
  enum ChangeDigit {
    MINUTE_TENS = 1,
    MINUTES = 2,
    SECOND_TENS = 3,
    SECONDS = 4,
    ROUNDS_TENS = 5,
    ROUNDS = 6
  }changeDigit;
  
  //const char* name, int time1, int time2, int rounds, bool countUp, bool isInterval
  MenuOption menuOptions[4] = {MenuOption("UP", 0, 0, 0, true, false),
                              MenuOption("UP", 45, 0, 2, true, false),  
                              MenuOption("dn", 10*60, 0, 0, false, false), 
                              MenuOption("nt", 60, 30, 5, false, true)};
  TimerClock activeTimer = TimerClock(menuOptions[0]);
  //pass display instance to timerclock

  private:

    void displayMenu(){
      //ToDo: Blinking changeDigit
      int intervalTime;
      switch (changeInterval){
      case MENUSTART:
        if ( menuOptions[activeMenu].hasRounds() ) {
          sprintf(displayText,"%2s  rd", menuOptions[activeMenu].getDisplayName());
        } else {
          sprintf(displayText,"%2s    ", menuOptions[activeMenu].getDisplayName());
        }
        break;
      case INTERVAL1:
        intervalTime = menuOptions[activeMenu].getStartTime1();
        sprintf(displayText,"1 %02d%02d", menuOptions[activeMenu].getStartTime1()/60, menuOptions[activeMenu].getStartTime1()%60);
        break;
      case INTERVAL2:
        sprintf(displayText,"2 %02d%02d", menuOptions[activeMenu].getStartTime2()/60, menuOptions[activeMenu].getStartTime2()%60);
        break;
      case NR_OF_ROUNDS:
        sprintf(displayText,"rd  %02d", menuOptions[activeMenu].getNrOfRounds());
        break;
      }
    }
  
  public:
    void setup() {
      activeMenu = 0;
      changeDigit = MINUTE_TENS; 
      changeInterval = INTERVAL1;
    }
  
    void loop() {
      if (activeTimer.roundsLeft() != 0){
        activeTimer.loop();//Display Timer
      }
      else {
        // Display Menu
        displayMenu();
      }
    }

    // To be called by Power/Start Button short press
    void startTheTimer(){

      switch (changeInterval) {
      case MENUSTART:
        activeTimer.setup(menuOptions[activeMenu]);
        activeTimer.startClock();
        changeInterval = TIMER_RUNNING;
        break;
      case INTERVAL1:
      case INTERVAL2:
      case NR_OF_ROUNDS:
        changeInterval = MENUSTART;
        break;
      case TIMER_RUNNING:
        if (activeTimer.timerRunning()) {
          activeTimer.startClock();
        }
        else {
          changeInterval = MENUSTART;
        }
        break;
      }
      
    }

    // To be called by Menu Button short press
    void changeTimerMode() {
      //cycle between MenuOptions
      activeMenu = (activeMenu+1)%sizeof(menuOptions);
      changeDigit = MINUTE_TENS;
      changeInterval = INTERVAL1;
    }

    // Switch between intervals or rounds
    void changeChangeMode() {
      
      // If at end of setting, switch to next Interval or Rounds, if applicable
      if (changeDigit == SECONDS || changeDigit == ROUNDS) {
        switch (changeInterval){
        case INTERVAL1:
          if (menuOptions[activeMenu].hasInterval()){
            changeInterval = INTERVAL2;
            changeDigit = MINUTE_TENS;
            break;
          } 
          else if ( menuOptions[activeMenu].hasRounds()) {
            changeInterval = NR_OF_ROUNDS;
            changeDigit = ROUNDS_TENS;
            break;
          }
          changeDigit = MINUTE_TENS; //stay at Interval1
          break;
        case INTERVAL2:
          changeInterval = NR_OF_ROUNDS;
          changeDigit = ROUNDS_TENS;
          break;
        case NR_OF_ROUNDS:
          changeInterval = INTERVAL1;
          changeDigit = MINUTE_TENS;
          break;
        }
      }
      else {
        // Move to next digit to change
        switch (changeDigit) {
        case MINUTE_TENS:
          changeDigit = MINUTES;
          break;
        case MINUTES:
          changeDigit = SECOND_TENS;
          break;
        case SECOND_TENS:
          changeDigit = SECONDS;
          break;
        case SECONDS:
          changeDigit = MINUTE_TENS;
          break;
        case ROUNDS_TENS:
          changeDigit = ROUNDS;
          break;
        case ROUNDS:
          changeDigit = MINUTE_TENS;
        } 
      }

      // Move to change menu, from menuStart
      switch (changeInterval) {
      case MENUSTART:
        changeInterval = INTERVAL1;
        changeDigit = MINUTE_TENS;
        break;
      case INTERVAL1:
      case INTERVAL2:
      case NR_OF_ROUNDS:
        break;
      }
      
    }

    // To be called by button +
    void incrementOption() {
      switch(changeDigit){
      case MINUTE_TENS: //minute tens
        menuOptions[activeMenu].changeTimeInterval(changeInterval, 10*10*60);
        break;
      case MINUTES:
        menuOptions[activeMenu].changeTimeInterval(changeInterval, 10*60);
        break;
      case SECOND_TENS:
        menuOptions[activeMenu].changeTimeInterval(changeInterval, 60);
        break;
      case SECONDS:
        menuOptions[activeMenu].changeTimeInterval(changeInterval, 1);
        break;
      case ROUNDS_TENS:
        menuOptions[activeMenu].changeRounds(10);
        break;
      case ROUNDS:
        menuOptions[activeMenu].changeRounds(1);
        break;             
      }
    }

    // To be called by button -
    void decrementOption() {
      switch (changeInterval) {
      case MENUSTART:
        changeInterval = INTERVAL1;
        changeDigit = MINUTE_TENS;
        break;
      case INTERVAL1:
      case INTERVAL2:
      case NR_OF_ROUNDS:
        break;
      }
      
      switch(changeDigit){
      case MINUTE_TENS: //minute tens
        menuOptions[activeMenu].changeTimeInterval(changeInterval, -10*10*60);
        break;
      case MINUTES:
        menuOptions[activeMenu].changeTimeInterval(changeInterval, -10*60);
        break;
      case SECOND_TENS:
        menuOptions[activeMenu].changeTimeInterval(changeInterval, -60);
        break;
      case SECONDS:
        menuOptions[activeMenu].changeTimeInterval(changeInterval, -1);
        break;
      case ROUNDS_TENS:
        menuOptions[activeMenu].changeRounds(-10);
        break;
      case ROUNDS:
        menuOptions[activeMenu].changeRounds(-1);
        break;       
      }
    }

};

void setup() {
  // put your setup code here, to run once:
  TimerMenu menu();
  

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
