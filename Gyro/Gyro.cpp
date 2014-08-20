#include "Arduino.h"
#include <Wire.h>
#include "Gyro.h"

// ------ SELF Test Trim Factors --------------------------
#define MPU6050_self_test_x 13       // R/W
#define MPU6050_self_test_y 14       // R/W
#define MPU6050_self_test_z 15       // R/W
#define MPU6050_self_test_A 16       // R/W 

// ----- Sample Divider -----------------------------------
#define MPU6050_sample_div 25        // R/W
/*       Sample Divider Discription
  Sample rate = 8/(1 + Sample rate divider) [kHz]  if DLPF is disabled
  Sample rate = 1/(1 + Sample rate divider) [kHz]  if DLPF is enabled
*/

// ----- Configration -------------------------------------
#define MPU6050_config 26            // R/W
#define MPU6050_gyro_config 27       // R/W
#define MPU6050_accel_config 28      // R/W

// ----- Data ---------------------------------------------
#define MPU6050_data_start 59

// ----- Power Management --------------------------------- 
#define MPU6050_PWR1 107
#define MPU6050_PWR2 108

// ----- Defining Constant --------------------------------
#define g 9.81                       // Gravitational acceleration

// ----- Extra Constants ----------------------------------
#define dt 20                       // time diffrence in milli seconds
#define rad2degree 57.3              // Radian to degree conversion
#define Filter_gain 0.95             // e.g.  angle = angle_gyro*Filter_gain + angle_accel*(1-Filter_gain)

int calibrationLEDPin;

int temp=0, accel_x=0, accel_y=0, accel_z=0, gyro_x=0, gyro_y=0, gyro_z=0; // Raw values varaibles
int accel_x_OC=0, accel_y_OC=0, accel_z_OC=0, gyro_x_OC=0 ,gyro_y_OC=0, gyro_z_OC=0; // offset variables
float temp_scalled,accel_x_scalled,accel_y_scalled,accel_z_scalled,gyro_x_scalled,gyro_y_scalled,gyro_z_scalled; //Scalled Data varaibles
float accel_scale_fact = 1, gyro_scale_fact = 1; // Scale factor variables

Gyro::Gyro(int addr)
{
	GyroAddress = addr;

	t = 0; //Time variable
	
	angle_x_gyro = 0;
	angle_y_gyro = 0;
	angle_z_gyro = 0;

	angle_x_accel = 0;
	angle_y_accel = 0;
	angle_z_accel = 0;
	
	angle_x = 0;
	angle_y = 0;
	angle_z = 0;

}

void Gyro::readData(){

	Wire.beginTransmission(GyroAddress);
	Wire.write(MPU6050_data_start);
	Wire.endTransmission();

	int read_bytes = 14;

	Wire.requestFrom(GyroAddress,read_bytes);

	if(Wire.available() == read_bytes){

		accel_x = Wire.read()<<8 | Wire.read();
		accel_y = Wire.read()<<8 | Wire.read();
		accel_z = Wire.read()<<8 | Wire.read();

		temp = Wire.read()<<8 | Wire.read();

		gyro_x = Wire.read()<<8 | Wire.read();
		gyro_y = Wire.read()<<8 | Wire.read();
		gyro_z = Wire.read()<<8 | Wire.read();

	}

	accel_x_scalled = (float)(accel_x-accel_x_OC)*accel_scale_fact/1000;
	accel_y_scalled = (float)(accel_y-accel_y_OC)*accel_scale_fact/1000;
	accel_z_scalled = (float)(accel_z-accel_z_OC)*accel_scale_fact/1000;

	gyro_x_scalled = (float)(gyro_x-gyro_x_OC)*gyro_scale_fact/1000;
	gyro_y_scalled = (float)(gyro_y-gyro_y_OC)*gyro_scale_fact/1000;
	gyro_z_scalled = ((float)(gyro_z-gyro_z_OC)*gyro_scale_fact/1000);

	temp_scalled = (float)temp/340+36.53;


}

void Gyro::resetWake(){

	Serial.println("Resetting MPU6050 and waking it up.....");
	Wire.beginTransmission(GyroAddress);
	Wire.write(MPU6050_PWR1);
	Wire.write(0b10000000);
	Wire.endTransmission();

	delay(100); // Waiting for the reset to complete

	Wire.beginTransmission(GyroAddress);
	Wire.write(MPU6050_PWR1);

	Wire.write(0b00000000);
	Wire.endTransmission();

}

void Gyro::setDLPF(int BW){
	
	if (BW < 0 || BW > 6){
		BW = 0;
	}

	Wire.beginTransmission(GyroAddress);
	Wire.write(MPU6050_config);
	Wire.write(BW);
	Wire.endTransmission();
}

void Gyro::setGains(int gyro,int accel){
	byte gyro_byte,accel_byte;

	// Setting up Gyro
	Wire.beginTransmission(GyroAddress);
	Wire.write(MPU6050_gyro_config); // Address to the configuration register
	if (gyro==0){

		gyro_scale_fact =(float)250*0.0305; // each data is of 16 bits that means, 250 is divided along 2^(15)-1 = 32767 so for milli degree/s 0.0305 = 1000/32767
		gyro_byte = 0b00000000;
	
	} else if (gyro == 1){
	
		gyro_scale_fact = 500*0.0305; // each data is of 16 bits that means, 500 is divided along 2^(15)-1 = 32767 so for milli degree/s 0.0305 = 1000/32767
		gyro_byte = 0b00001000;
	
	} else if (gyro == 2){
	
		gyro_scale_fact = 1000*0.0305;// each data is of 16 bits that means, 1000 is divided along 2^(15)-1 = 32767 so for milli degree/s 0.0305 = 1000/32767
		gyro_byte = 0b00010000;
	
	} else if (gyro == 3){
	
		gyro_scale_fact = 2000*0.0305;  // each data is of 16 bits that means, 2000 is divided along 2^(15)-1 = 32767 so for milli degree/s 0.0305 = 1000/32767
		gyro_byte = 0b00011000;
	
	} else {
	
		gyro_scale_fact = 1;
	
	}  

	Wire.write(gyro_byte);
	Wire.endTransmission();
	Serial.print("The gyro scale is set to ");
	Serial.print(gyro_scale_fact);
	Serial.println(" milli Degree/s");

	// Setting up Accel
	Wire.beginTransmission(GyroAddress);
	Wire.write(MPU6050_accel_config); // Address to the configuration register
	if (accel==0){
		accel_scale_fact =(float)2*g*0.0305; // each data is of 16 bits that means, 2g is divided along 2^(15)-1 = 32767 so for milli m/s^2 0.0305 = 1000/32767       
		accel_byte = 0b00000000;
	} else if (accel == 1){
		accel_scale_fact = 4*g*0.0305; // each data is of 16 bits that means, 4g is divided along 2^(15)-1 = 32767 so for milli m/s^2 0.0305 = 1000/32767
		accel_byte = 0b00001000;
	} else if (accel == 2){
		accel_scale_fact = 8*g*0.0305;// each data is of 16 bits that means, 8g is divided along 2^(15)-1 = 32767 so for milli m/s^2 0.0305 = 1000/32767
		accel_byte = 0b00010000;
	} else if (accel == 3){
		accel_scale_fact = 16*g*0.0305; // each data is of 16 bits that means, 16g is divided along 2^(15)-1 = 32767 so for milli m/s^2 0.0305 = 1000/32767
		accel_byte = 0b00011000;
	} else{
		accel_scale_fact = 1;
	}  

	Wire.write(accel_byte);
	Wire.endTransmission();
	Serial.print("The accel scale is set to ");
	Serial.print(accel_scale_fact);
	Serial.println(" milli m/s^2");

}

void Gyro::offsetCal(){

	Serial.println("Calibrating gyroscope .... dont move the hardware ..........");

	int x = 0 , y = 0 , z = 0, i;

	readData();
	readData();

	// Gyro Offset Calculation
	x=gyro_x;
	y=gyro_y;
	z=gyro_z;

	for (i=1;i<=1000;i++){
		readData();
		x=(x+gyro_x)/2;
		y=(y+gyro_y)/2;
		z=(z+gyro_z)/2;
		Serial.print(".");
	}

	Serial.println(".");
	gyro_x_OC = x;
	gyro_y_OC = y;
	gyro_z_OC = z;

	Serial.print("gyro_x register offset = ");
	Serial.println(x);

	Serial.print("gyro_y register offect = ");
	Serial.println(y);

	Serial.print("gyro_z register offset = ");
	Serial.println(z);

	// Accel Offset Calculation
	Serial.println("Calibrating accelerometer .... Don't move the hardware ..........");
	x = accel_x;
	y = accel_y;
	z = accel_z;

	for (i=1;i<=1000;i++){
		readData();
		x = (x+accel_x) / 2;
		y = (y+accel_y) / 2;
		z = (z+accel_z) / 2;
		Serial.print(".");
	}

	Serial.println(".");
	accel_x_OC = x;
	accel_y_OC = y;
	accel_z_OC = z - (float)g * 1000 / accel_scale_fact;

	Serial.print("Accel_x register offset = ");
	Serial.println(x);

	Serial.print("Accel_y register offect = ");
	Serial.println(y);

	Serial.print("Accel_z register offset = ");
	Serial.println(z);

	if(calibrationLEDPin != 0){
		digitalWrite(calibrationLEDPin, HIGH);
	}

}

void Gyro::setCalibrationLightPin(int LEDPin){

	calibrationLEDPin = LEDPin;
	pinMode(LEDPin, OUTPUT);

}

void Gyro::setThresholds(int x, int y, int z){

	x_threshold = x;
	y_threshold = y;
	z_threshold = z;

}

void Gyro::calculateValues(){

	angle_x_gyro = (gyro_x_scalled * ( (float) dt / 1000) + angle_x);
	angle_y_gyro = (gyro_y_scalled * ( (float) dt / 1000) + angle_y);
	angle_z_gyro = (gyro_z_scalled * ( (float)dt / 1000) + angle_z);

	angle_z_accel = atan(accel_z_scalled / (sqrt(accel_y_scalled * accel_y_scalled + accel_x_scalled * accel_x_scalled))) * (float)rad2degree;
	angle_y_accel = -atan(accel_x_scalled / (sqrt(accel_y_scalled * accel_y_scalled + accel_z_scalled * accel_z_scalled))) * (float)rad2degree;
	angle_x_accel = atan(accel_y_scalled / (sqrt(accel_x_scalled * accel_x_scalled + accel_z_scalled * accel_z_scalled))) * (float)rad2degree;

	angle_x = Filter_gain * angle_x_gyro + ( 1 - Filter_gain) * angle_x_accel;
	angle_y = Filter_gain * angle_y_gyro + ( 1 - Filter_gain) * angle_y_accel;
	angle_z = Filter_gain * angle_z_gyro + ( 1 - Filter_gain) * angle_z_accel;

}