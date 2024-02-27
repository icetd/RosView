#ifndef VOLTAGE_CALCULATE_H
#define VOLAGTE_CALCULATE_H
#include <math.h>

float calcaulateVoltage(float voltage)
{
    // 三次多项式拟合
    static const float a = -5.1;
    static const float b = 4.39702142647148;
    static const float c = -0.324682373242227;
    static const float d = 0.00843230301062134;
    static float real_voltage, temp;

    temp = voltage;
    if (voltage > 23.8005) {
        temp = 23.8;
    } else if(voltage < 20.2384) {
        temp = 20.2384;       
    } 

    real_voltage = a + b * temp + c * pow(temp, 2) + d * pow(temp, 3);
    return real_voltage;
} 

float calculatePower(float voltage)
{
    // 四参数方程拟合
   static const float A = 104.159360977592;
   static const float B = -32.0831163582468;
   static const float C = 26.4623337526865;
   static const float D = -36.0632167550997;
   static float real_pwoer, temp;
    
   temp = voltage;
   if (temp < 25.55) {
        temp = 25.55;
   } else if (temp > 29.4) {
        temp = 29.4;
   }


   real_pwoer = (A - D) / (1 + pow(temp / C, B)) + D;
   return real_pwoer;
}

#endif