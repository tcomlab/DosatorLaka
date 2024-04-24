#include<Dosator.h>

Dosator::Dosator()
{

}
void Dosator::tare()
{
  scale.tare();
}

void Dosator::begin(int cal_factor, int clk_pin, int data_pin)
{
  scale.begin(data_pin, clk_pin, 64);
  scale.set_scale(cal_factor);
  scale.tare();	
}

void Dosator::setCalibration(int cal)
{
  scale.set_scale(cal);
}

Dosator::~Dosator()
{
}

void Dosator::work()
{
  static bool trigg;
  if (scale.wait_ready_timeout(1000)) {
      weght = scale.get_units(6);
    }

  if (auto_en){
    if(!cycle)
    {
      if (weght < -400)
          auto_trig = true;

      if (weght > -10)
      {
        if (auto_trig)
        {
          if (!trigg)
          {
            trigg = true;
            delay_tmp = millis();
          }

          if (millis() - delay_tmp > delay_auto){
            auto_trig = false;
            cycle = true;
            trigg = false;
          }
        }
      }
    } 
  }
  if (cycle)
  {
    if (!cycle_init)
    {
      cycle_init = true;
      scale.tare();
      _pump_en(true);
      is_dosage = true;
      cyle_time_tmp = millis();
    }

    if (reset)
    {
       _pump_dir(0);
      reset = false;
      cycle = false;
      _pump_en(false);
      is_dosage = false;
      cycle_init = false;
    }

    if (weght >= dosage_weght-correction)
    {
      _pump_freq(pump_speed); 
      _pump_dir(1);
      if (millis() - suck_back_tmp > suck_back){
        cycle = false;
        _pump_en(false);
        is_dosage = false;
        cycle_init = false;
        cylce_time = (millis() - cyle_time_tmp)/100 ;
        last_weght = weght;
      }
    }
    else
    { // TODO: Добавить замедление насоса при приближении наполнения до заданного значения
      suck_back_tmp = millis();
      _pump_dir(0);
      debug_1 = (int)((((float)dosage_weght/100))*80);
      if (((int)weght) > debug_1)
      {
        _pump_freq(pump_st1);
      }
      else 
      {
        _pump_freq(pump_speed); 
      }
    }
  }
}

void Dosator::start()
{
  cycle = true;
}