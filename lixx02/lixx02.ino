/* Readme
Tags
{
  #in development (#indev)　作成中
  #unverified (#unveri)　作ったけど動作未検証
  #improve later (#imlat)　後で改良
  #develop later (#devlat)　後で実装
  #dellat : #delete later 後で消す
}
*/ 

/* #devlat
#if DEBUG
  const int DEBUG_MODE;
#endif

Course and Machine Specification
  Course:
  .Black line on White board
  Machine:
  .Two photoReflectors which get H level with black, get L leveValiables with white
 */
 
  // ↓↓↓↓Declaring Variables↓↓↓↓ //
  
//Global variables

// flags
unsigned int flag_onBrake = 1;
  
// Pin config
const int Pin_leftMotorIn1 =  32;
const int Pin_leftMotorIn2 =  33;
const int Pin_rightMotorIn1 = 25;
const int Pin_rightMotorIn2 = 26;

const int Pin_leftSensor = 36;
const int Pin_rightSensor = 39;

// ledc channel config (16 channels)
const int ledcCh_leftMotorIn1 = 0; //for leftMotor   in1
const int ledcCh_leftMotorIn2 = 1; //for lefttMotor  in2
const int ledcCh_rightMotorIn1 = 2; //for rightMotor  in1
const int ledcCh_rightMotorIn2 = 3; //for rightMotor  in2

// ledc precission (1~16bit configulable)
const int ledcTimerBit            = 13;   //timer counter resolution for pwm (finxed for hardware reason)
const int MAX_VALUE_DECIMAL_13BIT = 8191;
const int ledcTimerBitDecimal = 255;      //本当は13bitをArduinoライクにするため関数内で8bitにしているので名前が適切でないかも

// ledc base frequeny (1~80M?)
const int ledcBaseFrequency = 10000;

//adc 
const int VOLT = 3.3;
const int ANALOG_MAX = 4095;  


  // ↑↑↑↑Declaring Variables end↑↑↑↑ //

  // ↓↓↓↓Defining Functions↓↓↓↓ //

// Arduino like analogWrite
// value has to be between 0 and valueMax

  //esp32のpwmモジュールは13bitだが、ArduinoのAnalog Writeの書式に合わせて8bitの値を引数に入れる仕様にしている
  //いっそのこと1~100にしても良い
void ledcAnalogWrite(uint8_t channel, uint32_t value, uint32_t valueMax = 255)
{
  // calculate duty, 8191 from 2 ^ 13 - 1
  uint32_t duty = (MAX_VALUE_DECIMAL_13BIT / valueMax) * min(value, valueMax);
  // write duty to LEDC
  ledcWrite(channel, duty);
}

  // ↑↑↑↑Defining Functions end↑↑↑↑ //

  // ↓↓↓↓Classes Definition↓↓↓↓ //

//mode L => IN/IN mode (decided by hardware)
//in1,in2 => motor function
 //0,0 => Coast
 //0,1 => Reverse
 //1,0 => Forward
 //1,1 => Brake
class DRV8835
{
  public:
    void forward(int duty)
    {
      ledcAnalogWrite(ledcCh_leftMotorIn1, 0);
      ledcAnalogWrite(ledcCh_leftMotorIn2, duty);
      ledcAnalogWrite(ledcCh_rightMotorIn1, 0);
      ledcAnalogWrite(ledcCh_rightMotorIn2, duty);
    }  
    void forward(int dutyL, int dutyR)  //turnAvalable
    {
      ledcAnalogWrite(ledcCh_leftMotorIn1, dutyL);
      ledcAnalogWrite(ledcCh_leftMotorIn2, 0);
      ledcAnalogWrite(ledcCh_rightMotorIn1, dutyR);
      ledcAnalogWrite(ledcCh_rightMotorIn2, 0);
    }
    void forward(int duty, unsigned int flag_onBrake)  //#in development #(inDev) //flag_onBrake: onBrakeにオーバーロードするためだけの引数
    {
      ledcAnalogWrite(ledcCh_leftMotorIn1, 255-duty); //#imlat 関数内で計算させたくない（CPUパワー都合）
      ledcAnalogWrite(ledcCh_leftMotorIn2, 255);
      ledcAnalogWrite(ledcCh_rightMotorIn1, 255-duty);
      ledcAnalogWrite(ledcCh_rightMotorIn2, 255);
    }
/*
    void forward(int dutyL, int dutyR, unsigned int flag_onBrake) //turnAvalable and onBrake
    {
      ledcAnalogWrite(ledcCh_leftMotorIn1, 255-dutyL);
      ledcAnalogWrite(ledcCh_leftMotorIn2, 255);
      ledcAnalogWrite(ledcCh_rightMotorIn1, 255-dutyR);
      ledcAnalogWrite(ledcCh_rightMotorIn2, 255); 
    }
*/
    void forward(int dutyL, int dutyR, unsigned int flag_onBrake) //turnAvalable and onBrake //#imlat tempマイナスも受け付けるようにした
    {
      if(dutyL > 0)
      {
        ledcAnalogWrite(ledcCh_leftMotorIn1, 255-dutyL);
        ledcAnalogWrite(ledcCh_leftMotorIn2, 255);
      }
      else if(dutyL < 0)
      {
        ledcAnalogWrite(ledcCh_leftMotorIn1, 255);
        ledcAnalogWrite(ledcCh_leftMotorIn2, 255+dutyL);  //duty比がマイナス値なのでプラスする //#imlat アンダーフロー対策
      }
      if(dutyR > 0)
      {
        ledcAnalogWrite(ledcCh_rightMotorIn1, 255-dutyR);
        ledcAnalogWrite(ledcCh_rightMotorIn2, 255); 
      }
      else if(dutyR < 0)
      {
        ledcAnalogWrite(ledcCh_rightMotorIn1, 255);
        ledcAnalogWrite(ledcCh_rightMotorIn2, 255+dutyR);        
      }
    }
    
    void backward(int duty)  //onFree
    {
      ledcAnalogWrite(ledcCh_leftMotorIn1, duty);
      ledcAnalogWrite(ledcCh_leftMotorIn2, 0);
      ledcAnalogWrite(ledcCh_rightMotorIn1, duty);
      ledcAnalogWrite(ledcCh_rightMotorIn2, 0);
    }
     void backward(int dutyL, int dutyR)
    {
      ledcAnalogWrite(ledcCh_leftMotorIn1, 0);
      ledcAnalogWrite(ledcCh_leftMotorIn2, dutyL);
      ledcAnalogWrite(ledcCh_rightMotorIn1, 0);
      ledcAnalogWrite(ledcCh_rightMotorIn2, dutyR);
    }   
    void backward(int duty, unsigned int flag_onBrake)  //#in development #(inDev) //flag_onBrake: onBrakeにオーバーロードするためだけの引数
    {
      ledcAnalogWrite(ledcCh_leftMotorIn1, 255); //#imlat 関数内で計算させたくない（CPUパワー都合）
      ledcAnalogWrite(ledcCh_leftMotorIn2, 255-duty);
      ledcAnalogWrite(ledcCh_rightMotorIn1, 255);
      ledcAnalogWrite(ledcCh_rightMotorIn2, 255-duty);
    }
    void backward(int dutyL, int dutyR, unsigned int flag_onBrake)
    {
      ledcAnalogWrite(ledcCh_leftMotorIn1, 255);
      ledcAnalogWrite(ledcCh_leftMotorIn2, 255-dutyL); //#imlat
      ledcAnalogWrite(ledcCh_rightMotorIn1, 255);
      ledcAnalogWrite(ledcCh_rightMotorIn2, 255-dutyR);
    }

    void rightSpin(int duty)
    {
      ledcAnalogWrite(ledcCh_leftMotorIn1, duty);
      ledcAnalogWrite(ledcCh_leftMotorIn2, 0);
      ledcAnalogWrite(ledcCh_rightMotorIn1, 0);
      ledcAnalogWrite(ledcCh_rightMotorIn2, duty);
    }
    void leftSpin(int duty)
    {
      ledcAnalogWrite(ledcCh_leftMotorIn1, 0);
      ledcAnalogWrite(ledcCh_leftMotorIn2, duty);
      ledcAnalogWrite(ledcCh_rightMotorIn1, duty);
      ledcAnalogWrite(ledcCh_rightMotorIn2, 0);
    }
    void coast()  //coast:惰性走行
    {
      ledcAnalogWrite(ledcCh_leftMotorIn1, 0);
      ledcAnalogWrite(ledcCh_leftMotorIn2, 0);
      ledcAnalogWrite(ledcCh_rightMotorIn1, 0);
      ledcAnalogWrite(ledcCh_rightMotorIn2, 0);
    }
    void brake()
    {
      ledcAnalogWrite(ledcCh_leftMotorIn1, 255);
      ledcAnalogWrite(ledcCh_leftMotorIn2, 255);
      ledcAnalogWrite(ledcCh_rightMotorIn1, 255);
      ledcAnalogWrite(ledcCh_rightMotorIn2, 255);
    }
    void brake(int duty) //#unveri 周期が揃っているならワークするはず
    {
      ledcAnalogWrite(ledcCh_leftMotorIn1, duty);
      ledcAnalogWrite(ledcCh_leftMotorIn2, duty);
      ledcAnalogWrite(ledcCh_rightMotorIn1, duty);
      ledcAnalogWrite(ledcCh_rightMotorIn2, duty);      
    }
};

  // ↑↑↑↑Classes Definition end↑↑↑↑ //


void setup()
{
  Serial.begin(115200);
  
  ledcSetup(ledcCh_leftMotorIn1, ledcBaseFrequency, ledcTimerBit); 
  ledcAttachPin(Pin_leftMotorIn1, ledcCh_leftMotorIn1);  //gpio32 => ch0

  ledcSetup(ledcCh_leftMotorIn2, ledcBaseFrequency, ledcTimerBit); 
  ledcAttachPin(Pin_leftMotorIn2, ledcCh_leftMotorIn2); //gpio33 => ch1

  ledcSetup(ledcCh_rightMotorIn1, ledcBaseFrequency, ledcTimerBit); 
  ledcAttachPin(Pin_rightMotorIn1, ledcCh_rightMotorIn1);  //gpio25 => ch2

  ledcSetup(ledcCh_rightMotorIn2, ledcBaseFrequency, ledcTimerBit); 
  ledcAttachPin(Pin_rightMotorIn2, ledcCh_rightMotorIn2); //gpio26 => ch3

//LRモータの正常動作確認
  DRV8835 drv8835;
  int testTime =  300;
  int testDuty =  127;
  int dutyL =     150;
  int dutyR =     150;

//onBrakeTest with turn
  drv8835.forward(dutyL,dutyR, flag_onBrake);
  delay(testTime);
  drv8835.brake();
  delay(testTime);

}

void loop()
{
  const int sensorMinL = 336;   //LeftMotor Minvalue
  const int sensorMaxL = 3300;  //LeftMotor Maxvalue
  const int sensorMinR = 1990;  //RightMotor MinValue
  const int sensorMaxR = 3950;  //RightMotor Maxvalue
  const int absoluteValueRangeL = sensorMaxL - sensorMinL; //Absolute sensorL valueRange
  const int absoluteValueRangeR = sensorMaxR - sensorMinR; //Absolute sensorR valueRange
  int     valueL = 0;
  int     valueR = 0;
  double  sensorRateL = 0;
  double  sensorRateR = 0;
  int     analogValueL = 0;
  int     analogValueR = 0;
  int absoluteValueL = 0;
  int absoluteValueR = 0;

  analogValueL = analogRead(Pin_leftSensor);  //get sensorL value via ADC pin
  analogValueR = analogRead(Pin_rightSensor); //get sensorR value via ADC pin

  absoluteValueL = analogValueL - sensorMinL; //calibrate sensorValue from 0 
  absoluteValueR = analogValueR - sensorMinR; //calibrate sensorValue from 0
  
  if(absoluteValueL < 0) absoluteValueL = 0;  //Not to correct the sensor value less than 0
  if(absoluteValueR < 0) absoluteValueR = 0;  //Not to correct the sensor value less than 0

  Serial.print("SensorValue_L: ");
  Serial.println(analogValueL);
  Serial.print("SensorValue_R: ");
  Serial.println(analogValueR);
  Serial.println();
  
  sensorRateL = (double)absoluteValueL/(double)absoluteValueRangeL;  //Change the leftSensorValue into percentage
  sensorRateR = (double)absoluteValueR/(double)absoluteValueRangeR;  //Change the rightSensorValue into percentage
  
  if(sensorRateL > 1) sensorRateL = 1.0;
  if(sensorRateR > 1) sensorRateR = 1.0;

  Serial.print("sensorRateL: ");
  Serial.println(sensorRateL);
  Serial.print("sensorRateR: ");
  Serial.println(sensorRateR);
  Serial.println();
  Serial.println();

//Line Trace Program start  //gain 2, dutyBase 170(160E), gain 5(5.5), dutyBase 160(last cornar stop..)
  const int dutyBase =200;  //定義するベース速度 （値の範囲：0 ~ 255）
  const double pGain = 1;  //比例ゲイン       （値の範囲：0.0 ~ 1.0）
  
  DRV8835 drv8835;  //MotorDriver instance化
  int dutyL;  //LeftMotorDuty   (valueRange: xxx ~ xxx)
  int dutyR;  //RightMotorDuty  (valueRange: xxx ~ xxx)
  int defL;   //
  int defR;

  //↓Cast必要
/*
  defL =  (ledcTimerBitDecimal - dutyBase) * sensorRateL;  //
  defR =  (ledcTimerBitDecimal - dutyBase) * sensorRateR;  //sensorRate(0~1)
*/

  //コースとセンサのかかり方を図にして条件分岐をすべき。常に左右の値のdefを参照していたらおかしくなる。
  defL =  dutyBase * sensorRateL;  //
  defR =  dutyBase * sensorRateR;  //sensorRate(0~1)


 //dutyLとdefRで左右互い違い
  dutyL = dutyBase + (defR * pGain);  //#imlat dutyLはクラスの中に入れたほうが良い
  dutyR = dutyBase + (defL * pGain);  //
  
  if(dutyL > ledcTimerBitDecimal)               //計算結果が最大値を超えたら
  {
    dutyR = dutyR-(dutyL-ledcTimerBitDecimal);  //はみ出した値を逆のモータのバックとして引き継ぐ
    if(dutyR < 0) dutyR = 0;                    //0を下回ってしまったらマイナスにならないように0にする
    dutyL = ledcTimerBitDecimal;
  }
  if(dutyR > ledcTimerBitDecimal)               //計算結果が最大値を超えたら
  {
    dutyL = dutyL-(dutyR-ledcTimerBitDecimal);  //はみ出した値を逆のモータのバックとして引き継ぐ
    if(dutyL < 0) dutyL = 0;                    //0を下回ってしまったらマイナスにならないように0にする
    dutyR = ledcTimerBitDecimal; 
  }

//  dutyL = dutyBase - (defL * pGain);  //#imlat dutyLはクラスの中に入れたほうが良い
//  dutyR = dutyBase - (defR * pGain);  //
//  if(dutyL < 0) dutyL = 0;
//  if(dutyR < 0) dutyR = 0; 


//黒・黒でストレートだが、そこから外れ始めたタイミングを意図的に設定している　はず
  if(sensorRateL > 0.3 && sensorRateR > 0.3)  //#imlat 0.3が何のマジックナンバーかわからない
  {
    drv8835.forward(dutyBase/2, dutyBase/2, flag_onBrake);  //#imlat 
  }
  else 
  {
    drv8835.forward(dutyL, dutyR, flag_onBrake);  //#imlat
  }
}
