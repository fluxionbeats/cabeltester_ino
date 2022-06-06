// передатчик
// C++ code
//
#include <Servo.h>
#include "nRF24L01.h"
#include "RF24.h"
RF24 radio(9, 10);
byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"}; //возможные номера труб
void setup()
{
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  Serial.begin(9600); //открываем порт для связи с ПК
  radio.begin(); //активировать модуль
  radio.setAutoAck(1);         //режим подтверждения приёма, 1 вкл 0 выкл
  radio.setRetries(0, 15);    //(время между попыткой достучаться, число попыток)
  radio.enableAckPayload();    //разрешить отсылку данных в ответ на входящий сигнал
  radio.setPayloadSize(32);     //размер пакета, в байтах

  radio.openReadingPipe(1, address[0]);     //хотим слушать трубу 0
  radio.setChannel(0x60);  //выбираем канал (в котором нет шумов!)

  radio.setPALevel (RF24_PA_MAX); //уровень мощности передатчика. На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate (RF24_250KBPS); //скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
  //должна быть одинакова на приёмнике и передатчике!
  //при самой низкой скорости имеем самую высокую чувствительность и дальность!!

  radio.powerUp(); //начать работу
  radio.startListening();  //начинаем слушать эфир, мы приёмный модуль
}

void loop(void)
{
  byte pipeNo;
  int pin_num;
  bool cancel;
  while ( radio.available(&pipeNo)) {  // слушаем эфир со всех труб
    radio.read(&pin_num, sizeof(pin_num) );  // чиатем входящий сигнал
    Serial.print("pin_num: "); Serial.println(pin_num);
    digitalWrite(pin_num, HIGH);
    delay(100);
    Serial.print("Signal pin: "); Serial.println(pin_num);
  }
  while ( radio.available(&pipeNo)) {  // слушаем эфир со всех труб
    radio.read(&cancel, sizeof(cancel) );  // чиатем входящий сигнал
    if (cancel) {
      Serial.print("Off: "); Serial.println(pin_num);
      digitalWrite(pin_num, LOW);
    }
  }
}
