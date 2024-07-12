
#include <Servo.h>

Servo servo1;  // create servo object to control a servo
Servo servo2;  // create servo object to control a servo
Servo servo3;  // create servo object to control a servo
Servo servo4;  // create servo object to control a servo
Servo servo5;  // create servo object to control a servo
Servo servo6;  // create servo object to control a servo
Servo servo7;  // create servo object to control a servo
Servo servo8;  // create servo object to control a servo

#define FACTOR_Distancia 65  // factor de correccion distancia 100/100
#define FACTOR_RPM 99  // factor de correccion Revoluciones
#define RADIORUEDAM 0.101  // Radio de la rueda en metros

#define EnconderSampleTimems 500//1000
#define ReadEnconderRxTimeOutms 33  // usado en timeout de la recepcion del roboclaw 
#define TIMEOUTSERIALBEAGLE 200

#define pin_servo1 7
#define pin_servo2 6
#define pin_servo3 5
#define pin_servo4 4
#define pin_servo5 3  //Led Right
#define pin_servo6 2
#define pin_servo7 1  //Led Left
#define pin_servo8 0

#define err_servo1  36
#define err_servo2  34
#define err_servo3  32
#define err_servo4  30
#define err_servo5  28
#define err_servo6  26
#define err_servo7  24
#define err_servo8  22

#define DD2         27
#define err_485     25
#define V12_in      23
#define V24_in      44
#define SYS_5V      42
#define beagle_rst  40
#define RBC_in      38
#define V24_adc     15 

int V24_val;    // variable to read the value from the analog pin
byte receive_buffer[40] ={0};
byte transmit_buffer[40] ={0};

float joystick_axes[4] ={0};
byte joystick_buttons[17] ={0};

int pos_servo[8]={0};
float life = 0.0;

int8_t target_speed[2] = {0};
int8_t current_speed[2] = {0};
int8_t max_speed[4] ={50,-50,50,-50};
int8_t acc_speed = 5;
int8_t dec_speed = 5; //10
int8_t throttle_speed = 30;
int8_t reverse_speed = 30; 
int8_t MovimientoExplora=0; // =1: avanza, =2: retrocede, 
float distancef;
int8_t contaerror;

int8_t ini = 1;
int dat=0;
/*
//Inicio
byte speedin1[5] ={0x80,0,0,0x00,0x00};
byte speedin2[5] ={0x80,4,0,0x00,0x00};
byte speedin3[5] ={0x81,0,0,0x00,0x00};
byte speedin4[5] ={0x81,4,0,0x00,0x00};
*/


byte EncoderDirection;
       
void setup() {
   pin_servo_setup();
   //delay(3600000);
   //ini = digitalRead(0);
   //Serial.begin(9600,SERIAL_8N1);
   distancef=0.0;
   contaerror=0;
}

void loop() 
{
 unsigned long MyRPM;
 unsigned long MytimeOld,MyTotaltime;
 unsigned long EncodeOld,EncodeNew;
 int n_rcv;
   
    //Serial.write(0x41); 

  EncodeOld=ReadEnconder();
  MytimeOld=millis();
  MyTotaltime=0;
  while(MyTotaltime<EnconderSampleTimems)
  {
//  Serial.write(0x30);
//  n_rcv = Serial1.readBytes(receive_buffer,35); 

  n_rcv = RecibetramaBeagle(35);
  if(n_rcv>0) contaerror=0;
  
   dat=n_rcv;
  ini = digitalRead(0);
  process_command_and_responde(receive_buffer);  
  MyTotaltime=millis()-MytimeOld;   
  }
  EncodeNew=ReadEnconder();
  MyRPM=CalculaRPM(EncodeOld,EncodeNew,MyTotaltime);
  CalculaDistancia(MyRPM);  // modifica distancef


//  Serial.println(MyTotaltime, DEC);
// Serial.println(MyRPM, DEC);
  //Serial.println(EncodeOld, HEX);   
  //Serial.println(EncodeNew, HEX);   

/*
  if(MyRPM!=-1) Serial.println(MyRPM, DEC);
  else Serial.print("e\n"); 
  */
  //delay(2000);
}

void process_command_and_responde(byte* receive_buffer)
{
  int head=0;
  head = read_joystick(); //delay(10);
  if(head==1 && dat!=0){
  process_joystick(); //delay(10);
  write_servos(); //delay(10);
  command_roboclaw(0); //delay(10); debe ser el roboclaw de arriba
  command_roboclaw(1); //delay(10);
  answer();
  head=0;
  }
  else
  {  // Agregado LaloRP 

   if(contaerror>10)
    {
     target_speed[0] = 0;
    target_speed[1] = 0;
    command_roboclaw(0); //delay(10); debe ser el roboclaw de arriba
    command_roboclaw(1); //delay(10);  
    }
 
  }
  
  //transmit_buffer[2]=V24_val;
  //transmit_buffer[3]=V24_val>>8;
  //cal_battery(V24_val); delay(10);
  //if(Serial.available()>0){Serial1.write(transmit_buffer,5);}
  //
}

void answer()
{
  cal_battery(); //delay(10);
  //Serial1.write(transmit_buffer,2);
  Serial1.print("$EO");
  //Serial1.print(V24_val); 
  Serial1.print(life);
  Serial1.print(',');
  Serial1.print(throttle_speed);
  Serial1.print(',');
  Serial1.print(reverse_speed);
  Serial1.print(',');
  Serial1.print(distancef);
  Serial1.print('#');
}

void cal_battery()
{
  V24_val = analogRead(V24_adc); 
  delay(10);
  //life = ((float)V24_val)*(5.32)/(0.99*204.6);  //40.92
  life = ((float)V24_val)/(0.176*204.6);  //40.92
}

////// Servos //////
void write_servos(void){
  for(int i;i<8;i++){
    if (pos_servo[i]<0) pos_servo[i]=0;
    else if (pos_servo[i]>180) pos_servo[i]=180;
  }
  servo1.write(pos_servo[0]);
  servo2.write(pos_servo[1]);
  servo3.write(pos_servo[2]);
  servo4.write(pos_servo[3]);
  servo5.write(pos_servo[4]);
  servo6.write(pos_servo[5]);
  servo7.write(pos_servo[6]);
  servo8.write(pos_servo[7]);
}


/// Control Motor DC ///
void command_roboclaw(uint8_t n){

byte incomingByte;

 // target 0 => Right motor  ///  target 1 => Left motor
 if (target_speed[n]>0 && current_speed[n]>=0 && current_speed[n]<=target_speed[n]){
      current_speed[n]+=acc_speed;
        if (current_speed[n]>target_speed[n]){current_speed[n]=target_speed[n];}}
 else if (target_speed[n]>0 && current_speed[n]<0){
      current_speed[n]=0;}

 else if (target_speed[n]<0 && current_speed[n]<=0 && current_speed[n]>=target_speed[n]){
      current_speed[n]-=acc_speed;
        if (current_speed[n]<target_speed[n]){current_speed[n]=target_speed[n];}}
 else if (target_speed[n]<0 && current_speed[n]>0){
    current_speed[n]=0;}
 else {
    current_speed[n]=0;}

 byte rbc_tx_buffer[5] ={0x80+n,0,abs(round(current_speed[n])),0x00,0x00};
 
 if (current_speed[n]>=0){
     rbc_write(rbc_tx_buffer);
     incomingByte = Serial3.read();  // Agregado LaloRP
     rbc_tx_buffer[1]= 4;
     rbc_write(rbc_tx_buffer);
     incomingByte = Serial3.read();  // Agregado LaloRP
     }
 else {
    rbc_tx_buffer[1]= 1;
    rbc_write(rbc_tx_buffer);
    incomingByte = Serial3.read();  // Agregado LaloRP
    rbc_tx_buffer[1]= 5;
    rbc_write(rbc_tx_buffer);
    incomingByte = Serial3.read();  // Agregado LaloRP
    }
 }
 
byte rbc_write(byte* tx_buffer)
{
    unsigned int crc = crc16(tx_buffer,3);
    tx_buffer[3]= highByte(crc);
    tx_buffer[4]= lowByte(crc);
    Serial3.write(tx_buffer,5);
    byte rx_byte[1] = {0}; //byte n_byte=0;
    byte n_byte = Serial3.readBytes(rx_byte,1);
    if (!n_byte || (rx_byte[0]!=0xFF)) return -1;
    return 0;
}

//Calculates CRC16 of nBytes of data in byte array message
unsigned int crc16(unsigned char *packet, int nBytes) {
    unsigned  int crc = 0;
    for (int byte = 0; byte < nBytes; byte++) {
       crc = crc ^ ((unsigned int)packet[byte] << 8);
        for (unsigned char bit = 0; bit < 8; bit++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ 0x1021;
            } else {
                crc = crc << 1;
            }
        }
    }
    return crc;
}

void process_joystick(void){
  // color_buttons (right) control speeds and speed_limits

     MovimientoExplora=0;   // Agregado LaloRP
   
  // x, y
  if (joystick_buttons[2]){
    if(joystick_buttons[12] && reverse_speed<80){
      reverse_speed=reverse_speed+acc_speed;
      }
    else if (joystick_buttons[13] && reverse_speed>10){
      reverse_speed=reverse_speed-dec_speed;
      }
    }
  else if (joystick_buttons[3]){
    if(joystick_buttons[12] && throttle_speed<80){
      throttle_speed=throttle_speed+acc_speed;
      }
    else if (joystick_buttons[13] && throttle_speed>10){
      throttle_speed=throttle_speed-dec_speed;
      }
    }
   /*
  if (joystick_buttons[2] && throttle_speed>10){
    throttle_speed=throttle_speed-dec_speed;
    }
  else if (joystick_buttons[3] && throttle_speed<80){
    throttle_speed=throttle_speed+acc_speed;      
    }
  */
  
  else if (joystick_buttons[0]){
   target_speed[0] = throttle_speed;
   target_speed[1] = throttle_speed;
   MovimientoExplora=1;   // Agregado LaloRP
   }
  
  else if (joystick_buttons[1]){
   target_speed[0] = -reverse_speed;
   target_speed[1] = -reverse_speed; 
   MovimientoExplora=2;   // Agregado LaloRP
  }
  /*
  else if (joystick_buttons[2]){
   if (joystick_buttons[5]){
    max_speed[0] += acc_speed; if (max_speed[0]>100) max_speed[0]=100; 
    max_speed[2] += acc_speed; if (max_speed[2]>100) max_speed[2]=100;} 
   else {
    max_speed[0] -= acc_speed; if (max_speed[0]<10) max_speed[0]=10;
    max_speed[2] -= acc_speed; if (max_speed[2]<10) max_speed[2]=10;}}
  
  else if (joystick_buttons[3]){
   if (joystick_buttons[5]){
    max_speed[1] -= dec_speed; if (max_speed[1]<-100) max_speed[1]=-100; 
    max_speed[3] -= acc_speed; if (max_speed[3]<-100) max_speed[3]=-100;} 
   else {
    max_speed[1] += acc_speed; if (max_speed[1]>-10) max_speed[1]=-10;
    max_speed[3] += acc_speed; if (max_speed[3]>-10) max_speed[3]=-10;}}
    */

// dpad controls servos

  else if (joystick_buttons[12]){
   if      (!joystick_buttons[4] && !joystick_buttons[5]) pos_servo[0]+=10;
   else if (!joystick_buttons[5])                         pos_servo[4]+=10;
   else if (!joystick_buttons[4])                         pos_servo[6]+=10;
   else                                                   pos_servo[2]+=10;}
 
  else if (joystick_buttons[13]){
   if      (!joystick_buttons[4] && !joystick_buttons[5]) pos_servo[0]-=10;
   else if (!joystick_buttons[5])                         pos_servo[4]-=10;
   else if (!joystick_buttons[4])                         pos_servo[6]-=10;
   else                                                   pos_servo[2]-=10;}
     
  else if (joystick_buttons[14]){
   if      (!joystick_buttons[4] && !joystick_buttons[5]) pos_servo[1]-=10;
   else if (!joystick_buttons[5])                         pos_servo[5]-=10;
   else if (!joystick_buttons[4])                         pos_servo[7]-=10;
   else                                                   pos_servo[3]-=10;}
      
  else if (joystick_buttons[15]){
   if      (!joystick_buttons[4] && !joystick_buttons[5]) pos_servo[1]+=10;
   else if (!joystick_buttons[5])                         pos_servo[5]+=10;
   else if (!joystick_buttons[4])                         pos_servo[7]+=10;
   else                                                   pos_servo[3]+=10;}
   
// triggers

  else if (joystick_buttons[6]){ // Left Gyro
      target_speed[0] = (unsigned int)(throttle_speed*(float)joystick_buttons[6]/255.0);
      target_speed[1] = -(unsigned int)(reverse_speed*(float)joystick_buttons[6]/255.0);   
    }
  else if (joystick_buttons[7]){ // Right Gyro
      target_speed[0] = -(unsigned int)(reverse_speed*(float)joystick_buttons[7]/255.0);
      target_speed[1] = (unsigned int)(throttle_speed*(float)joystick_buttons[7]/255.0); 
    }


 /// modificado por LaloRP
 else if (joystick_buttons[9]) distancef=0.0; // resetea distancia
  /// Fin modificacion por LaloRP
    
// axes_not configured yet.
  else {
   target_speed[0] = 0;
   target_speed[1] = 0;}
}


int read_joystick(void)
{
  byte temp[4] = {0};
  if ((receive_buffer[0] == 'X') && (receive_buffer[1] == 'P'))
  {   
   int k=2;
    for(int i=0;i<4;i++){
      temp[0] = receive_buffer[k+4*i];
      temp[1] = receive_buffer[k+4*i+1];
      temp[2] = receive_buffer[k+4*i+2];
      temp[3] = receive_buffer[k+4*i+3];  
      joystick_axes[i]= *((float*)(temp));  
    }
    k= 18;
    for(int i=0;i<=17;i++){
      joystick_buttons[i]=receive_buffer[k+i];
    }
    return 1;
  }
  else{
    return 0;
  }
}

/// Config Servo ///
void pin_servo_setup(void){
  servo1.attach(pin_servo1);  // attaches the servo on pin 9 to the servo object
  servo2.attach(pin_servo2);  // attaches the servo on pin 9 to the servo object
  servo3.attach(pin_servo3);  // attaches the servo on pin 9 to the servo object
  servo4.attach(pin_servo4);  // attaches the servo on pin 9 to the servo object
  servo5.attach(pin_servo5);  // attaches the servo on pin 9 to the servo object
  servo6.attach(pin_servo6);  // attaches the servo on pin 9 to the servo object
  servo7.attach(pin_servo7);  // attaches the servo on pin 9 to the servo object
  servo8.attach(pin_servo8);  // attaches the servo on pin 9 to the servo object

  pinMode(err_servo1,INPUT);
  pinMode(err_servo2,INPUT);
  pinMode(err_servo3,INPUT);
  pinMode(err_servo4,INPUT);
  pinMode(err_servo5,INPUT);
  pinMode(err_servo6,INPUT);
  pinMode(err_servo7,INPUT);
  pinMode(err_servo8,INPUT);

  pinMode(err_485,INPUT);
  pinMode(V12_in,INPUT);
  pinMode(V24_in,INPUT);
  pinMode(SYS_5V,INPUT);
  pinMode(RBC_in,INPUT);
  
  digitalWrite(beagle_rst,HIGH);
  pinMode(beagle_rst,OUTPUT);
  digitalWrite(DD2,HIGH);
  pinMode(DD2,OUTPUT);

  servo1.write(90);
  servo2.write(90);
  servo3.write(0);
  servo4.write(0);
  servo5.write(0);
  servo6.write(0);
  servo7.write(0);
  servo8.write(0);

  transmit_buffer[0]='E'; 
  transmit_buffer[1]='O';

  //Beagle reset initial
  //digitalWrite(beagle_rst,LOW);
  //delay(1000);
  //digitalWrite(beagle_rst,HIGH);
  
//  Serial1.begin(9600,SERIAL_8N1);  // Beaglebone Black
  Serial1.begin(19200,SERIAL_8N1);  // Beaglebone Black  MODIFICADO POR LALORP

  Serial2.begin(9600,SERIAL_8N1);
  Serial3.begin(9600,SERIAL_8N1);  // Roboclaw

  Serial1.setTimeout(500);
  Serial2.setTimeout(500);
  Serial3.setTimeout(500);
/*
  rbc_write(speedin1); delay(10);
  rbc_write(speedin2); delay(10);
  rbc_write(speedin3); delay(10);
  rbc_write(speedin4); delay(10);
*/
}

void Reset()
{
   //Beagle reset initial
  digitalWrite(beagle_rst,LOW);
  delay(1000);
  digitalWrite(beagle_rst,HIGH);
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 unsigned long ReadEnconder(void)// rpm 
 {
 byte MybufferEncoder[6];
 byte incomingByte,i;
 unsigned long Encoder1;
  
 byte BufferTx[5] ={0x80,17,0,0,0};

delay(10);
//  Serial3.flush();
while(Serial3.available() > 0) incomingByte = Serial3.read();
    
rbc_write(BufferTx);
i=ReadEnconderRxTimeOutms;
while(Serial3.available()<4)
{
   delay(1);
   i--;
   if(i==0) break;
}

  i=0;
  while(Serial3.available() > 0) {
     incomingByte = Serial3.read();
 //           Serial.write(incomingByte);
     if(i==0)
      {
        if(incomingByte!=0xFF)
         {
          MybufferEncoder[i]=incomingByte;
//                          Serial.write(incomingByte);
          i++;
         }  
      }
      else
      {
        if(i<4)
        {
          MybufferEncoder[i]=incomingByte;
//                  Serial.write(incomingByte);
          i++;        
          
        }
      }
    
  } // fin del while Serial3.available

 EncoderDirection=MybufferEncoder[3];
 Encoder1=(((unsigned long)MybufferEncoder[0])<<16)+ (((unsigned long)MybufferEncoder[1])<<8)+((unsigned long)MybufferEncoder[2]);
if((EncoderDirection!=0x80)&&(EncoderDirection!=0x82)) return -1;
// if((EncoderDirection!=0x80)&&(EncoderDirection!=0x82))Serial.write(0x65) ;

 
 return Encoder1;
 }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 unsigned long CalculaRPM( unsigned long num1 , unsigned long num2, unsigned long tiempo)
 {
  unsigned long pulsos;
  unsigned long RPM; 
  if(num1==-1) return -1;
  if(num2==-1) return -1;
  if(num2>num1) pulsos=num2-num1;
  else  pulsos=num1-num2;
  RPM=(pulsos*1000/tiempo)*60/(1800*4*8.3);
  RPM=RPM*FACTOR_RPM/100;
  //RPMs=RPMs*FACTOR_RPM; // FACTOR /(1800*2*8.3)*0.1 seg Revoluciones_Rueda*min
  //RPMs=RPMs*FACTOR_RPM; // FACTOR /(1800*2*8.3)*1/60 Revoluciones_Rueda*min

  return RPM;
 }

 void CalculaDistancia(unsigned long rpm)
 {
  static unsigned long oldrpm=0;
  float dist;
  if(rpm!=-1)
  {

    if(oldrpm!=rpm) oldrpm=rpm;

 
  }

    if(MovimientoExplora==0x01) distancef=distancef+((oldrpm*FACTOR_Distancia*RADIORUEDAM*6.2832)/(60*100)); // 2*PI=6.2832
    if(MovimientoExplora==0x02)  distancef=distancef-((oldrpm*FACTOR_Distancia*RADIORUEDAM*6.2832)/(60*100));

    
//    if(rpm!=-1) distancef=rpm*1.0;  // SOLO PARA PREUBAS
 }

bool EsperaCabecera(void)
{
  byte midato;
  byte conta;
  conta=0; 
  delay(3);
  while(Serial1.available())
  {

    midato=Serial1.read();
   
    switch (midato)
    {
      case 'X':
        conta=1;
        break;
      case 'P':
        if(conta==1) conta=2;
        break;
      default:
        conta=0;
    }
    if (conta==2) break;
  } // fin del while

  if (conta==2) 
   {
 
    receive_buffer[0]='X';
    receive_buffer[1]='P';
    return true;
   }
 
  else return false;   
}
 
int Recibedatos(byte numdatos)
{
byte i;
i=2;
 while(Serial1.available())
 {
  receive_buffer[i]=Serial1.read();
  i++;
  if(i==numdatos)  break;
 }
   
   if(i!=numdatos) return 0;
  else 
  {
     //Serial.write('A');
     return i;
  }
 
}

int RecibetramaBeagle(byte numdatos)
{
unsigned long timeold,timenew;

  timeold=millis();
  timenew=timeold;
  while((timenew-timeold)<TIMEOUTSERIALBEAGLE)
  {
 
   if(EsperaCabecera())
   {
     delay(20);
     return Recibedatos(numdatos);
   }
   timenew=millis();

  }
 
    contaerror++;
  return 0;
  
}

