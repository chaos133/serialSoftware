#include <SoftwareSerial.h>

#define BAUDRATE                4800

#define WEIGHT_MACHINE_RX_PIN   2
#define WEIGHT_MACHINE_TX_PIN   3
#define WEIGHT_MACHINE_ADRESS   UNDEFINED
#define GET_VALUE               UNDEFINED

#define PUMP_RX_PIN             2
#define PUMP_TX_PIN             3
#define PUMP_ADRESS             UNDEFINED
#define ACTION                  UNDEFINED

#define START_BIT               0x10
#define STOP_BIT                0x20

//Shenchen registers
#define PUMP_HEAD               1000  //Relative datas refer to Chart 1
#define TUBING_SIZE             1001  //Relative datas refer to Chart 1
#define MOTOR_SPEED             1002  //0.1-600rpm
#define FLOW_RATE               1004  //0.1-99999 mL 
#define BACK_SUCTION_ANGLE      1007  //0-360°
#define START_STOP_CONTROLE     1008  //1:Start 0:Stop
#define DIRECTION_CONTROLE      1009  //1:Anticlockwise 0:Clockwise
#define FULL_SPEAD_RUNNING      1010  //1:Start full speed 0:Stop full speed
#define SET_FLOW_VOLUME         1015  //0-99999 mL
#define WORKING_TIME            1018  //0.1-9999（s）
#define WORKING_MODE            1020  //0: Transferring 1:Dispensing
#define PAUSE_TIME              1021  //0.1-9999（s）
#define COPY_NUMBER             1023  //0-9999 times. 0 means infinite

// Set up a new SoftwareSerial object
SoftwareSerial wmSerial(WEIGHT_MACHINE_RX_PIN, WEIGHT_MACHINE_TX_PIN);
SoftwareSerial pumpSerial(PUMP_RX_PIN, PUMP_TX_PIN);

byte MSG_IN[4];
byte MSG_OUT[4] = {START_BIT, 0, 0, STOP_BIT};  // шаблон сообщения: 0 - начальный, 1 - адрес устройства, 2 - команда, 3 - конечный

//-------------------------------------------------------------------------------------------

//
void post() {
  static unsigned long timerT;
  if (timerT > millis() - 100) return;      // отправляем последовательно каждые 100 мс
  for (byte i = 0; i < 4; i++) {            // отправляем 4 байта сообщения
    kakoytoSerial.write(MSG_OUT[i]);
  }
  delay(5);                                 // ожидаем пока уйдет команда до того, как переключить отправку на прием  
  timerT = millis();
}

//-------------------------------------------------------------------------------------------

//команда активации насоса с данным количеством шагов
void pumpAction(double pumpValue){
  MSG_OUT[1] = PUMP_ADRESS;
  MSG_OUT[2] = ACTION;
  for (byte i = 0; i < 4; i++) {   // передаем
    wmSerial.write(MSG_OUT[i]);
  }
}

//функция получения веса, намеренного весами
double getWmWeight(){
  //отправить команду весам с запросом веса
  MSG_OUT[1] = WEIGHT_MACHINE_ADRESS;
  MSG_OUT[2] = GET_VALUE;
  for (byte i = 0; i < 4; i++) {   // передаем
    wmSerial.write(MSG_OUT[i]);
  }
  //может пригодиться записывать полученный ответ, но скорее всего придётся разбирать посылку
  int value = 0;
  if (wmSerial.available()) {
    //    delay(1);
    for (byte i = 0; i < 3; i++) MSG_IN[i] = MSG_IN[i + 1];
    MSG_IN[3] = Serial.read();
    if (MSG_IN[0] == START_BIT &&  MSG_IN[3] == STOP_BIT) { // пришло полное сообщение
      value += MSG_IN[2]; //записываем принятое значение
      }
    }
  }
  return value;
}

//-------------------------------------------------------------------------------------------

//Передавать в функцию обЪём, по которому нужно померить, и количество повторений для калибровки
double calibrationStart(double pumpValue, int repeat){
  //обнулить погрешнотсь
  fault = 0;
  
  //процесс калибровки
  for(int i = 0; i < repeat; i++){
    //налить заданный объём
    pumpAction(pumpValue);
    while(wmSerial.isListening()) wmSerial.read();
    //суммируем значения, снятые с весов, чтобы потом высчитать ошибку
    fault += getWmWeight();
  }
  return fault/(double)repeat-pumpValue;
}

//-------------------------------------------------------------------------------------------

//Настройка портов
void setup() {
  double fault = 0;
  double PumpValue = 0;

  // Define pin modes for TX and RX
  pinMode(WEIGHT_MACHINE_RX_PIN, INPUT);
  pinMode(WEIGHT_MACHINE_TX_PIN, OUTPUT);

  pinMode(PUMP_RX_PIN, INPUT);
  pinMode(PUMP_TX_PIN, OUTPUT);

  // Set the baud rate for the SoftwareSerial object
  wmSerial.begin(BAUDRATE);
  pumpSerial.begin(BAUDRATE);
}

//Функционал
void loop() {  
  if (Serial.available()) {     // прием команд от ведомых
    //    delay(1);
    for (byte i = 0; i < 3; i++) MSG_IN[i] = MSG_IN[i + 1];
    MSG_IN[3] = Serial.read();
    if (MSG_IN[0] == START_BIT && MSG_IN[3] == STOP_BIT) { // пришло полное сообщение
      switch(MSG_IN[2]){// Выбираем что нам делать
        case COMMAND1:

          break;
        case COMMAND2:
          break;
        default:
          print("Неизвестная команда");
          break;
      }
    }
  }
}
