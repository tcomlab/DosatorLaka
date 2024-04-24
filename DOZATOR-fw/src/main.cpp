// MAIN SOFTWARE 08.05.21

/*
  Прошивка кондроллера машины для дозирования лака


  Доработки: 
  1. Добавлен отсос материала после успешного дозирования

  Редакция 23.08.2021
*/



#include <setting.h>

// Rs485 direction pin definition
#define MAX485 2



CRGB leds[12];

volatile Dosator Station1;
volatile Dosator Station2;

// Переменніе для синхронизации насосов
bool clk_pump1;
bool clk_pump2;
bool clk_led;


bool debug0,debug1,debug2,debug3,debug4,debug5,debug6,debug7;


ModbusMaster node;

unsigned long m_timer;
unsigned long m_timer1;

int current_window;
int setup_window;

void save_setting()
{
  eeprom_write_block((void*)&setting, 0, sizeof(Setting));
}

void read_setting()
{
  eeprom_read_block((void*)&setting, 0, sizeof(Setting));
}

void preTransmission()
{
  digitalWrite(MAX485, 1);
}

void postTransmission()
{
  digitalWrite(MAX485, 0);
}

ISR(TIMER1_A) {
    digitalWrite(A2, clk_pump1);
    clk_pump1 = !clk_pump1;
}

ISR(TIMER2_A) {
    digitalWrite(A0, clk_pump2);
    clk_pump2 = !clk_pump2;
}

void pump1(bool en)
{
  if (en)
    Timer1.enableISR();
  else
    Timer1.disableISR();
}

void pump2(bool en)
{
  if (en)
    Timer2.enableISR();
  else
    Timer2.disableISR();
}

void pump_f1(int freq)
{
  Timer1.setFrequency(freq);
}

void pump_f2(int freq)
{
  Timer2.setFrequency(freq);
}

void pump_dir1(bool direct)
{
  digitalWrite(A3, direct);
}

void pump_dir2(bool direct)
{
  digitalWrite(A1, direct);
}

void set_dosator_setting()
{
  Station1.pump_speed = setting.pump1_speed;
  Station2.pump_speed = setting.pump2_speed;

  Station1.pump_st1 = setting.pump1_f_st1;
  Station2.pump_st1 = setting.pump2_f_st1;

  Station1.correction = setting.correct1;
  Station2.correction = setting.correct2;

  Station1.suck_back = setting.flacon1_w;
  Station2.suck_back = setting.flacon2_w;
}

void setup() {
  // put your setup code here, to run once:
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(MAX485, OUTPUT);

  pinMode(KEY1, INPUT);
  pinMode(KEY2, INPUT);
  pinMode(RESET, INPUT);

  Serial.begin(115200);
  node.begin(1, Serial);
  
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);
  
  read_setting();

  Station1.begin(setting.calibration1,6,5);
  Station1._pump_en = pump1;
  Station1._pump_freq = pump_f1;
  Station1._pump_dir = pump_dir1;


  Station2.begin(setting.calibration2,4,3);
  Station2._pump_en = pump2;
  Station2._pump_freq = pump_f2;
  Station2._pump_dir = pump_dir2;

  //Station1.suck_back = 500;
  //Station2.suck_back = 500;

  set_dosator_setting();

  //Timer1.setFrequency(100);  
  //Timer2.setFrequency(100); 
  
  FastLED.addLeds<NEOPIXEL, 12>(leds, 12); 
}


void blink()
{
  if (clk_led){
    if (debug0) leds[0] = CRGB::Red;
    if (debug1) leds[1] = CRGB::Blue;
    if (debug2) leds[2] = CRGB::Red;
    if (debug3) leds[3] = CRGB::Blue;
    if (debug4) leds[4] = CRGB::Red;
    if (debug5) leds[5] = CRGB::Blue;
    if (debug6) leds[6] = CRGB::Red;
    if (debug7) leds[7] = CRGB::Blue;
  }
  else{
    if (debug0) leds[0] = CRGB::Black;
    if (debug1) leds[1] = CRGB::Black;
    if (debug2) leds[2] = CRGB::Black;
    if (debug3) leds[3] = CRGB::Black;
    if (debug4) leds[4] = CRGB::Black;
    if (debug5) leds[5] = CRGB::Black;
    if (debug6) leds[6] = CRGB::Black;
    if (debug7) leds[7] = CRGB::Black;
  }
  clk_led = !clk_led;
  FastLED.show();
}


void Window0(bool setup)
{
 
  int result;
  result = node.readHoldingRegisters(3, 1);
  if (result == node.ku8MBSuccess)
  {
      int d1 = node.getResponseBuffer(0);
      bool i0 = bitRead(d1,0); // 1 цикл станция 1
      bool i1 = bitRead(d1,1); // 1 цикл станция 2
      bool i2 = bitRead(d1,2); // AUTO MODE CH1
      bool i3 = bitRead(d1,3); // AUTO MODE CH2
      //bool i4 = bitRead(d1,4);
      //bool i5 = bitRead(d1,5);
      //bool i6 = bitRead(d1,6);
      //bool i7 = bitRead(d1,7);
      
      if (i0)
      {
        node.writeSingleRegister(3,bitClear(d1,0));
        Station1.start();
      }

      if (i1)
      {
        node.writeSingleRegister(3,bitClear(d1,1));
        Station2.start();
      }

      Station1.auto_en = i2;
      Station2.auto_en = i3;
    }

    result = node.readHoldingRegisters(33, 2);
    if (result == node.ku8MBSuccess)
    {
      Station1.dosage_weght = (node.getResponseBuffer(0)/10);
      Station2.dosage_weght  = (node.getResponseBuffer(1)/10);
  
    }
    node.writeSingleRegister(6,Station1.cylce_time);
    node.writeSingleRegister(7,Station2.cylce_time);
    node.writeSingleRegister(0,Station1.weght);
    node.writeSingleRegister(1,Station2.weght);
    node.writeSingleRegister(8,Station1.debug_1);
    node.writeSingleRegister(9,Station2.debug_1);

    //node.writeSingleRegister(9,Station2.bottle_w);
    //node.writeSingleRegister(9,Station2.bottle_w);

    result = node.readHoldingRegisters(10, 5);
    if (result == node.ku8MBSuccess)
    {
      Station1.delay_auto = (node.getResponseBuffer(0)*100);
      Station2.delay_auto  = (node.getResponseBuffer(1)*100);

      setting.button_func  = node.getResponseBuffer(2);

      Station1.correction  = node.getResponseBuffer(3);
      Station2.correction  = node.getResponseBuffer(4);
    }
}

void Window10(bool setup)
{
 
  int result;
  node.writeSingleRegister(0,Station1.weght);
  node.writeSingleRegister(1,Station2.weght);
  if (setup)
  {
    node.writeSingleRegister(4,setting.calibration1);
    node.writeSingleRegister(5,setting.calibration2);

    node.writeSingleRegister(12,setting.button_func);
    node.writeSingleRegister(35,setting.pump1_f_st1);
    node.writeSingleRegister(36,setting.pump1_f_st2);
    node.writeSingleRegister(37,setting.pump1_f_st3);
    node.writeSingleRegister(38,setting.pump1_speed);
    node.writeSingleRegister(39,setting.pump2_f_st1);
    node.writeSingleRegister(40,setting.pump2_f_st2);
    node.writeSingleRegister(41,setting.pump2_f_st3);
    node.writeSingleRegister(42,setting.pump2_speed);

    //node.writeSingleRegister(10,setting.delay_auto1);
    //node.writeSingleRegister(11,setting.delay_auto2);
    node.writeSingleRegister(15,setting.flacon1_w);
    node.writeSingleRegister(16,setting.flacon2_w);
  }
  else
  {
    //debug1 = false;
  }
  result = node.readHoldingRegisters(2, 1);
  if (result == node.ku8MBSuccess)
  {
      int d1 = node.getResponseBuffer(0);
      bool i0 = bitRead(d1,0); // Кнопка сохранить параметры
      bool i1 = bitRead(d1,1); // Кнопка сброса весов 1
      bool i2 = bitRead(d1,2); // Кнопка сброса весов 2
      bool i3 = bitRead(d1,3);
      bool i4 = bitRead(d1,4);
      bool i5 = bitRead(d1,5);
      bool i6 = bitRead(d1,6);
      bool i7 = bitRead(d1,7);
      
    
      if (i0)
      {
        node.writeSingleRegister(2,bitClear(d1,0));
        save_setting();
      }

      if (i1)
      {
        node.writeSingleRegister(2,bitClear(d1,1));
        Station1.tare();
      }

      if (i2)
      {
        node.writeSingleRegister(2,bitClear(d1,2));
        Station2.tare();
      }
    }

    result = node.readHoldingRegisters(4, 2);
    if (result == node.ku8MBSuccess)
    {
      setting.calibration1 = node.getResponseBuffer(0);
      setting.calibration2 = node.getResponseBuffer(1);
      Station1.setCalibration(setting.calibration1);
      Station2.setCalibration(setting.calibration2);
    }

    result = node.readHoldingRegisters(10, 3);
    if (result == node.ku8MBSuccess)
    {
      //Station1.delay_auto = (node.getResponseBuffer(0)*100);
      //Station2.delay_auto = (node.getResponseBuffer(1)*100);
      setting.button_func = node.getResponseBuffer(2);
    }

    result = node.readHoldingRegisters(15, 2);
    if (result == node.ku8MBSuccess)
    {
      setting.flacon1_w = node.getResponseBuffer(0);
      setting.flacon2_w = node.getResponseBuffer(1);
    }

    result = node.readHoldingRegisters(35, 8);
    if (result == node.ku8MBSuccess)
    {
      setting.pump1_f_st1 = node.getResponseBuffer(0);
      setting.pump1_f_st2 = node.getResponseBuffer(1);
      setting.pump1_f_st3 = node.getResponseBuffer(2);
      setting.pump1_speed = node.getResponseBuffer(3);
      setting.pump2_f_st1 = node.getResponseBuffer(4);
      setting.pump2_f_st2 = node.getResponseBuffer(5);
      setting.pump2_f_st3 = node.getResponseBuffer(6);
      setting.pump2_speed = node.getResponseBuffer(7);
      set_dosator_setting();
    }
}

void loop() {
  uint8_t result;
  static int mem_win = 999;
  static bool setup_win = true;

  if (!digitalRead(RESET))
  {
    Station1.work();
    Station2.work();
  }
  else
  {
    Station1.reset = true;
    Station2.reset = true;
  }

  if (millis() - m_timer1 > 300) {
    m_timer1 = millis();    
    blink();
  }

  if (millis() - m_timer > 150) {
    m_timer = millis();     
    result = node.readHoldingRegisters(32, 1);
    if (result == node.ku8MBSuccess)
      current_window = node.getResponseBuffer(0);

    if (current_window != mem_win)
    {
      setup_win = true;
      mem_win = current_window;
    }
    else 
      setup_win = false;

    switch(current_window)
    {
      case 0:
        Window0(setup_win);
        break;
      case 10:
        Window10(setup_win);
        break;
    }

  }


  switch(setting.button_func)
  {
    case 0:
      if (!digitalRead(KEY2))
        Station1.start();
      if (!digitalRead(KEY1))
        Station2.start();
    break;
        
    case 1:
        pump1(!digitalRead(KEY2));
        pump2(!digitalRead(KEY1));
    break;
  }

  
  
}
