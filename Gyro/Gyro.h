#ifndef Gyro_h
#define Gyro_h

#include "Arduino.h"

class Gyro
{
  public:
    Gyro(int addr);
    
    int GyroAddress;
    
    int accel_x_OC, accel_y_OC, accel_z_OC, gyro_x_OC ,gyro_y_OC, gyro_z_OC;
    float temp_scalled,accel_x_scalled,accel_y_scalled,accel_z_scalled,gyro_x_scalled,gyro_y_scalled,gyro_z_scalled;

    unsigned long t;

    float angle_x_gyro, angle_y_gyro, angle_z_gyro, angle_x_accel, angle_y_accel, angle_z_accel, angle_x, angle_y, angle_z;

	void readData();
	void resetWake();
	void setDLPF(int BW);
	void setGains(int gyro,int accel);
	void offsetCal();
	void calculateValues();

};

#endif