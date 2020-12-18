/*
 * Ultimate64 RGB led controller
 *  
 * Code by M.F. Wieland ( TSB ) 2020
 * V 1.0
 * 
 * License information: See LICENSE file on the github repository
 */
 
#include <EEPROM.h>

#define PRINT(s, v) { Serial.print(F(s)); Serial.print(v,HEX); }

#define in_pwr  2
#define in_act  3

#define out_red 9
#define out_green 5
#define out_blue 6

char pwr_r = 0;
char pwr_g = 25;
char pwr_b = 0;

char act_r = 25;
char act_g = 0;
char act_b = 0;

char tst_r = 0;
char tst_g = 25;
char tst_b = 0;

int current_setting = 0; // 1=pwr, 2=act

char s_buf;   // incoming serial data

void setup() {

  // Setup serial communication at 300 baud
  Serial.begin(300);

  // set led outputs
  pinMode(out_red,OUTPUT);
  pinMode(out_green,OUTPUT);
  pinMode(out_blue,OUTPUT);

  // load led color from eprom
  read_eprom();

  // set pwr led
  analogWrite(out_red,pwr_r);
  analogWrite(out_green,pwr_g);
  analogWrite(out_blue,pwr_b);

  // Set interrupts on drive act pin
  // uno has 2 interrupts, pin2(0) & 3(1)
  attachInterrupt(1, rising, RISING);
}

char serial_read_next()
{
  char  c;
  
  while (!Serial.available())
  {
    // wait until next input..
  }
  c = Serial.read();

  return(c);
}
void loop() {

  // proces serial input data if available
  if (Serial.available()) 
  {
    s_buf =serial_read_next();
    switch( toupper(s_buf) ) 
    {
      /*
       * Read power or activity data from eeprom
       */
      case 'R' :
        s_buf =serial_read_next();
        switch( toupper(s_buf) ) 
        {
          case 'P' : 
            current_setting = 1;
            
            tst_r = EEPROM.read(1);
            tst_g = EEPROM.read(2);
            tst_b = EEPROM.read(3);
            break;
        
          case 'A' : 
            current_setting = 2;
            
            tst_r = EEPROM.read(4);
            tst_g = EEPROM.read(5);
            tst_b = EEPROM.read(6);
            break;
        }
        // set zero values to 255, will be changed back to zero in C64 code and set values
        Serial.write( (tst_r==0) ? 255 : tst_r );
        Serial.write( (tst_g==0) ? 255 : tst_g );
        Serial.write( (tst_b==0) ? 255 : tst_b );        
        
        test_color();
        break;

      /*
       * Test led color values received from U64
       */
      case 'T' :
        tst_r = serial_read_next();
        tst_g = serial_read_next();
        tst_b = serial_read_next();
        
        test_color();
        break;

      /*
       * write power or activity data from eeprom
       */
      case 'W' :
        analogWrite(out_red,tst_r);
        analogWrite(out_green,tst_g);
        analogWrite(out_blue,tst_b);
        
        write_eprom();
        break;
      }
  }
}

/*
 * Read power or activity data from eeprom
 */
void read_eprom() 
{
  // write init values to eprom
  if( EEPROM.read(0)!=0xFF ) 
  {
    EEPROM.write(0, 0xFF);
    EEPROM.write(1, pwr_r);
    EEPROM.write(2, pwr_g);
    EEPROM.write(3, pwr_b);
    EEPROM.write(4, act_r);
    EEPROM.write(5, act_g);
    EEPROM.write(6, act_b);
  }

  // read values from eprom
  pwr_r = EEPROM.read(1);
  pwr_g = EEPROM.read(2);
  pwr_b = EEPROM.read(3);
  act_r = EEPROM.read(4);
  act_g = EEPROM.read(5);
  act_b = EEPROM.read(6);
}

/*
 * Write power or activity data from eeprom
 */
void write_eprom()
{

  switch (current_setting ) 
  {
    case 1 : 
      EEPROM.write(1, tst_r);
      EEPROM.write(2, tst_g);
      EEPROM.write(3, tst_b);
      
      // update active power led color
      pwr_r = tst_r;
      pwr_g = tst_g;
      pwr_b = tst_b;
      
      break;
    
    case 2 : 
      EEPROM.write(4, tst_r);
      EEPROM.write(5, tst_g);
      EEPROM.write(6, tst_b);

      // update active activity led color
      act_r = tst_r;
      act_g = tst_g;
      act_b = tst_b;
      break;
  }
}

/*
 * Test active color values 
 */
void test_color()
{
  analogWrite(out_red,tst_r);
  analogWrite(out_green,tst_g);
  analogWrite(out_blue,tst_b);
}

/*
 * Show activity led color
 * Set interupt for power led
 */
void rising() 
{
  // activate drive act led
  analogWrite(out_red,act_r);
  analogWrite(out_green,act_g);
  analogWrite(out_blue,act_b);

  // set new interrupt
  attachInterrupt(1, falling, FALLING);
}
 
/*
 * Show power led color
 * Set interupt for activity led
 */
void falling() 
{
  // activate pwr led
  analogWrite(out_red,pwr_r);
  analogWrite(out_green,pwr_g);
  analogWrite(out_blue,pwr_b);

  // set new interrupt
  attachInterrupt(1, rising, RISING);
}
