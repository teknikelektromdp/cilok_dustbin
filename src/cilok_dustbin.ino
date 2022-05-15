/*
 * Smart Dustbin v1.2 
 * Made by Marco and Wahyu
 * Universitas Multi Data Palembang
*/

//Libraries
#include <LiquidCrystal_I2C.h>
#include <NewPing.h>
#include <Wire.h>
#include <Servo.h>

//Constants
#define SONAR_NUM 2      // Number of sensors.
#define MAX_DISTANCE_IN 70 // Maximum distance (in cm) to ping.
#define MAX_DISTANCE_OUT 20 // Maximum distance (in cm) to ping.
#define CLOSE 0// Minimum angle for servo.
#define OPEN 100// Maximum angle for servo.
#define buzzer 19//Buzzer pin
#define servo 15//Servo pin

//Variables
//millis in
unsigned int period_in = 300;
unsigned int time_now_in = 0;
// Garbage level = (Depth - Height) / Depth 
float height = 0; // height measured by ultrasonic in cm
float garbage;
unsigned int garbage_percentage;
// To detect whether there is human detected
unsigned int human; 

//Classes
NewPing sonar[SONAR_NUM] = {   // Sensor object array.
  NewPing(22, 23, MAX_DISTANCE_IN), // Each sensor's trigger pin, echo pin, and max distance to ping. 
  NewPing(21, 20, MAX_DISTANCE_OUT)
};
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
Servo Pintu; //set Servo object as Pintu

//Methods
void setup() {
  lcd.init();
  Pintu.attach(servo);//set pinout for servo    
  pinMode(buzzer, OUTPUT);
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(3,0);
  lcd.print("Cilok Dustbin");
  lcd.setCursor(2,3);
  lcd.print("v1.2");
  Pintu.write(CLOSE);
  delay(5000);
  lcd.clear();
}

void loop() {

  if(millis() > time_now_in + period_in){
    time_now_in = millis();
    // Loop through each sensor and display results.
    delay(50); // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
    //show the garbage level
    garbage_level_lcd();
    //lcd.setCursor(11,0)  ;
    //lcd.print(human);
  }
  if(garbage_level() < 90)
  {
    if(human_detection() > 0)
    {
      Pintu.write(OPEN);
      delay(4000);
      alarm_on_open();
      close_slowly();
      //clear the LCD
      lcd.clear(); 
      
    }
    else
    {
      Pintu.write(CLOSE);
      lcd.setCursor(1,0);
      lcd.print("Not Detected");
      delay(300);
      lcd.clear();
    } 
  }
  else
  {
    lcd.setCursor(1,0);
    lcd.print("Full");
    delay(300);
    alarm();
    lcd.clear();
    //Set buzzer to high
  }   
}

//show the garbage level on the LCD
void garbage_level_lcd()
{
  height = sonar[0].ping_cm();
  garbage = (MAX_DISTANCE_IN - height) / MAX_DISTANCE_IN;
  garbage = garbage * 100;
  garbage_percentage = int(garbage);
  lcd.setCursor(0,1);
  lcd.print("Capacity:<");
  lcd.setCursor(11,1);
  if(garbage_percentage>=0 && garbage_percentage<=20)
  {
    lcd.print("20");
  }
  else if(garbage_percentage>20 && garbage_percentage<=40)
  {
    lcd.print("40");
  }
  else if(garbage_percentage>40 && garbage_percentage<=60)
  {
    lcd.print("60");
  }
  else if(garbage_percentage>60 && garbage_percentage<90)
  {
    lcd.print("80");
  }
  else
  {
    lcd.print("100");
  }
  lcd.setCursor(14,1);
  lcd.print("%");
  //delay(1000);
  //lcd.clear();
}

//garbage level to be used for conditional purposes
int garbage_level()
{
  height = sonar[0].ping_cm();
  garbage = (MAX_DISTANCE_IN - height) / MAX_DISTANCE_IN;
  garbage = garbage * 100;
  garbage_percentage = int(garbage);
  return garbage_percentage;
}

//human/motion detection
int human_detection()
{
  human = sonar[1].ping_cm();
  return human;   
}


//close the cover slowly after a few seconds
void close_slowly()
{
  for(int i=OPEN; i>=0; i--)
  {
    Pintu.write(i);
    delay(10);    
  }  
}

//start the alarm when the dustbin is full
void alarm(){
  digitalWrite(buzzer, HIGH);
  delay (1000);
  digitalWrite(buzzer, LOW);
  delay (1000);
}

//start the alarm when the motion is detected
void alarm_on_open(){
  digitalWrite(buzzer, HIGH);
  delay (500);
  digitalWrite(buzzer, LOW);
}
