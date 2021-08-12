/*
 * This program is for a project called 'Wake-von'. It is a clock with alarm and also shows the temperature using a 4 digitits 7 segments 
 * display. Uses multipexation and 4 transistors 2n2222A to control de sequence of the displays and one more transistor for the bright, 
 * wich is calculated with the help of photoresistor. It has too a little buzzer and two buttons connected to the same analog pin.
 * 
 * It works like this:
 * 
 * Connected already to a supply, you need to push any of the buttons 
 * and it must start working. Shows temperature one second and then 
 * shows the time another second and so on. 
 * 
 * To SET the ALARM you have to press the first button for 3 seconds, 
 * after that you must hear the buzzer and stop pressing the button, 
 * because if you hold it for more than five seconds you get into 
 * the TIME SET UP.
 * 
 * In both SETUPs, the first button's function is now for selecting 
 * between hours and minutes and the second is used to change values.
 * And to save those changes and go out, you have to press again the 
 * first button for 3 seconds.
 * 
 * I still don't know why but, it might happen that when the buzzer 
 * sounds the displays turn off. When that happens is neccesary to 
 * press any button and it must 'fix' that.
 * 
 * To turn off the alarm you just have to get into the SETUP and go out.  * 
 * 
 * 
 * By Pablo Conde
 * pabloimconde@gmail.com
 * 
 */


//Libraries for clock
#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
//EEPROM used just to save the 'Alarm settings'
#include <EEPROM.h>
 
// Variables for the temperature sensor
float temp=0;
#define thermistor_pin A3
#define SERIESRESISTOR 10000 
#define THERMISTORNOMINAL 10000
#define TEMPERATURENOMINAL 25
#define BCOEFICENT 3950

// 
#define LDR A2 // photoresistor pin analog 2 of arduino
#define bright 6 //pin connected to the base of the transistor that controls the display's bright
#define buzzer A1

#define buttons A0//both buttons are connected to the analog 0 of the arduino
//botton  analog value: 767 +/- 50     . Holded for 5 seconds and enter to setup
#define b1val 770 
//botton2 analog value: 928 +/- 50     . In setup is used to change time (adding)
#define b2val 930

//counter for the time that button is holded, blinking of section: mins/hrs, button 2 is pressed for long it adds continuosly
unsigned long holded, blinking, button2;

boolean stillhold;//flag used to check if a button is still pressed

int segs[]={13,12,11,10,9,8,7};//pins for the 7-segment-displays. In order from A to G

//Represent:            0,       1,         2,        3,         4,        5,        6,       7,        8,        9,          , degree sign,    C,       F,        -
int numbers0_9[15]={ B1111110, B0110000, B1101101, B1111001, B0110011, B1011011, B1011111, B1110000, B1111111, B1111011, B0000000, B1100011, B1001110,B1000111, B0000001};
//
int transistor[]={2,3,4,5};//from left to right. Connected to the transistors's bases. Remember a resistor.
int displays=4;//of the transistor list
int positionDisplay;//used to turn one display per time
int nums[]={0,0,0,0};//This is a case of how it is used and interpreted: nums[1,2,3,0] = 12:30<--Time
//another example: nums[3,4,11,12]=34*C

boolean showw=true;//true is to show Time, false to show temperature.

//flags for buttons
boolean bstate=false;
boolean bstate2=false;

int pscn=0;// past second. used to know if a second has passed
int interval=0;//counter

uint8_t scn=0;//seconds counter
uint8_t mins=0;//minutes counter
uint8_t hrs=0;//hours counter

uint8_t hrsA,minsA;//hour of alarm, minute of alarm
uint8_t selection=2;//1, for setup mins, 2 for hrs
uint8_t cnslt=0;//counter per selection. Just for setups. 0 is out,1 is in setup of alarm, 2 is in setup of time

boolean insetTime=false;//flag to check if is doing setup of time(hour)
boolean insetAlarm=false;//flag to check if is doing setup of time of alarm

boolean segstate=1;//default 1 to turn on. 0 when it is in a setup and blinks a section (hour/minute)

boolean act=false;//alarm active. Flag to know if alarm must be on or if someone has turned it off.
boolean repeatAlarm = true;

tmElements_t datetime;//declare datetime to get and set time

void setup() {
  //Declare all ports for the displays and transistors as OUTPUTS
  for(int i=0; i<7; i++){
    pinMode(segs[i],OUTPUT);
  }
  for(int i=0;i<displays; i++){
    pinMode(transistor[i],OUTPUT);
  }
  pinMode(bright,OUTPUT);//for brightness
  pinMode(buzzer,OUTPUT);
  //read time from clock
  updateTime();
  //get 'Alarm settings' from EEPROM
  hrsA=EEPROM.read(1);
  minsA=EEPROM.read(2);
  
}

void loop() {
  
  counter_displays();//Go to everything happens.
  
}

void counter_displays(){
  while(true){ //infinite loop
    //set brightness
    int brightness;
    brightness=map(analogRead(LDR), 0, 1023, 5, 240);
    //taking the value it might be from 0 to 1023 and as minimum result
    //of 5 and a maximum of 240
    analogWrite(bright,brightness);
    readtime();
    
    if(act && analogRead(buttons)>400)repeatAlarm=false;//if button is pressed when alarm is active
    
    if(scn!=pscn){
      pscn=datetime.Second;
      interval++;
      if(interval==1){
        interval=0;
        showw=!showw;
      }
      
      alarm();//check for alarm
      
      if(showw || insetAlarm || insetTime){//showing time
        updateTime();
      }
      else{//showing temperature
        temp=get_temp();
        updateTemp();
      }
    }






   if(analogRead(buttons)>(b1val-25) && analogRead(buttons)< (b1val+25)){
      if(bstate==false){
        holded=millis();
        if(insetAlarm || insetTime){
          if(selection>1){
            selection-=1;
          }
          else{
            selection=2;
          }
        }
      }
      bstate=true;
      if((millis()-holded)>2500){//botton holded
        //turn on a led to see it is time setup, or make a sound
        //Serial.println("holded.   ");
        
        if((insetTime || insetAlarm) && stillhold==false){//was it in the setup
          //Serial.println("go out setup of alarm/time");
          if(insetAlarm && insetTime==false){
            changeAlarm();
          }
          insetTime=false;
          insetAlarm=false;
          selection=0;//reset all values
          blinking=0;
          segstate=1;//become to normallity. no more blinking
          holded=millis();
        }
        else if(millis()-holded>4500){
          //Serial.print("in setTime");
          if(insetTime==false)sound_effect();
          insetTime=true;
          insetAlarm=false;
          //blinking=millis();
        }
        else{
          stillhold=true;
          //Serial.print("in setAlarm");
          if(insetAlarm==false)sound_effect();
          insetAlarm=true;
          insetTime=false;
          blinking=millis();          
        }
      }
      
    }
    else{
      holded=millis();
      bstate=false;
      stillhold=false;
    }
    if(insetTime || insetAlarm){
      if(analogRead(buttons)>(b2val-25) && analogRead(buttons)< (b2val+25)){
        if(bstate2==false){
          select();
          button2=millis();
        }
        bstate2=true;
        if(millis() - button2 > 110){
          select();
          button2=millis();
        }
      }
      else{
        bstate2=false;
      }
    }


    if(true){
      if(positionDisplay<displays){
        positionDisplay+=1;
      }
      else{
        positionDisplay=0;
      }
      offAll();


      if(insetTime || insetAlarm){
        if(millis()-blinking>=100){
          if(segstate==0){
            segstate=1;//for blinking of selection(hour or minutes displays)
          }
          else{
            segstate=0;
          }
          blinking=millis();
        }
        switch(selection){
          case 1:
          if((positionDisplay==2) || (positionDisplay==3)){
            digitalWrite(transistor[positionDisplay],segstate);
          }
          else{
            digitalWrite(transistor[positionDisplay],HIGH);
          }
          break;
          case 2:
          if((positionDisplay==0) || (positionDisplay==1)){
            digitalWrite(transistor[positionDisplay],segstate);
          }
          else{
            digitalWrite(transistor[positionDisplay],HIGH);
          }
          break;
        }
      }
      else{
        digitalWrite(transistor[positionDisplay],HIGH);
      }




      
      number(nums[positionDisplay]);//and from that is converted to Int
    }
  }
}


void alarm(){
//time of next repetitions
  int sch[3][2] = {
    {hrsA, minsA},
    {},
    {}
  };
//time when alarm stops
  int schSTOP[3][2] = {
    {hrsA, minsA},
    {},
    {}
  };
  
  for(int j=0; j<3; j++){
    if(j>0){
      sch[j][0] = sch[j-1][0];
      sch[j][1] = sch[j-1][1];
      sch[j][1]+=5;
      re(&sch[j][1]);
    }
    schSTOP[j][0] = sch[j][0];
    schSTOP[j][1] = sch[j][1] + 2;
    re(&schSTOP[j][1]);
  }
//
  if(repeatAlarm){
    for(int i=0; i<3; i++){
      if(hrs==sch[i][0] && mins==sch[i][1] && scn<3)act = true;
    }
    for(int i=0; i<3; i++){
      if(hrs==schSTOP[i][0] && mins==schSTOP[i][1] && scn<3)act = false;
    }
  }
  else if(hrs==schSTOP[2][0] && mins==schSTOP[2][1] && scn<3){
    act = false;
    repeatAlarm= true;
  }
  else{
    act=false;
  }
  
  if(showw && !(insetTime || insetAlarm) && act){
    alarm_event(1);
  }
  else{
    alarm_event(0);
  }
}

void re(int * var){//direction of var[row][column]
  if(*var >= 60){
      *var -= 60;
      *(var-1) += 1;//addres of var[row][column - 1]
  }
}

void alarm_event(bool var){
  //digitalWrite(lamp,var);
  if(var==1){
    tone(buzzer,300);
  }
  else{
    noTone(buzzer);  
  }
}


void offAll(){
  offistors();
  off();
}
void offistors(){
  for(int i=0;i<displays;i++){
    digitalWrite(transistor[i],0);
  }
}
void off(){
  number(10);
}
void number(int num){//the number corresponds to index of numbers0_9
  for(int i=0;i<7;i++){//is applied to all the 7-segments
    digitalWrite(segs[i], bitRead(numbers0_9[num], 6-i));//it reads the byte from right
  }  
}
float get_temp(){
  float temp;
  temp=analogRead(thermistor_pin);
  temp = 1023 /temp-1;
  temp = SERIESRESISTOR/temp;
  float steinhart;
  steinhart = temp / THERMISTORNOMINAL;
  steinhart = log(steinhart);
  steinhart /= BCOEFICENT;
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15);
  steinhart = 1.0 / steinhart;
  steinhart -= 273.15; 
  return steinhart;
}

void readtime(){
  if(RTC.read(datetime)){
    scn=datetime.Second;
    hrs=datetime.Hour;
    mins=datetime.Minute;
  }  
}

void updateTime(){
  if(RTC.read(datetime)){
        
    if(insetAlarm){
      hrs=hrsA;
      mins=minsA;
    }
    else{
      hrs=datetime.Hour;
      mins=datetime.Minute;      
    }

      nums[0]=hrs/10;
      nums[1]=hrs-(10*(hrs/10));
      nums[2]=mins/10;
      nums[3]=mins-(10*(mins/10));     
  }
  else{
    if (RTC.chipPresent()){//need to set up again
      hrs=0;
      mins=0;
      changeTime();
    }
    else{//bad connections
      nums[0]=15;
      nums[1]=14;
      nums[2]=15;
      nums[3]=14;
    }
  }  
}

void updateTemp(){
  int positive;
  positive = int(temp);
  if(temp>=0){
    nums[0]=positive/10;
    nums[1]=positive-(10*(positive/10));
    nums[2]=11;// degrees sign
    nums[3]=12;//position wich has a letter "C" (for centigrades)
  }
  else{// fix details
    nums[0]=15;//draw a line for "-" sign
    nums[1]=positive/10;
    nums[2]=positive-(10*(positive/10));
    nums[3]=11;//position wich has a letter "C" (for centigrades)        
  }  
}

void select(){
  if(insetTime==true && insetAlarm==false){
    //added
    switch(selection){
      case 1:
      mins+=1;
      if(mins==60)mins=0;
      break; 
      case 2:
      hrs+=1;
      if(hrs==24)hrs=0;
      break;                              
    }
    changeTime();
  }
  else{
    switch(selection){
      case 1:
      minsA+=1;
      if(minsA>=60)minsA=0;
      break; 
      case 2:
      hrsA+=1;
      if(hrsA>=24)hrsA=0;
      break;                              
    }
  }

}

void changeTime(){
  setTime(hrs,mins,0,1,1,1);
  RTC.set(now());  
}

void changeAlarm(){
  EEPROM.update(1,hrsA);
  EEPROM.update(2,minsA);
}

void sound_effect(){
  tone(buzzer,300);
  delay(100);
  noTone(buzzer);
}

/*
 * 
 */
