#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const int healthLED1 = 2; 
const int healthLED2 = 3; 
const int healthLED3 = 4; 
const int buttonUp=A3;
const int buttonSelect=13.2;

const byte sevenSegmentPins[7] = {5, 6, 7, 8, 9, 10, 11};
const byte digitPins[2]={A1,12.2};
const int lightSensorPin = A2;
boolean end=false;

byte Values[10][7] = {
  {0,0,0,0,0,0,1}, //0
  {1,0,0,1,1,1,1}, //1
  {0,0,1,0,0,1,0}, //2
  {0,0,0,0,1,1,0}, //3
  {1,0,0,1,1,0,0}, //4
  {0,1,0,0,1,0,0}, //5
  {0,1,0,0,0,0,0}, //6
  {0,0,0,1,1,1,1}, //7
  {0,0,0,0,0,0,0}, //8
  {0,0,0,0,1,0,0}  //9  
};

int paddle_width=24;
int paddle_height=1;
float paddle_x = 48.0;
float paddle_y=60;


float ball_x=64.0, ball_y=32.0;
float ball_speed_x = 1.0;
float ball_speed_y = -1.0;
float ball_speed_x_l=1.0;
float ball_speed_y_l=-1.0;
int brick_width=31;
int brick_height=5;
int num_brick_x=128/(brick_width+1);
int num_brick_y=18/(brick_height+1);

int lightValue = 0;
int background=0;
int objects=1;

int health = 3;
int score=0;

int menu_selection = 0;

float itemX, itemY;
bool itemActive = false;

bool bricks[60][3];

static int stage=1;
bool start=false;



Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire);

void setup() {

pinMode(buttonUp, INPUT_PULLUP );
pinMode(buttonSelect, INPUT_PULLUP );

  pinMode(healthLED1, OUTPUT);
  pinMode(healthLED2, OUTPUT);
  pinMode(healthLED3, OUTPUT);
 
 for (int i = 0; i < 7; ++i) {
    pinMode(sevenSegmentPins[i], OUTPUT);
 }
 for (int i = 0; i < 3; ++i) {
    pinMode(digitPins[i], OUTPUT);
  }

display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
pinMode(A0, INPUT);
display.clearDisplay();
  
    
   
    display.display();

    delay(200);

}


void loop() {
display.clearDisplay();

if(!start){
menu();

}
if(start){

game();
lights();
display.display();

}

delay(10);
}
void menu(){

display.clearDisplay();
display.setTextSize(1);
display.setTextColor(1);

display.setCursor(32,16);
display.println("Start");
display.setCursor(32,32);
display.println("Exit");
display.setCursor(24,(16*menu_selection)+16);
display.println(">");

display.display();

if(digitalRead(buttonUp)==LOW){
menu_selection=(menu_selection+1)%2;
delay(100);
}
if(digitalRead(buttonSelect)==LOW){

switch (menu_selection){

case 0:
start=true;
newGame();
break;
case 1:
display.clearDisplay();
display.setCursor(8,16);
display.println("Thanks For Playing");
display.display();
delay(6000);
exit(0);
break;
}

}

}

void game(){
lightValue = analogRead(lightSensorPin); 
if(lightValue>50) {
 background = 1;
 objects=0;
}
else {
background = 0;
 objects=1;
}
if(end){
nextLevel();
display.clearDisplay();
display.display();
delay(5000);
end=false;
}
if(health==0){
display.clearDisplay();
display.setTextSize(1);
display.setTextColor(1);
display.setCursor(32,16);
display.println("Game Over");
display.setCursor(32,32);
display.println("Score :");
display.setCursor(80,32);
display.println(score);
display.display();
delay(5000);
start=false;
}

display.fillScreen(background);
for (int i = 0; i < num_brick_x; i++) {
        for (int j = 0; j < num_brick_y; j++) {
            if (bricks[i][j]) {
                int brick_x = i * (brick_width + 1);
                int brick_y = j * (brick_height + 1);
                display.fillRect(brick_x, brick_y, brick_width, brick_height, objects);
            }
        }
    }


int value = analogRead(A0);
paddle_x = map(value, 0, 1023, 0, 128 - paddle_width);

displayScore();
display.fillRect(paddle_x,paddle_y,paddle_width,paddle_height,objects);
display.fillRect(ball_x, ball_y, 1,1, objects);
item();
ball();
}

void ball(){
ball_x += ball_speed_x;
ball_y += ball_speed_y;

if (ball_x <= 0 || ball_x >= 127) {
        ball_speed_x = -ball_speed_x;
    }
    if (ball_y <= 0) {
        ball_speed_y = -ball_speed_y;
 }
   if(ball_y >=63){
    health--;
    ball_x=64.0, ball_y=32.0;
    ball_speed_x = ball_speed_x_l;
    ball_speed_y = ball_speed_y_l;
  }
if (ball_x + 1 >= paddle_x && ball_x <= paddle_x + paddle_width && ball_y + 1 >= paddle_y) {
    float relativeIntersectX = (paddle_x + paddle_width / 2) - ball_x;
    float angle = (PI / 4) * (relativeIntersectX / (paddle_width / 2));
    ball_y = paddle_y - 1;
    float magnitude = sqrt(ball_speed_x * ball_speed_x + ball_speed_y * ball_speed_y);
    ball_speed_x = sin(angle) * -magnitude;
    ball_speed_y = -cos(angle) * magnitude;

  }



  int brick_x=ball_x/(brick_width + 1) ;
  int brick_y=ball_y/(brick_height + 1);
  if (brick_x >= 0 && brick_x < num_brick_x && brick_y >= 0 && brick_y < num_brick_y) {
    if(bricks[brick_x][brick_y]){
       bricks[brick_x][brick_y]=false;
      int ball_x_int = (int)ball_x;
      int ball_y_int = (int)ball_y;
       if (((ball_x_int) % (brick_width + 1) == 0 && ball_speed_x > 0) || ((ball_x_int + 1) % (brick_width + 1) == 0 && ball_speed_x < 0)) 
    ball_speed_x = -ball_speed_x;
    else    
    ball_speed_y = -ball_speed_y;
       score++;
       if (random(10) == 0) {
        if(itemActive==false){
         itemX=ball_x;
         itemY=ball_y;
         itemActive=true;
        }
       }
        end=nobricks();
       
    }
   }


}
void item() {
  if (itemActive == true) {
    if (itemX >= paddle_x && itemX <= paddle_x + paddle_width && itemY >= paddle_y) {
      if (health < 3) {
        health++;
      }
      itemActive = false; 
    } else {
      if (itemY <= 63) {
        itemY++;
      } else {
        itemActive = false;
      }
      display.fillCircle(itemX, itemY, 1, objects);
    }
  }
}

boolean nobricks(){

for (int i = 0; i < num_brick_x; i++) {
        for (int j = 0; j < num_brick_y; j++) {
          if (bricks[i][j]) {
            return false;
          }
      }
    }

return true;
}
void newGame(){
ball_x=64.0;
ball_y=32.0;
brick_width=31;
 ball_speed_x_l=1.0;
 ball_speed_y_l=-1.0;
 ball_speed_x = 1.0;
 ball_speed_y = -1.0;
 score=0;
 health=3;
itemActive=false;

num_brick_x=128/(brick_width+1);
for(int i=0;i<num_brick_x;i++){
     for(int j=0;j<num_brick_y;j++){

         bricks[i][j]=true;
     }
  }

}
void nextLevel(){
ball_x=64.0;
ball_y=32.0;
ball_speed_x_l*=1.2;
ball_speed_y_l*=1.2;
brick_width=31;
brick_width=brick_width-(3*stage);

ball_speed_x = ball_speed_x_l;
ball_speed_y = ball_speed_y_l;
itemActive=false;

num_brick_x=128/(brick_width+1);
for(int i=0;i<num_brick_x;i++){
     for(int j=0;j<num_brick_y;j++){

         bricks[i][j]=true;
     }
  }

 if(stage<6)
 stage++;
}
void lights(){
if(health>0)
digitalWrite(healthLED1, HIGH);
else
digitalWrite(healthLED1, LOW);
if(health>1)
digitalWrite(healthLED2, HIGH);
else
digitalWrite(healthLED2, LOW);
if(health>2)
digitalWrite(healthLED3, HIGH);
else
digitalWrite(healthLED3, LOW);

}
void displayScore() {

int units = score % 10; 
int tens = (score / 10) % 10;
digitOutPut(1,units);
digitOutPut(0, tens);   
 
}
void digitOutPut(int d, int Number){
for (int i = 0; i < 7; i++) {
    digitalWrite(sevenSegmentPins[i], HIGH);
  }
  digitalWrite(digitPins[d], HIGH);
for (int i = 0; i < 7; i++) {
    digitalWrite(sevenSegmentPins[i], Values[Number][i]);
  }
  digitalWrite(digitPins[d], LOW);

}

