/*

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// IMU
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Connection of LSM9DS0 as I2C -------------------------------------------------

// Assign a unique base ID for the sensor
Adafruit_LSM9DS0 lsm = Adafruit_LSM9DS0(1000);  // Use I2C, ID #1000

// event object
sensors_event_t accel, mag, gyro, temp;

// configure the sensor: set range
void configureSensor(void)
{
  // 1.) Set the accelerometer range
  lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_2G);
  //lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_4G);
  //lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_6G);
  //lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_8G);
  //lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_16G);

  // 2.) Set the magnetometer sensitivity
  lsm.setupMag(lsm.LSM9DS0_MAGGAIN_2GAUSS);
  //lsm.setupMag(lsm.LSM9DS0_MAGGAIN_4GAUSS);
  //lsm.setupMag(lsm.LSM9DS0_MAGGAIN_8GAUSS);
  //lsm.setupMag(lsm.LSM9DS0_MAGGAIN_12GAUSS);

  // 3.) Setup the gyroscope
  lsm.setupGyro(lsm.LSM9DS0_GYROSCALE_245DPS);
  //lsm.setupGyro(lsm.LSM9DS0_GYROSCALE_500DPS);
  //lsm.setupGyro(lsm.LSM9DS0_GYROSCALE_2000DPS);
}

// how often should sample
//unsigned long interval_sampling_micro = 10000L; // 100 HZ
// unsigned long interval_sampling_micro = 20000L; // 50 HZ
//unsigned long interval_sampling_micro = 50000L; // 20 HZ
// unsigned long interval_sampling_micro = 100000L; // 10 HZ
unsigned long interval_sampling_micro = 200000L; // 5 HZ
// unsigned long interval_sampling_micro = 1000000L; // 1 HZ

// variables for timing when should measure again
unsigned long time_previous_IMU;

// IMU string
String dataStringIMU = "";


  if (!lsm.begin())
  {
    #if SERIAL_PRINT
      Serial.print(F("D;No LSM9DS0 detected"));
    #endif
    while (1);
  }
  #if SERIAL_PRINT
    Serial.println(F("D;found LSM9DS0"));
  #endif

  #if SERIAL_PRINT
    Serial.println(F("D;configure sensor"));
  #endif
  configureSensor();


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// generate the dataStringIMU to be posted on SD card
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void generateDataStringIMU(void){
  dataStringIMU = "";
  dataStringIMU += "I;";
  dataStringIMU += String(accel.acceleration.x);
  dataStringIMU += ";";
  dataStringIMU += String(accel.acceleration.y);
  dataStringIMU += ";";
  dataStringIMU += String(accel.acceleration.z);
  dataStringIMU += ";";
  dataStringIMU += String(gyro.gyro.x);
  dataStringIMU += ";";
  dataStringIMU += String(gyro.gyro.y);
  dataStringIMU += ";";
  dataStringIMU += String(gyro.gyro.z);
  dataStringIMU += ";";
  dataStringIMU += String(mag.magnetic.x);
  dataStringIMU += ";";
  dataStringIMU += String(mag.magnetic.y);
  dataStringIMU += ";";
  dataStringIMU += String(mag.magnetic.z);
  dataStringIMU += ";";
  dataStringIMU += "\nM,";
  dataStringIMU += String(millis());
}



// check if time to do new measurement
  if (micros() - time_previous_IMU >= interval_sampling_micro) {

    time_previous_IMU += interval_sampling_micro;

    // Get a new sensor event
    lsm.getEvent(&accel, &mag, &gyro, &temp);


    #if SERIAL_PRINT
      Serial.print("D;**********************\n");
      // print out accelleration data
      Serial.print("D;Accel X: "); Serial.print(accel.acceleration.x); Serial.print(" ");
      Serial.print("D;  \tY: "); Serial.print(accel.acceleration.y);       Serial.print(" ");
      Serial.print("D;  \tZ: "); Serial.print(accel.acceleration.z);     Serial.println("  \tm/s^2");

      // print out magnetometer data
      Serial.print("D;Magn. X: "); Serial.print(mag.magnetic.x); Serial.print(" ");
      Serial.print("D;  \tY: "); Serial.print(mag.magnetic.y);       Serial.print(" ");
      Serial.print("D;  \tZ: "); Serial.print(mag.magnetic.z);     Serial.println("  \tgauss");

      // print out gyroscopic data
      Serial.print("D;Gyro  X: "); Serial.print(gyro.gyro.x); Serial.print(" ");
      Serial.print("D;  \tY: "); Serial.print(gyro.gyro.y);       Serial.print(" ");
      Serial.print("D;  \tZ: "); Serial.print(gyro.gyro.z);     Serial.println("  \tdps");

      // print out temperature data
      Serial.print("D;Temp: "); Serial.print(temp.temperature); Serial.println(" *C");

      Serial.println("D;**********************\n");
    #endif


    // generate the string to post on SD card
    generateDataStringIMU();

    // post data
    // SD post
    postSD(dataStringIMU);
    #if SERIAL_PRINT
      Serial.println(dataStringIMU);
    #endif
  }
*/