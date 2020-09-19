/*
 * VALVE TURNING WITH STEP MOTOR WITHOUT LIBS
   MADE BY TURNEROQ + PHILLIP 17-19.09.20
*/
#define IN1 4
#define IN2 5
#define IN3 6
#define IN4 7
#define INTER_PIN 2//INTERRPUT PIN FOR TRACKING SENSOR

#define MARKED_LINES 3 //IZOLENTA
#define MAX_TURNS 0.5 //FORCHECKER MARKED LINES, full open or close for valve

#define EIGHT_PHASE_MODE  0
#define FOUR_PHASE_MODE   1 //more torque by experience

#define GEAR_RATIO 2 // 2.8 CHECKER STEPS
#define FULL_OPEN_CLOSE_RATIO 2 // 2.5 CHECKER STEPS

#define LED_BLINKING_DELAY 300 // ms

#if FOUR_PHASE_MODE
#define PHASE_MODE 4
#else
#define PHASE_MODE 8
#endif

#define MAX_STEPS_FULL_TURN  PHASE_MODE * 512

#if FOUR_PHASE_MODE
#define PHASE_ENABLE_DELAY 3 // coils have inertial charging delay because of physics ;)
#else
#define PHASE_ENABLE_DELAY 2
#endif


int8_t  direction_counter;
int8_t currentPhase;
uint8_t stop_flag;
volatile uint8_t lines_counter;
uint32_t step_counter;

#if FOUR_PHASE_MODE
uint8_t fourStepsPhases[4][4] =
{
  {1, 0, 0, 1},
  {1, 1, 0, 0},
  {0, 1, 1, 0},
  {0, 0, 1, 1}
};
#endif

#if EIGHT_PHASE_MODE
uint8_t eightStepsPhases[8][4] =
{
  {1, 0, 0, 0},
  {1, 1, 0, 0},
  {0, 1, 0, 0},
  {0, 1, 1, 0},
  {0, 0, 1, 0},
  {0, 0, 1, 1},
  {0, 0, 0, 1},
  {1, 0, 0, 1}
};
#endif



void setup()
{
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(INTER_PIN), state_read, CHANGE);

  //start from open, from 0 phase, not stopped
  direction_counter = 1; // CW=1, CCW=-1 DIRECTION
  currentPhase = 0;
  stop_flag = 0;
  lines_counter = 0;
  step_counter = 0;
}

void checker()
{
  //currentPhase [0;3] or [0;7]
  if (currentPhase >= PHASE_MODE)
  {
    currentPhase = 0;//right boundary
  }
  else if (currentPhase < 0)
  {
    currentPhase = PHASE_MODE - 1; //left boundary
  }
  ++step_counter;//steps are increased each delay(each phase change)
  //CHECKER WITH STEPS
  /*
    if(step_counter >=  MAX_STEPS_FULL_TURN * 7) //GEAR_RATIO * FULL_OPEN_CLOSE_RATIO) 7 = 2.5 * 2.8
    {
    stop_flag = 1;
    step_counter = 0;
    }
  */
}

void state_read()
{
  ++lines_counter;
  //CHECKER WITH MARKED LINES
  if (lines_counter / (MARKED_LINES * 2) > MAX_TURNS)
  {
    stop_flag = 1;
    lines_counter = 0;
  }
}

void loop()
{
  if (stop_flag)
  {
    //INDICATION reverse direction
    for (uint8_t i = 0; i < 4; i++)
    {
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
      delay(LED_BLINKING_DELAY);
    }
    stop_flag = 0;
    direction_counter *= (-1); //reverse direction
  }
  else
  {
    checker();
    //SET PINS POWER
#if FOUR_PHASE_MODE
    digitalWrite(IN1, ((fourStepsPhases[currentPhase][0]) ? 1 : 0));
    digitalWrite(IN2, ((fourStepsPhases[currentPhase][1]) ? 1 : 0));
    digitalWrite(IN3, ((fourStepsPhases[currentPhase][2]) ? 1 : 0));
    digitalWrite(IN4, ((fourStepsPhases[currentPhase][3]) ? 1 : 0));
#endif

#if EIGHT_PHASE_MODE
    digitalWrite(IN1, ((eightStepsPhases[currentPhase][0]) ? 1 : 0));
    digitalWrite(IN2, ((eightStepsPhases[currentPhase][1]) ? 1 : 0));
    digitalWrite(IN3, ((eightStepsPhases[currentPhase][2]) ? 1 : 0));
    digitalWrite(IN4, ((eightStepsPhases[currentPhase][3]) ? 1 : 0));
#endif
    currentPhase += direction_counter;//change row in matrix of phases
    delay(PHASE_ENABLE_DELAY);
  }
}
