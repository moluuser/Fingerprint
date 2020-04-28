/*  项目名：指纹门禁
 *  开发时间：2018年10月1日
 *  
 *  功能实现：1.指纹门禁
 *            2.非法闯入报警
 *            
 *  待开发  ：1.机械部分；；预留tower()函数
 *            2.NFC门禁
 *            3.密码输入
 *            4.人体传感器实现节电
 *            
 *  注意：1.该代码仅实现电子控制部分，机械部分预留
 *        2.该项目仅为实验性质！
 *        
 *  关于作者：moluuser@gmail.com  //部分代码参考库文件案例
 *  版本：  alpha
*/

#include <LiquidCrystal.h>  //1602LCD
#include <DYE_Fingerprint.h>  //指纹模块
// pin #2 is IN from sensor (GREEN wire)
// pin #3 is OUT from arduino  (WHITE wire)
#include <SoftwareSerial.h>
#include <Servo.h>
 
#define TOWER 7  //舵机引脚7
#define BUZZER 8  //蜂鸣器引脚1
#define BODY 6  //人体传感器引脚6

Servo myservo;
SoftwareSerial mySerial(2, 3);
uint8_t id;
DYE_Fingerprint finger = DYE_Fingerprint(&mySerial);


const int rs = 4, en = 5, d4 = 10, d5 = 11, d6 = 12, d7 = 13;  //1602引脚
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);  //1602
int body_dis; //人体传感器参数

//错误识别用户反应
void is_false()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Warning!");
  lcd.setCursor(0, 1);
  lcd.print("Illegal Enter  ");
  digitalWrite(BUZZER,HIGH);  //蜂鸣器报警
  delay(2500);
  }

//正确识别用户反应
void is_true()
{
  tower();  //开门操作
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Welcome Home!");
  lcd.setCursor(0, 1);
  lcd.print("State:Door OPEN ");  //缺省关闭状态
  digitalWrite(BUZZER,LOW);  //缺省蜂鸣器为静音
  delay(2500);
  }

//待机操作
void stand()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("NO:402");
  lcd.setCursor(0, 1);
  lcd.print("Martial Law");  //缺省关闭状态
  digitalWrite(BUZZER,LOW);  //缺省蜂鸣器为静音
  }
  
void setup() {
    Serial.begin(9600);
    finger.begin(57600);
    myservo.attach(TOWER);
    pinMode(BUZZER,OUTPUT); 
    pinMode(BODY, INPUT);  
    digitalWrite(BUZZER,LOW);  //缺省蜂鸣器为静音
    lcd.begin(16, 2);  //1602
    stand();
}

void loop() {
  body_dis = digitalRead(BODY);
  Serial.println(body_dis);
  digitalWrite(BUZZER,LOW);  //缺省蜂鸣器为静音
  if (body_dis != 0)  //判断人体传感器
  {
    delay(3000);
    if (getFingerprintIDez() == -1)  //非法指纹
      is_false();
    else
      is_true();
    }
  else
  {
    stand();  //待机操作
    delay(2500);
    }
}

//预留机械部分函数
void tower() {
  myservo.write(0);
  delay(1000);
  myservo.write(80);
  delay(1000);
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;
  
  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID; 
}
