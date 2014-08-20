#MPU6050_Arduino - Library
===============

This library allows for the use of two MPU6050 Gyro's in an Arduino project. It has been based on the code originally written by Helscream but has been wrapped in a Gyro class to allow for different initialisation options, in this case, different I2C addresses (0x68 || 0x69).

###Usage

####Installation

Drag the Gyro folder into your Arduino libraries folder and then include it in your sketch.

```cpp
	#include <Gyro.h>
```

####Setup

To start using a MPU6050 Gyro, you must first create an instance of the class with a name and pass through the I2C address you want the gyro to communicate with.

```cpp
	Gyro first(0x68)
```

Next, in the 'setup' loop, you call the exact same methods as Helscreams library but for your instance instead of just global method calls...

```cpp

void setup(){

	first.resetWake();
	first.setGains(0,1);// Setting the lows scale
	first.setDLPF(0); // Setting the DLPF to inf Bandwidth for calibration
	first.setCalibrationLightPin(10);
	first.offsetCal();
	first.setDLPF(6); // Setting the DLPF to lowest Bandwidth
	first.setThresholds(15, 25, 40); // X, Y, Z

}

```

####Reading Values

In the 'loop' loop you then call the readData and calculateValues. After this, all of the values for your MPU6050 gyro will be available and up to date for reading.

```cpp
	void loop(){
		
		first.readData();
		first.calculateValues();

		Serial.println(first.angle_x_gyro);
		Serial.println(first.angle_y_gyro);
		Serial.println(first.angle_z_gyro);

		Serial.println(first.angle_x_accel);
		Serial.println(first.angle_y_accel);
		Serial.println(first.angle_z_accel);
		//...etc....

	}
```


