/*
   Kendall Callister
   2/11/2018
   Snake
   The goal of snake is to get the snake as long as possible without running off the edge of the map or colliding with yourself.
   The snake gets longer as the snake eats the cherries.
*/

//Libaries
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#ifndef PSTR
#define PSTR // Make Arduino Due happy
#endif

#define ZigZag

// Defines the pin in which the data is sent to the matrix.
#define MATRIX_OUTPUT_PIN 6

#define JOYSTICK_DOWN_PIN 13
#define JOYSTICK_UP_PIN 12
#define JOYSTICK_RIGHT_PIN 11
#define JOYSTICK_LEFT_PIN 10

#define SCORE_BOARD_RESET_PIN 0

#define GOBBLE_CLOCK_PIN 1
#define GOBBLE_UPDOWN_SELECT_PIN 2

#define SCORE_COUNT_PIN 3
#define SCORE_100_PIN 4

#define MAX_SNAKE_SIZE 256

typedef enum
{
  PRE_GAME,
  START_GAME,
  RUNNING_GAME,
  END_GAME,
} GameState_e;

typedef enum
{
  EASY,
  HARD,
} Difficulty_e;

typedef enum
{
  NO_DIRECTION,
  UP,
  DOWN,
  LEFT,
  RIGHT,
} Direction_e;

typedef enum
{
  NO_COLLISION,
  BODY_COLLISION,
  CHERRY_COLLISION,
  GOBBLE_COLLISION,
} Collision_e;

typedef union
{
  struct
  {
    uint8_t X : 4;
    uint8_t Y : 4;
  };

  struct
  {
    uint8_t A : 8;
  };

} Location_t;

typedef struct
{
  uint8_t Length;
  Location_t Head;
} Snake_t;

// Defines the Matrix.
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(16, 16, MATRIX_OUTPUT_PIN,
                                               NEO_MATRIX_TOP + NEO_MATRIX_RIGHT +
                                                   NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG,
                                               NEO_GRB + NEO_KHZ800);

// COOOOLOURS!!!
//Sets Snake Color
const uint16_t COLOR_RED = matrix.Color(255, 0, 0);
//Sets Cherry Color
const uint16_t COLOR_GREEN = matrix.Color(0, 255, 0);
//Sets the Eat Self Color for Hard Mode.
const uint16_t COLOR_BLUE = matrix.Color(0, 0, 255);

// Sets GREEN, RED, BLUE rotating colors for scrolling text.
const uint16_t colors[] = {COLOR_RED,
                           COLOR_GREEN,
                           COLOR_BLUE};

//Sets Snake Color
const uint16_t SNAKE_COLOR = matrix.Color(255, 102, 0);
//Sets Cherry Color
const uint16_t CHERRY_COLOR = matrix.Color(255, 0, 0);
//Sets the Eat Self Color for Hard Mode.
const uint16_t GOBBLE_COLOR = matrix.Color(91, 215, 213);

const uint16_t BACKGROUND_COLOR = matrix.Color(0, 0, 0);

// Static Variables
Snake_t Snake;
Location_t SnakeBody[MAX_SNAKE_SIZE];
uint8_t SnakeHeadIndex = 0;

Location_t Gobble, Cherry;

GameState_e GameState = PRE_GAME;
Difficulty_e Difficulty = EASY;

int score, gobblesStored, hold;

Direction_e SnakeDirection = NO_DIRECTION;
Direction_e NextSnakeDirection = NO_DIRECTION;
Direction_e LastDirection = NO_DIRECTION;

boolean PRINT = true;
int cursorIndex = matrix.width();

Direction_e GetDirection(void)
{
  if (!digitalRead(JOYSTICK_DOWN_PIN))
  {
    return DOWN;
  }
  else if (!digitalRead(JOYSTICK_UP_PIN))
  {
    return UP;
  }
  else if (!digitalRead(JOYSTICK_RIGHT_PIN))
  {
    return RIGHT;
  }
  else if (!digitalRead(JOYSTICK_LEFT_PIN))
  {
    return LEFT;
  }
  else
  {
    return NO_DIRECTION;
  }
}

int GetPixel(Location_t location)
{
  uint8_t address = location.A;

#ifdef ZigZag
  // Correct for the ZigZag in the LED Array
  if (!bitRead(address, 4))
  {
    address ^= 0x0F;
  }
#endif
  //matrix.setPixelColor(address, COLOR_BLUE);
  return matrix.getPixelColor(address);
}

int SetPixel(Location_t location, int color)
{
  matrix.drawPixel(location.X, location.Y, color);
}

inline boolean LocationsEqual(Location_t left, Location_t right)
{
  return left.A == right.A;
}

Collision_e GetCollision(Location_t location)
{

  if (LocationsEqual(location, Cherry))
  {
    return CHERRY_COLLISION;
  }
  else if (Difficulty == HARD && LocationsEqual(location, Gobble))
  {
    return GOBBLE_COLLISION;
  }
  else if (GetPixel(location) == BACKGROUND_COLOR)
  {
    return NO_COLLISION;
  }
  else
  {
    return BODY_COLLISION;
  }
}

Location_t GetRandomLocation(void)
{
  Location_t result;
  result.X = random(16);
  result.Y = random(16);
  return result;
}

Location_t DrawRandomLocation(int color)
{
  Location_t result = GetRandomLocation();
  SetPixel(result, color);
  return result;
}

void ResetGame(void)
{
  // Reset the scoreboard
  digitalWrite(SCORE_BOARD_RESET_PIN, LOW);

  matrix.fillScreen(BACKGROUND_COLOR);
  matrix.show();

  LastDirection = NO_DIRECTION;
  SnakeDirection = NextSnakeDirection = DOWN;

  Snake.Head.X = 5;
  Snake.Head.Y = 5;
  Snake.Length = 4;

  score = gobblesStored = 0;
  hold = 500;

  Cherry = DrawRandomLocation(CHERRY_COLOR);

  if (Difficulty == HARD)
  {
    Gobble = DrawRandomLocation(GOBBLE_COLOR);
  }

  digitalWrite(SCORE_BOARD_RESET_PIN, HIGH);
}

void setup()
{
  // put your setup code here, to run once:

  // Randomizes where the cherry starts
  randomSeed(analogRead(0));

  // Tell matrix to start, clear the screen, and set the Brightness of the martix.
  matrix.begin();
  matrix.fillScreen(0);
  matrix.setTextWrap(false);
  matrix.setBrightness(40);
  matrix.setTextColor(colors[0]);
  matrix.show();

  //PULLUPs the controller so that when direction made that arduino takes input.
  pinMode(JOYSTICK_DOWN_PIN, INPUT_PULLUP);
  pinMode(JOYSTICK_UP_PIN, INPUT_PULLUP);
  pinMode(JOYSTICK_RIGHT_PIN, INPUT_PULLUP);
  pinMode(JOYSTICK_LEFT_PIN, INPUT_PULLUP);

  //Setups the pins need to interface with external digital logic that will display information about the game.
  pinMode(SCORE_BOARD_RESET_PIN, OUTPUT);
  pinMode(GOBBLE_CLOCK_PIN, OUTPUT);
  pinMode(GOBBLE_UPDOWN_SELECT_PIN, OUTPUT);
  pinMode(SCORE_COUNT_PIN, OUTPUT);
  pinMode(SCORE_100_PIN, OUTPUT);

  // Allows for the arduino to talk to the computer.
  //Serial.begin(9600);
}

void loop()
{
  // put your main code here, to run repeatedly:
  delay(1);

  //Changes the Direction of the snake.
  Direction_e activeDirection = GetDirection();

  if (activeDirection != NO_DIRECTION)
  {
    LastDirection = activeDirection;

    // Only allow changes from Vertical to Horizontal or vise versa
    if (SnakeDirection == UP || SnakeDirection == DOWN)
    {
      if (activeDirection == LEFT || activeDirection == RIGHT)
      {
        NextSnakeDirection = activeDirection;
      }
    }
    else
    {
      if (activeDirection == UP || activeDirection == DOWN)
      {
        NextSnakeDirection = activeDirection;
      }
    }
  }

  //Allows controls to be input separately from movement in game.
  static int loopCount;
  if (++loopCount >= hold)
  {
    loopCount = 0;

    switch (GameState)
    {
    default:
    case PRE_GAME:
    {
      hold = 100;

      matrix.show();
      matrix.fillScreen(0);
      matrix.setCursor(cursorIndex, 0);

      // Displays how to select the hard difficulty.
      matrix.setTextColor(matrix.Color(255, 93, 21));
      matrix.print("Hard");

      int arrowCenter = cursorIndex + 27;
      // Draw up arrow
      matrix.drawLine(arrowCenter, 0, arrowCenter, 6, COLOR_RED);
      matrix.drawLine(arrowCenter, 0, arrowCenter - 2, 2, COLOR_RED);
      matrix.drawLine(arrowCenter, 0, arrowCenter + 2, 2, COLOR_RED);
      matrix.setCursor(cursorIndex, 8);

      // Displays how to select the easy difficulty.
      matrix.print("Easy");
      matrix.drawLine(arrowCenter, 15, arrowCenter, 8, COLOR_RED);
      matrix.drawLine(arrowCenter, 15, arrowCenter - 2, 13, COLOR_RED);
      matrix.drawLine(arrowCenter, 15, arrowCenter + 2, 13, COLOR_RED);

      if (--cursorIndex < -29)
      {
        cursorIndex = matrix.width();
      }

      // Grabs the player input if they select hard.
      /*
        * Hard Mode adds shedding to the game. If the Player hits the she--dded skin it will also end the game.
        * An new item is added to the game that allows the player to eat themselves and the shedded skin, but
        * eating the item will increase the speed of the snake without increasing the score.
        */

      switch (LastDirection)
      {
      case UP:
        Difficulty = HARD;
        GameState = START_GAME;
        break;

      case DOWN:
        Difficulty = EASY;
        GameState = START_GAME;
        break;
      }

      LastDirection = NO_DIRECTION;
    }
    break;

    case START_GAME:
      ResetGame();
      GameState = RUNNING_GAME;
      break;

    case RUNNING_GAME:
    {
      digitalWrite(GOBBLE_UPDOWN_SELECT_PIN, LOW);
      digitalWrite(GOBBLE_CLOCK_PIN, LOW);
      digitalWrite(SCORE_COUNT_PIN, LOW);
      delay(1);

      SnakeDirection = NextSnakeDirection;

      // Makes sure the snake is not outside of the boundaries.
      Location_t collisionLocation = Snake.Head;

      switch (SnakeDirection)
      {
      case UP:
        --collisionLocation.Y;
        if (Snake.Head.Y == 0)
        {
          GameState = END_GAME;
        }
        break;
      case DOWN:
        ++collisionLocation.Y;
        if (Snake.Head.Y == 15)
        {
          GameState = END_GAME;
        }
        break;
      case LEFT:
        --collisionLocation.X;
        if (Snake.Head.X == 0)
        {
          GameState = END_GAME;
        }
        break;
      case RIGHT:
        ++collisionLocation.X;
        if (Snake.Head.X == 15)
        {
          GameState = END_GAME;
        }
        break;
      }

      if (GameState == END_GAME)
      {
        break;
      }

      // Adds the color of the snake to the board and detects collision with the snake.

      switch (GetCollision(collisionLocation))
      {
      case BODY_COLLISION:
        if (gobblesStored == 0)
        {
          GameState = END_GAME;
        }
        else
        {
          digitalWrite(GOBBLE_CLOCK_PIN, HIGH);
          --gobblesStored;
          ++Snake.Length;
        }
        break;

      case CHERRY_COLLISION:

        Cherry = DrawRandomLocation(CHERRY_COLOR);

        ++Snake.Length;
        ++score;

        digitalWrite(SCORE_COUNT_PIN, HIGH);
        digitalWrite(SCORE_100_PIN, score >= 100);

        if (hold > 100)
        {
          hold -= 10;
        }
        break;

      case GOBBLE_COLLISION:

        Gobble = DrawRandomLocation(GOBBLE_COLOR);

        ++gobblesStored;

        digitalWrite(GOBBLE_UPDOWN_SELECT_PIN, HIGH);
        delay(1);
        digitalWrite(GOBBLE_CLOCK_PIN, HIGH);

        break;

      case NO_COLLISION:
        // Nothing to do sir
        break;
      }

      SetPixel(collisionLocation, SNAKE_COLOR);

      // Erases the snakes tail.
      if (++SnakeHeadIndex >= MAX_SNAKE_SIZE)
      {
        SnakeHeadIndex = 0;
      }

      SnakeBody[SnakeHeadIndex] = Snake.Head = collisionLocation;

      int16_t tailPointer = SnakeHeadIndex - Snake.Length;

      if (tailPointer < 0 && Difficulty == EASY)
      {
        tailPointer += MAX_SNAKE_SIZE;
      }

      if (tailPointer >= 0)
      {
        Location_t clearLocation = SnakeBody[tailPointer];

        int color = BACKGROUND_COLOR;

        if (LocationsEqual(clearLocation, Cherry))
        {
          color = CHERRY_COLOR;
        }
        else if (Difficulty == HARD && LocationsEqual(clearLocation, Gobble))
        {
          color = GOBBLE_COLOR;
        }

        SetPixel(clearLocation, color);
      }

      matrix.show();
    }
    break;

    case END_GAME:

      static byte loopTimes = 0;
      hold = 100;

      // delay the matrix then repeat.
      if ((loopTimes >= 1 && (LastDirection == RIGHT || LastDirection == LEFT)) || loopTimes >= 3)
      {
        loopTimes = 0;
        GameState = PRE_GAME;
      }
      else
      {
        LastDirection = NO_DIRECTION;

        matrix.fillScreen(0);
        matrix.setCursor(cursorIndex, 0);
        matrix.print(F("Gameover"));

        if (--cursorIndex < -46)
        {
          cursorIndex = matrix.width();

          matrix.setTextColor(colors[loopTimes]);
          ++loopTimes;
        }
        matrix.setCursor(0, 8);
        matrix.print(score);
        matrix.show();
      }
      break;
    }
  }
}
