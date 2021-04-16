/*
   Author: Sebastian G. Speleman
   Last update:
   Smart_kollektiv: Acsses.
*/

#include<CircusESP32Lib.h>

char ssid[] = "Telenor6619dyp";
char password[] = "josehome08";
char server[] = "www.circusofthings.com";
char token[] = "LED";
char User1_Key_1[] = "6442";
char User1_Key_2[] = "30385";
char User1_Key_3[] = "8691";
char User1_Key_4[] = "21957";

CircusESP32Lib circusESP32(server,ssid,password);


int button_pin0 = 0;
int button_pin1 = 1;
int button_pin2 = 2;
int button_pin3 = 3;
int button_pin4 = 4;
int button_pin5 = 5;
int button_pin6 = 6;
int button_pin7 = 7;
int button_pin8 = 8;
int button_pin9 = 9;

int button_pinEnter = 10;
int button_pinRemove = 11;

int user1_val_1;
int user1_val_2;
int user1_val_3;
int user1_val_4;

int key_nr ;
int code[4];
int access_code1[4] = {user1_val_1, user1_val_2, user1_val_3, user1_val_4} ;
int access_code2[4] = {1, 2, 3, 4} ;
int access_code3[4] = {1, 0, 0, 0} ;
int access_code4[4] = {4, 3, 2, 1} ;
int access_code5[4] = {1, 0, 0, 0} ;
int access_code6[4] = {1, 0, 0, 0} ;

int skjekk = 0;
int accept_the_code = 0;

void setup()
{
  Serial.begin(9600);
  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
  pinMode(11, INPUT_PULLUP);
  circusESP32.begin();

}

void loop()
{
  user1_val_1 = circusESP32.read(User1_Key_1,token);
  user1_val_2 = circusESP32.read(User1_Key_2,token);
  user1_val_3 = circusESP32.read(User1_Key_3,token);
  user1_val_4 = circusESP32.read(User1_Key_4,token);
  
  byte b0 = digitalRead(button_pin0);
  byte b1 = digitalRead(button_pin1);
  byte b2 = digitalRead(button_pin2);
  byte b3 = digitalRead(button_pin3);
  byte b4 = digitalRead(button_pin4);
  byte b5 = digitalRead(button_pin5);
  byte b6 = digitalRead(button_pin6);
  byte b7 = digitalRead(button_pin7);
  byte b8 = digitalRead(button_pin8);
  byte b9 = digitalRead(button_pin9);

  byte bE = digitalRead(button_pinEnter);
  byte bC = digitalRead(button_pinRemove);

  int Code_box[] = {b0, b1, b2, b3, b4, b5, b6, b7, b8, b9};
  for (int i = 0; i < 10; i++)
  {
    if (Code_box[i] == LOW && key_nr < 4)
    {
      code[key_nr] = i;
      key_nr++;
    }
  }

  if (bC == LOW)
  {
    code[0] = 0;
    code[1] = 0;
    code[2] = 0;
    code[3] = 0;
    key_nr = 0;
  }

  else if (bE == LOW)
  {
    Enter(access_code1);
    Enter(access_code2);
    Enter(access_code3);
    Enter(access_code4);
    Enter(access_code5);
    Enter(access_code6);
    if (accept_the_code >= 1)
    {
      Serial.println("LE GOW");
      accept_the_code = 0;
    }
    else
    {
      Serial.println("Nope");
    }
  }
  delay(400);
}
















void Enter(int user_code[])
{
  for (int i = 0; i < 4; i++)
  {
    if (code[i] == user_code[i])
    {
      skjekk++;
      if (skjekk == 4)
      {
        accept_the_code++;
        code[0] = 0;
        code[1] = 0;
        code[2] = 0;
        code[3] = 0;
        key_nr = 0;
        skjekk = 0;
      }
    }
    else
    {
      skjekk = 0;
      break;
    }
  }
}
