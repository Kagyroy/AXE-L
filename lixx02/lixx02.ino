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

//arduino nano, esp32, microbit, rpi FLAG
 */
 
  // ↓↓↓↓Declarations↓↓↓↓ //
  
//Global variables
  // flags
  unsigned int flag_onBrake = 1;
//Globaruvariables
  
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

// ledc precission (1~16bit)
const int ledcTimerBit = 13;
//int ledcTimerBitDecimal; //後で考える

// ledc base frequeny (1~80M?)
const int ledcBaseFrequency = 10000;

//adc 
const int VOLT = 3.3;
const int ANALOG_MAX = 4095;  
const int MAX_VALUE_DECIMAL_13BIT = 8191;

  // ↑↑↑↑Decralations end↑↑↑↑ //

  // ↓↓↓↓Functions↓↓↓↓ //

// Arduino like analogWrite
// value has to be between 0 and valueMax
void ledcAnalogWrite(uint8_t channel, uint32_t value, uint32_t valueMax = 255)
{
  // calculate duty, 8191 from 2 ^ 13 - 1
  uint32_t duty = (MAX_VALUE_DECIMAL_13BIT / valueMax) * min(value, valueMax);
  // write duty to LEDC
  ledcWrite(channel, duty);
}

  // ↑↑↑↑Functions end↑↑↑↑ //

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

  drv8835.forward((int)dutyL/2,(int)dutyR/2, flag_onBrake);
  delay(testTime);
  drv8835.brake();
  delay(testTime);

  drv8835.forward((int)dutyL,(int)dutyR/2, flag_onBrake);
  delay(testTime);
  drv8835.brake();
  delay(testTime);

  drv8835.forward((int)dutyL/2,(int)dutyR, flag_onBrake);
  delay(testTime);
  drv8835.brake();
  delay(testTime); 

  drv8835.forward((int)dutyL,0, flag_onBrake);
  delay(testTime);
  drv8835.brake();
  delay(testTime);

  drv8835.forward(0,(int)dutyR, flag_onBrake);
  delay(testTime);
  drv8835.brake();
  delay(testTime);
}

void loop()
{
  const int sensorMinL = 630;   //
  const int sensorMaxL = 3300;  //3150
  const int sensorMinR = 2150;  //
  const int sensorMaxR = 3950;  //3800

  int sensorRangeL = sensorMaxL - sensorMinL;
  int sensorRangeR = sensorMaxR - sensorMinR;
  
  int analogValueL = analogRead(Pin_leftSensor);  //adcから値を取得
  int analogValueR = analogRead(Pin_rightSensor); //adcから値を取得
  int valueL = analogValueL - sensorMinL; //（ほぼ）0からの値に補正
  int valueR = analogValueR - sensorMinR; //（ほぼ）0からの値に補正
  
  if(valueL < 0) valueL = 0;
  if(valueR < 0) valueR = 0;

  Serial.print("leftSensor: ");
  Serial.println(analogValueL);
  Serial.print("rightSensor: ");
  Serial.println(analogValueR);
  Serial.println();

  double sensorRateL = (double)valueL/(double)sensorRangeL;
  double sensorRateR = (double)valueR/(double)sensorRangeR;
  if(sensorRateL > 1) sensorRateL = 1.0;
  if(sensorRateR > 1) sensorRateR = 1.0;


  Serial.print("sensorRateL: ");
  Serial.println(sensorRateL);
  Serial.print("sensorRateR: ");
  Serial.println(sensorRateR);
  Serial.println();

//Line Trace Program start  //gain 2, dutyBase 170(160E), gain 5(5.5), dutyBase 160(last cornar stop..)
  const int dutyBase =16;  //定義するベース速度 
  const double pGain = 6;  //比例ゲイン proportional gain
  
  DRV8835 drv8835;
  int dutyL;
  int dutyR;
  int defL;
  int defR;
  
  defL =  (255 - dutyBase) * sensorRateL;  //255はconstで定数化 //duty最大値-dutyBase * sensorRate
  defR =  (255 - dutyBase) * sensorRateR;  //sensorRate(0~1)

  dutyL = dutyBase + (defR * pGain);  //#imlat dutyLはクラスの中に入れたほうが良い
  dutyR = dutyBase + (defL * pGain);  //
  if(dutyL > 255)
  {
    dutyR = dutyR-(dutyL-255);  //はみ出した値を逆のモータのバックとして引き継ぐ
    dutyL = 255;
  }
  if(dutyR > 255)
  {
    dutyL = dutyL-(dutyR-255);
    dutyR = 255; 
  }

//  dutyL = dutyBase - (defL * pGain);  //#imlat dutyLはクラスの中に入れたほうが良い
//  dutyR = dutyBase - (defR * pGain);  //
//  if(dutyL < 0) dutyL = 0;
//  if(dutyR < 0) dutyR = 0; 

  if(sensorRateL > 0.3 && sensorRateR > 0.3)
  {
    drv8835.forward(dutyBase/2, dutyBase/2, flag_onBrake);  //#imlat 
  }
  else 
  {
    drv8835.forward(dutyL, dutyR, flag_onBrake);  //#imlat
  }
}
