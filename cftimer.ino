// What are we changing in the menu
#define MENU_EXIT        0 //not in the menu but couting mode
#define MENU_MODE        1
#define MENU_MINUTE10    2
#define MENU_MINUTE1     3
#define MENU_SECONDS10   4
#define MENU_SECONDS1    5

// What counting mode are we in
#define MODE_UP     1
#define MODE_DOWN   2
#define MODE_EMOM   3
#define MODE_INT    4
#define MODE_CLOCK  5

#define BUZZERSHORTBEEP   500
#define BUZZERLONGBEEP    1000
#define BUZZERFREQUENCY   5000
#define ONESECOND         1000 


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

int timerMode;
int nrOfRounds;
int downResetStartTime;
int emomResetStartTime;
int interval1ResetStartTime;
int interval2ResetStartTime;
unsigned long timerStarted;
bool countDirectionUp;

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
  downResetStartTime
}

void loop() {
  // put your main code here, to run repeatedly:

}

/*Buttons:
 * Power / Start Button: Press to start, Hold to Wake from Deep sleep
 * Menu button: press to change mode
 * + button: plus time / menu
 * - button: minus time / menu
 */

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
