#include "GyroController.h"

Rotation::Rotation(double x, double y, double z)
{
    X_Angle = x;
    Y_Angle = y;
    Z_Angle = z;
}

Rotation::Rotation()
{
    ;
}

GyroController::GyroController()
    : IController(), _CurrentRotation(0, 0, 0) {}

void GyroController::calibrate()
{
    //czszczenie pierwszych probek
    for(int i = 0; i < 200; i++)
    {
        int16_t dumpData[6] = {0};
        bmi160.getAccelGyroData(dumpData);
        delay(4);
    }



    double sumX = 0.0, sumY = 0.0, sumZ = 0.0;
    double sumRoll = 0.0, sumPitch = 0.0;

    for(int i = 0; i < 500; i++)
    {
        int16_t data[6] = {0};
        bmi160.getAccelGyroData(data);

        sumX += -data[0] / 16.4;
        sumY += -data[1] / 16.4;
        sumZ += data[2] / 16.4;

        double accelX = data[3];
        double accelY = data[4];
        double accelZ = -data[5];

        sumRoll += atan2(accelY, accelZ) * 180.0 / PI;
        sumPitch += atan2(-accelX, sqrt(accelY * accelY + accelZ * accelZ)) * 180.0 / PI;

        delay(4);
    }

    _gyroX_offset = sumX / 500.0;
    _gyroY_offset = sumY / 500.0;
    _gyroZ_offset = sumZ / 500.0;

    _CurrentRotation.X_Angle = sumRoll / 500.0;
    _CurrentRotation.Y_Angle = sumPitch / 500.0;
    _CurrentRotation.Z_Angle = 0.0;

}

void GyroController::setup()
{
    Serial.begin(115200);
    
    Wire.begin(D4, D5);

    while(bmi160.I2cInit(i2c_addr) != BMI160_OK)
    {
        Serial.println("Error: BMI160 has't been initiated!");
        delay(100);
    }

    _CurrentRotationService = DeviceController::getInstance().getServiceOrCreate<Rotation>("CurrentRotation");

    calibrate();

    _CurrentRotationService->publish(_CurrentRotation);

    _GyroTimer.reset();
}

void GyroController::update()
{
    if(_GyroTimer.getDelta() >= 5)
    {
        int16_t data[6] = {0};
        bmi160.getAccelGyroData(data);

        double deltaT = double(_GyroTimer.getDelta()) / 1000.0;
        _GyroTimer.reset();

        double gyroX_rate = (-data[0] / 16.4) - _gyroX_offset;
        double gyroY_rate = (-data[1] / 16.4) - _gyroY_offset;
        double gyroZ_rate = (data[2] / 16.4) - _gyroZ_offset;

        double accelX = data[3];
        double accelY = data[4];
        double accelZ = -data[5];

        double accRoll = atan2(accelY, accelZ) * 180.0 / PI;
        double accPitch = atan2(-accelX, sqrt(accelY * accelY + accelZ * accelZ)) * 180.0 / PI;


        _CurrentRotation.X_Angle = 0.998 * (_CurrentRotation.X_Angle + gyroX_rate * deltaT) + 0.002 * accRoll;
        _CurrentRotation.Y_Angle = 0.998 * (_CurrentRotation.Y_Angle + gyroY_rate * deltaT) + 0.002 * accPitch;

        _CurrentRotation.Z_Angle += gyroZ_rate * deltaT;

        /*
        static int printDiv = 0; 
        if(printDiv++ > 20) 
        {
            Serial.println(String(_CurrentRotation.X_Angle) + " " + String(_CurrentRotation.Y_Angle) + " " + String(_CurrentRotation.Z_Angle));
            printDiv=0; 
        }
        */
        

        _CurrentRotationService->publish(_CurrentRotation);
    }
}
