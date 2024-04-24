#ifndef dosator_h
#define dosator_h

#include <HX711.h>

#define REW 1
#define FWD 0


class Dosator
{
private:
  /* data */
  HX711 scale;
  bool cycle;
  bool cycle_init;
  long cyle_time_tmp;
  long last_weght;
  long delay_tmp;
  bool auto_trig;
public:
  bool auto_en;
  int auto_delay;
  int bottle_w;
  long weght;
  float weght_f;
  int dosage_weght;
  bool is_dosage;
  void begin(int cal_factor, int clk_pin, int data_pin);
  void tare();
  Dosator();
  ~Dosator();
  void work();
  void start();
  void (*_pump_en)(bool en);
  void (*_pump_freq)(int freq);
  void (*_pump_dir)(bool dir);
  void setCalibration(int cal);
  long cylce_time;
  bool debug;
  int debug_1;
  int pump_st1;
  int pump_st2;
  int pump_st3;
  int pump_speed;
  int delay_auto;
  int correction;
  long suck_back;
  long suck_back_tmp;
  bool reset;
};


#endif