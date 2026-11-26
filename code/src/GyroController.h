#pragma once
#include <Wire.h>
#include <DFRobot_BMI160.h>
#include "RobotManagementSystem.h"
#include "Rotation.h"

/// @brief It is used for counting current orientation of the drone
class GyroController : public IController
{
    Rotation _CurrentRotation;

    Service<Rotation>* _CurrentRotationService; 

    DFRobot_BMI160 bmi160;

    const int i2c_addr = 0x69;

    Timer _GyroTimer;

    double _gyroX_offset = 0.0;
    double _gyroY_offset = 0.0;
    double _gyroZ_offset = 0.0;

    void calibrate();
public:
    GyroController();

    void setup() override;

    void update() override;
};
