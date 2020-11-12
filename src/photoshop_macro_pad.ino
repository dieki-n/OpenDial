#include "BleConnectionStatus.h"
#include "BleKeyboard.h"
#include "KeyboardOutputCallbacks.h"

#include "matrix.h"
#include "encoder.h"

#include <EEPROM.h>

#define EEPROM_SIZE 13
#define EEPROM_MAGIC_BYTE 0x42 //Magic byte; if we've stored a keymapping before, this will be the first byte in the EEPROM. Otherwise use the default.
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

#define DIAL_ROTATION_DIRECTION -1 //Depends on how encoder is wired
#define DIAL_LONGPRESS_DELAY 400 //Milliseconds before we trigger the vibration on longpress
const int PIN_LED = 5;
const int PIN_PAIR = 17;
const int PIN_VIBRATOR = 13;

bool scan_flag = false;
int dial_pos = 0;
float vibe_strength = 100;
int vibe_until = 0;
int longpress_trigger = 0;

//        (from front to back) r1   r2   r3   
uint8_t key_mapping[12] =   {'s', 'x', 'c',  //purple
                             'd', 'd', 'f',  //blue
                             'g', 'z', 'b',  //orange
                             'k', 'z', KEY_DIAL}; //red

uint8_t ctrl_mapping[12] =   {1, 0, 0,  
                              0, 0, 0,
                              0, 1, 0,
                              0, 1, 0};

uint8_t alt_mapping[12] =    {0, 0, 0,  
                              0, 0, 0,
                              0, 0, 0,
                              0, 0, 0};

uint8_t shift_mapping[12] =  {0, 0, 0,  
                              1, 0, 0,
                              0, 0, 0,
                              0, 1, 0};

                               
KeyboardMatrix matrix_handler;
RotaryEncoder encoder_handler;


BleKeyboard bleKeyboard("Bluetooth Macro Pad", "Victor Noordhoek", 100);
BLEServer* bleKeyboardServer;
BLEService* bleKeymappingService;
BLECharacteristic* bleKeymapping;



void IRAM_ATTR keyboard_isr(){
  scan_flag = true;
}


class keymapCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();

      if (value.length() == 12) {
        Serial.print("New value: ");
        for (int i = 0; i < 12; i++){
          Serial.print(value[i]);
          key_mapping[i] = value[i];
        }
        Serial.println();
        saveKeymap();
      }
    }
};

void loadKeymap(){
  for (int i = 0; i < 12; i++){
      key_mapping[i] = EEPROM.read(i + 1);
      ctrl_mapping[i] = EEPROM.read((i+ 12) + 1);
      alt_mapping[i] = EEPROM.read((i + 24) + 1);
      shift_mapping[i] = EEPROM.read((i + 36) + 1);
  }
}
void saveKeymap(){
  EEPROM.write(0, EEPROM_MAGIC_BYTE);
    for (int i = 0; i < 12; i++){
      EEPROM.write(i + 1, key_mapping[i]);
      EEPROM.write(i + 12 + 1, ctrl_mapping[i]);
      EEPROM.write(i + 24 + 1, alt_mapping[i]);
      EEPROM.write(i + 25 + 1, shift_mapping[i]);
    }
}
void vibratorOn(){
  digitalWrite(PIN_VIBRATOR, HIGH);
}

void vibratorOff(){
  digitalWrite(PIN_VIBRATOR, LOW);
}
void vibratorCheck(){
  if (vibe_until > millis()){
    digitalWrite(PIN_VIBRATOR, HIGH);
  } else {
    digitalWrite(PIN_VIBRATOR, LOW);
  }
}
void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(115200);
  
  EEPROM.begin(EEPROM_SIZE);
  //Check if we've saved a keymap to EEPROM before; if so load it, if not, use the default. 
  if (EEPROM.read(0) == EEPROM_MAGIC_BYTE){
    //loadKeymap();
  }
  
  bleKeyboard.begin();
 
  bleKeymappingService = bleKeyboard.pServer->createService(SERVICE_UUID);
  bleKeymapping = bleKeymappingService->createCharacteristic(CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  bleKeymapping->setValue((uint8_t*)key_mapping, 12);
  bleKeymapping->setCallbacks(new keymapCallbacks());
  bleKeymappingService->start();
  
  matrix_handler.init();
  encoder_handler.init();
  pinMode(PIN_VIBRATOR, OUTPUT);
  digitalWrite(PIN_VIBRATOR, LOW);
  saveKeymap();
  //esp_sleep_enable_gpio_wakeup();
}

void loop() {
  vibratorCheck();
  int changes_buff[12] = {};
  matrix_handler.scan(changes_buff);
  for (int j = 0; j < 4; j++){
    for (int i = 0; i < 3; i++){
      if (changes_buff[j * 3 + i] == KEY_PRESS_EVENT){
        if (key_mapping[j*3 + i] == KEY_DIAL){
          bleKeyboard.pressDial();
          longpress_trigger = millis() + DIAL_LONGPRESS_DELAY;
        } else {
          
          if (ctrl_mapping[j*3+i]) bleKeyboard.press(KEY_LEFT_CTRL);
          if (alt_mapping[j*3+i]) bleKeyboard.press(KEY_LEFT_ALT);
          if (shift_mapping[j*3+i]) bleKeyboard.press(KEY_LEFT_SHIFT);
          bleKeyboard.press(key_mapping[j*3 + i]);
          
        }
      } else if (changes_buff[j * 3 + i] == KEY_UNPRESS_EVENT){
        if (key_mapping[j*3 + i] == KEY_DIAL){
          bleKeyboard.releaseDial();
          longpress_trigger = 0;
        } else {
          if (ctrl_mapping[j*3+i]) bleKeyboard.release(KEY_LEFT_CTRL);
          if (alt_mapping[j*3+i]) bleKeyboard.release(KEY_LEFT_ALT);
          if (shift_mapping[j*3+i]) bleKeyboard.release(KEY_LEFT_SHIFT);
          bleKeyboard.release(key_mapping[j*3 + i]);
        }
      }
      changes_buff[j * 3 + i] = 0;
    }
  }

  if (longpress_trigger > 0 && longpress_trigger < millis()){
    vibe_until = millis() + vibe_strength;
    longpress_trigger = 0;
  }
  int d = encoder_handler.getPosition();
  if (d != 0){
    
    bleKeyboard.dial_pos += d;
    if (bleKeyboard.dial_pos % bleKeyboard.dial_interval == 0){
      bleKeyboard.dial_pos = 0;
      bleKeyboard.rotate(d * DIAL_ROTATION_DIRECTION);
      if (bleKeyboard.dial_vibrate){
        vibe_until = millis() + vibe_strength;
        vibratorCheck();
      }
    }
  }
}
