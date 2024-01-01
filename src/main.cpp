#include <DDT_Motor_M15M06.h>
#include <M5Atom.h>
#include <esp_now.h>
#include <WiFi.h>
#define CHANNEL 1
#define LED_PIN 27
#define NUM_LEDS 1

uint8_t Acce = 0;    // Acceleration of motor
uint8_t Brake_P = 0xff; // Brake position of motor
uint8_t ID = 1;      // ID of Motor (default:1)
uint8_t Mode = 0x03; //Angle loop
uint8_t reset = 0;
Receiver Receiv;
CRGB leds[NUM_LEDS];
// M5Stackのモジュールによって対応するRX,TXのピン番号が違うためM5製品とRS485モジュールに対応させてください
auto motor_handler = MotorHandler(32, 26); // RX=21,TX=25
void setAngle(uint16_t Angle);



// Init ESP Now with fallback
void InitESPNow() {
  WiFi.disconnect();
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESPNow Init Success");
  }
  else {
    Serial.println("ESPNow Init Failed");
    // Retry InitESPNow, add a counte and then restart?
    // InitESPNow();
    // or Simply Restart
    ESP.restart();
  }
}

// config AP SSID
void configDeviceAP() {
  const char *SSID = "Slave_1";
  bool result = WiFi.softAP(SSID, "Slave_1_Password", CHANNEL, 0);
  if (!result) {
    Serial.println("AP Config failed.");
  } else {
    Serial.println("AP Config Success. Broadcasting with AP: " + String(SSID));
    Serial.print("AP CHANNEL "); Serial.println(WiFi.channel());
  }
}
// callback when data is recv from Master
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("Last Packet Recv from: "); Serial.println(macStr);
  Serial.print("Last Packet Recv Data: "); Serial.printf("%02x %02x %02x\n",data[0],data[1],data[2]);
  uint16_t newposision = data[0] | data[1]<<8;
  reset = data[2];
  // Serial.printf("newposision: %d\n",newposision);
  uint16_t angle =  (newposision%64)*512;
  // Serial.println(angle);
  setAngle(angle);

}

void setup()
{
  M5.begin(true, false, false); //Serial:true I2C:false
  // Serial.begin(115200);
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS); 
  Serial.println("DDT-Motor RS485");
  // pinMode(39 ,INPUT_PULLUP);
  // delay(100);
  // // motor_handler.Control_Motor(0, ID, Acce, Brake_P, &Receiv); //モータ停止
  // delay(100);
  motor_handler.Set_MotorMode(Mode, ID); //モード変更
  delay(100);
    //Set device in AP mode to begin with
  WiFi.mode(WIFI_AP);
  // configure device AP mode
  configDeviceAP();
  // This is the mac address of the Slave in AP Mode
  Serial.print("AP MAC: "); Serial.println(WiFi.softAPmacAddress());
  // Init ESPNow with a fallback logic
  InitESPNow();
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info.
  esp_now_register_recv_cb(OnDataRecv);
}

void loop()
{

  while (Receiv.BMode != Mode){
    M5.update();
    leds[0] = CRGB::Red;
    FastLED.show();
    if (M5.Btn.isPressed() || reset ){
      ESP.restart();
    }
    
  }
  leds[0] = CRGB::Black;
  FastLED.show();
}

void setAngle(uint16_t Angle)
{
  motor_handler.Control_Motor(Angle, ID, Acce, Brake_P, &Receiv);
  Serial.print("Mode:");
  Serial.print(Receiv.BMode);
  Serial.print(" Position:");
  Serial.print(Receiv.Position);
  Serial.print(" inputAngle:");
  Serial.println(Angle);

}