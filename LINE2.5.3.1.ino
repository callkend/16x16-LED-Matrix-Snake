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
// Defines the pin in which the data is sent to the matrix.
#define PIN 6

// Defines the Matrix.
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(16, 16, PIN,
                            NEO_MATRIX_TOP     + NEO_MATRIX_RIGHT +
                            NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG,
                            NEO_GRB            + NEO_KHZ800);

// Sets GREEN, RED, BLUE rotating colors for scrolling text.
const uint16_t colors[] = {
  matrix.Color(255, 0, 0), matrix.Color(0, 255, 0), matrix.Color(0, 0, 255)
};
//Varibles
unsigned char sx = 5;
unsigned char sy = 5;
char gx;
char gy;
char cx;
char cy;
int frames;
unsigned char tailLength = 4;
int i = 0;
unsigned char snakeX[256];
unsigned char snakeY[256];
unsigned char pointer = 4;
int x    = matrix.width();
int pass = 0;
int sColor = matrix.Color(255, 102, 0);
int cColor = matrix.Color(255, 0, 0);
int gColor = matrix.Color(91, 215, 213);
char sbit;
int pixelNumber;
int score = 0;
int times = 0;
int hold = 500;
int blueErase = 0;

boolean Change;
boolean Hard;
boolean Diffculty = false;
boolean UP, DOWN, RIGHT, LEFT, OVER = true, ifx = true, STARTED = true;
boolean LUP, LDOWN, LRIGHT, LLEFT;
boolean PRINT = true;

String Word = " ";

void setup() {
  // put your setup code here, to run once:
  //tells matirx to start, clear the screen, and set the Brightness of the martix.
  matrix.begin();
  matrix.fillScreen(0);
  matrix.setTextWrap(false);
  matrix.setBrightness(40);
  matrix.setTextColor(colors[0]);
  matrix.show();

  //PULLUPs the controller so that when direction made that arduino takes input.
  pinMode(13, INPUT_PULLUP) ;
  pinMode(12, INPUT_PULLUP) ;
  pinMode(11, INPUT_PULLUP) ;
  pinMode(10, INPUT_PULLUP) ;

  //Setups the pins need to interface with external digital logic that will display information about the game.
  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);

  // Randomizes where the cherry starts
  randomSeed(analogRead(0));
  // Allows for the arduino to talk to the computer.
  //Serial.begin(9600);
  // Spawns a cherry some where on the board.
  cx = random(16);
  cy = random(16);
  digitalWrite(0, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:

  //Allows controls to be input seperatly from movement in game.
  if (frames >= hold)
  {
    //lets you choosse between a hard or easy diffculty.
    if (!Diffculty)
    {
      matrix.show();
      matrix.fillScreen(0);
      matrix.setCursor(x, 0);
      
      // Displays how to select the hard diffculty.
      matrix.setTextColor(matrix.Color(255, 93, 21));
      matrix.print("Hard");
      matrix.drawLine(x + 27, 0, x + 27, 6, matrix.Color(255,0,0));
      matrix.drawLine(x + 27, 0, x + 25, 2, matrix.Color(255,0,0));
      matrix.drawLine(x + 27, 0, x + 29, 2, matrix.Color(255,0,0));
      matrix.setCursor(x, 8);
      
      // Displays how to select the easy diffculty.
      matrix.print("Easy");
      matrix.drawLine(x + 27, 15, x + 27, 8, matrix.Color(255,0,0));
      matrix.drawLine(x + 27, 15, x + 25, 13, matrix.Color(255,0,0));
      matrix.drawLine(x + 27, 15, x + 29, 13, matrix.Color(255,0,0));
      
      // Resets the game.
      sx = 5;
      sy = 5;
      hold = 100;
      DOWN = false;
      UP = false;
      RIGHT = false;
      LEFT = false;

      if (--x < -29) {
        x = matrix.width();
        if (++pass >= 3) pass = 0;
      }

      // Grabs the player input if they select hard.
      if (!digitalRead(12))
      {
        Hard = true;
        Diffculty = true;
        STARTED = true;
      }

      // Grabs the player input if they select easy
      if (!digitalRead(13))
      {
        Hard = false;
        Diffculty = true;
        STARTED = true;
      }

    }
    // Refreshes the game getting rid of old scores and set defaults.
    if (STARTED && Diffculty)
    {
      matrix.fillScreen(0);
      matrix.show();
      tailLength = 4;
      blueErase = 0;
      digitalWrite(0, HIGH);
      score = 0;
      sx = 5;
      sy = 5;

      cx = random(16);
      cy = random(16);
      DOWN = false;
      UP = false;
      RIGHT = false;
      LEFT = false;
      STARTED = false;
      OVER = false;
      hold = 500;
      matrix.drawPixel(cx,cy,cColor);

      if (Hard)
      {
        gx = random(16);
        gy = random(16);
        matrix.drawPixel(gx,gy,gColor);
      }
    }
  }
  
  Change = false;
  
  // Makes sure the snake is not outside of the boundries.
  if (sx < 0 || sx > 15 || sy < 0 || sy > 15)
  {
    OVER = true;
  }
  
  //Changes the Direction of the snake.
  if (!digitalRead(13) && !UP && !Change && !LUP)
  {
    DOWN = true;
    Change = true;
    UP = false, RIGHT = false, LEFT = false;
  }
  if (!digitalRead(12) && !DOWN && !Change && !LDOWN)
  {
    UP = true;
    Change = true;
    DOWN = false, RIGHT = false, LEFT = false;
  }
  if (!digitalRead(11) && !LEFT && !Change && !LLEFT)
  {
    RIGHT = true;
    Change = true;
    UP = false, DOWN = false, LEFT = false;
  }
  if (!digitalRead(10) && !RIGHT && !Change && !LRIGHT)
  {
    LEFT = true;
    Change = true;
    UP = false, DOWN = false, RIGHT = false;
  }

  if (frames >= hold)
  {
    digitalWrite(2, LOW);
    digitalWrite(1, LOW);
    digitalWrite(3, LOW);
    digitalWrite(0, LOW);
    delay(1);
    
    // Adds the color of the snake to the board and decects collision with the snake.
    if (DOWN && !OVER )
    {
      if (bitRead(sy * 16, 4) == 1) {
        if (matrix.getPixelColor(((sy + 1) * 16) + (16 - sx - 1)) == 0 || (sx == cx && sy + 1 == cy)  || (sx == gx && sy + 1 == gy)  ) {
          //matrix.setPixelColor(((sy + 1) *16) + (16 - sx - 1),RED);
          matrix.drawPixel(sx, ++sy, sColor);
        }
        else
        {
          if (blueErase == 0)
          {
            OVER = true;
          }
          else
          {
            matrix.drawPixel(sx, ++sy, sColor);
            digitalWrite(1, HIGH);
            blueErase--;
            tailLength++;
          }
        }
      }
      else {
        if (matrix.getPixelColor(((sy + 1) * 16 ) + sx) == 0  || (sx == cx && sy + 1 == cy)  || (sx == gx && sy + 1 == gy)) {
          //matrix.setPixelColor(((sy +1) *16 ) + sx,RED);
          matrix.drawPixel(sx, ++sy, sColor);
        }
        else
        {
          if (blueErase == 0)
          {
            OVER = true;
          }
          else
          {
            matrix.drawPixel(sx, ++sy, sColor);
            digitalWrite(1, HIGH);
            blueErase--;
            tailLength++;
          }
        }
      }
      LDOWN = true;
      LUP = false;
      LLEFT = false;
      LRIGHT = false;

    }
    if (UP && !OVER)
    {
      if (bitRead(sy * 16, 4) == 1) {
        if (matrix.getPixelColor(((sy - 1) * 16) + (16 - sx - 1)) == 0  || (sx == cx && sy - 1 == cy) || (sx == gx && sy - 1 == gy)) {
          // Used to debug the the collision function.
          //matrix.setPixelColor(((sy - 1) *16) + (16 - sx- 1),RED);
          matrix.drawPixel(sx, --sy, sColor);
        }
        else
        {
          if (blueErase == 0)
          {
            OVER = true;
          }
          else
          {
            matrix.drawPixel(sx, --sy, sColor);
            digitalWrite(1, HIGH);
            blueErase--;
            tailLength++;
          }
        }
      }
      else {
        if (matrix.getPixelColor(((sy - 1) * 16 ) + sx) == 0  || (sx == cx && sy - 1 == cy)  || (sx == gx && sy - 1 == gy)) {
          // Used to debug the the collision function.
          //matrix.setPixelColor(((sy -1) *16 ) + sx,RED);
          matrix.drawPixel(sx, --sy, sColor);
        }
        else
        {
          if (blueErase == 0)
          {
            OVER = true;
          }
          else
          {
            matrix.drawPixel(sx, --sy, sColor);
            digitalWrite(1, HIGH);
            blueErase--;
            tailLength++;
          }
        }
      }
      LDOWN = false;
      LUP = true;
      LLEFT = false;
      LRIGHT = false;
    }

    if (RIGHT && !OVER)
    {
      if (bitRead(sy * 16, 4) == 0) {
        if (matrix.getPixelColor((sy * 16) + (16 - (sx + 2))) == 0  || (sx + 1 == cx && sy == cy) || (sx + 1 == gx && sy == gy)) {
          //matrix.setPixelColor((sy *16) + (16 - (sx + 2)),RED);
          matrix.drawPixel(++sx, sy, sColor);
        }
        else
        {
          if (blueErase == 0)
          {
            OVER = true;
          }
          else
          {
            matrix.drawPixel(++sx, sy, sColor);
            digitalWrite(1, HIGH);
            blueErase--;
            tailLength++;
          }
        }
      }
      else {
        if (matrix.getPixelColor((sy * 16 ) + (sx + 1)) == 0  || (sx + 1 == cx && sy == cy) || (sx + 1 == gx && sy == gy)) {
          //matrix.setPixelColor((sy *16 ) + (sx + 1),RED);
          matrix.drawPixel(++sx, sy, sColor);
        }
        else
        {
          if (blueErase == 0)
          {
            OVER = true;
          }
          else
          {
            matrix.drawPixel(++sx, sy, sColor);
            digitalWrite(1, HIGH);
            blueErase--;
            tailLength++;
          }
        }
      }
      LDOWN = false;
      LUP = false;
      LLEFT = false;
      LRIGHT = true;
    }

    if (LEFT && !OVER)
    {
      if (bitRead(sy * 16, 4) == 0) {
        if (matrix.getPixelColor((sy * 16) + (16 - (sx))) == 0  || (sx - 1 == cx && sy == cy) || (sx - 1 == gx && sy == gy)) {
          //.setPixelColor((sy *16) + (16 - (sx)),RED);
          matrix.drawPixel(--sx, sy, sColor);
        }
        else
        {
          if (blueErase == 0)
          {
            OVER = true;
          }
          else
          {
            matrix.drawPixel(--sx, sy, sColor);
            digitalWrite(1, HIGH);
            blueErase--;
            tailLength++;
          }
        }
      }
      else {
        if (matrix.getPixelColor((sy * 16 ) + (sx - 1)) == 0  || (sx - 1 == cx && sy == cy) || (sx - 1 == gx && sy == gy)) {
          // matrix.setPixelColor((sy *16 ) + (sx - 1),RED);
          matrix.drawPixel(--sx, sy, sColor);
        }
        else
        {
          if (blueErase == 0)
          {
            OVER = true;
          }
          else
          {
            matrix.drawPixel(--sx, sy, sColor);
            digitalWrite(1, HIGH);
            tailLength++;
            blueErase--;
          }
        }
      }
      LDOWN = false;
      LUP = false;
      LLEFT = true;
      LRIGHT = false;
    }
    // tells the matrix to display all the changes.
    matrix.show();
    // if the snake head lands on cherry eat cherry, make snake longer, draw new cherry, and speeds up the snake up to a certain point.
    if (cx == sx && cy == sy && !OVER)
    {
      cx = random(16);
      cy = random(16);
      matrix.drawPixel(cx, cy, cColor);
      digitalWrite(3, HIGH);
      tailLength++;
      score = score + 1;
      if (score == 100)
      {
        digitalWrite(4, HIGH);
      }
      else
      {
        digitalWrite(4, LOW);
      }
      if (hold > 100) {
        hold = hold - 10;
      }
    }
    
    // Eats and spawns a Blue Eraser for hard mode.
    if (gx == sx && gy == sy && !OVER && Hard)
    {
      digitalWrite(2, HIGH);
      delay(1);
      digitalWrite(1, HIGH);
      gx = random(16);
      gy = random(16);
      matrix.drawPixel(gx, gy, gColor);
      blueErase++;
    }

    // erases the snakes tail.
    pointer++;
    if (pointer == 256)
    {
      pointer = 0;
    }
    snakeX[pointer] = sx;
    snakeY[pointer] = sy;
    if (pointer - tailLength < 0 && !Hard)
    {
      matrix.drawPixel(snakeX[pointer + 256 - tailLength], snakeY[pointer + 256 - tailLength] , 0);
      if(snakeX[pointer + 256 - tailLength]== cx && snakeY[pointer + 256 - tailLength == cy])
      {
        matrix.drawPixel(cx,cy,cColor);
      }
    }
    else
    {
      matrix.drawPixel(snakeX[pointer - tailLength], snakeY[pointer - tailLength], 0);
      if(snakeX[pointer - tailLength]== cx && snakeY[pointer - tailLength == cy])
      {
        matrix.drawPixel(cx,cy,cColor);
      }
    }

    matrix.show();
    // delay the matrix then repeat.
    if (times == 3)
    {
      times = 0;
      Diffculty = false;
    }
    // Displays that the game is over and the score of the player.
    if (OVER == true && Diffculty)
    {
      hold = 100;
      matrix.fillScreen(0);
      matrix.setCursor(x, 0);
      matrix.print(F("Gameover"));
      if (times >= 1 && !digitalRead(11) || !digitalRead(10))
      {
        times = 3;
      }
      if (--x < -46) {
        x = matrix.width();
        if (++pass >= 3) pass = 0;
        matrix.setTextColor(colors[pass]);
        times++;
      }
      matrix.setCursor(0, 8);
      matrix.print(score);
      matrix.show();
    }
    frames = 0;
  }
  frames = frames + 2;
  delay(1);
}
