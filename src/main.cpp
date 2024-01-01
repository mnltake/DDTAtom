#include <Arduino.h>

#include <DDT_Motor_M15M06.h>
//#include <M5StickC.h>


int16_t Speed = 0;   // Speed of motor
uint8_t Acce = 0;    // Acceleration of motor
uint8_t Brake_P = 0; // Brake position of motor
uint8_t ID = 1;      // ID of Motor (default:1)
const int16_t SPEED_MAX = 330;
const int16_t SPEED_MIN = -330;
Receiver Receiv;
// M5Stackのモジュールによって対応するRX,TXのピン番号が違うためM5製品とRS485モジュールに対応させてください
auto motor_handler = MotorHandler(0, 1); // RX,TX

void setup()
{
  Serial.begin(115200);
  Serial.println("DDT-Motor RS485");

  motor_handler.Control_Motor(0, ID, Acce, Brake_P, &Receiv);
}


void loop()
{
  while (true)
  {
    Speed++;
    delay(50);
    motor_handler.Control_Motor(Speed, ID, Acce, Brake_P, &Receiv);
    delay(5);
    Serial.print(" Mode:");
    Serial.print(Receiv.BMode);
    Serial.print(" Speed:");
    Serial.println(Receiv.BSpeed);
    if (Speed > int16_t(SPEED_MAX))
    
    {
      delay(5000);
      break;
    }
  }

  while (true)
  {
    Speed--;
    delay(50);
    motor_handler.Control_Motor(Speed, ID, Acce, Brake_P, &Receiv);
    delay(5);
    Serial.print(" Mode:");
    Serial.print(Receiv.BMode);
    Serial.print(" Speed:");
    Serial.println(Receiv.BSpeed);
    if (Speed < SPEED_MIN)
    {
            delay(5000);
      break;
    }
  }
}