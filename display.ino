// приемник
#include <LiquidCrystal_I2C.h>
#include "nRF24L01.h"
#include <Servo.h>
#include "RF24.h"

// initialize the library with the numbers of the interface pins
LiquidCrystal_I2C lcd(0x27, 16, 2);
RF24 radio(9, 10);
int pin_num = 3;
bool finished = false;
int pins[3][2];
byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"}; //возможные номера труб
void setup() {
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  pinMode(6, INPUT);
  pinMode(7, INPUT);
  pinMode(8, INPUT);

  Serial.begin(9600); //открываем порт для связи с ПК

  radio.begin(); //активировать модуль
  radio.setAutoAck(1);         //режим подтверждения приёма, 1 вкл 0 выкл
  radio.setRetries(0, 15);    //(время между попыткой достучаться, число попыток)
  radio.enableAckPayload();    //разрешить отсылку данных в ответ на входящий сигнал
  radio.setPayloadSize(32);     //размер пакета, в байтах

  radio.openWritingPipe(address[0]);   //мы - труба 0, открываем канал для передачи данных
  radio.setChannel(0x60);  //выбираем канал (в котором нет шумов!)

  radio.setPALevel (RF24_PA_MAX); //уровень мощности передатчика. На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate (RF24_250KBPS); //скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
  //должна быть одинакова на приёмнике и передатчике!
  //при самой низкой скорости имеем самую высокую чувствительность и дальность!!

  radio.powerUp(); //начать работу
  radio.stopListening();  //не слушаем радиоэфир, мы передатчик
}

void loop() {
  if (pin_num <= 8 && !finished) {
    Serial.print("pin_num: "); Serial.println(pin_num);
    //сигнал о готовности принять сигнал по проводам
    radio.write(&pin_num, sizeof(pin_num));
    delay(10000);
    for (int i = 3; i <= 8; i++) {
      int port = digitalRead(i);
      if (!port) {
        pins[(i - 3) / 2][(i - 3) % 2] = (pin_num - 1) / 2;
        Serial.print("Recieved signal: "); Serial.println(i);
        break;
      }
    }
    //манипуляции проделаны, заканчиваем принимать сигнал на порт
    bool cancel = true;
    radio.write(&cancel, sizeof(cancel));
    Serial.print("Off: "); Serial.println(pin_num);

    pin_num++;
    if (pin_num > 8) {
      finished = true;
      for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 2; j++) {
          Serial.print(pins[i][j]);
        }
      }
    }
  }
