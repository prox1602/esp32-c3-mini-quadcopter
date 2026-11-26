#include "EnginesController.h"
#include <Arduino.h>
#include <math.h>

void Engine::setup(int pin, int engineResolution, int engineFreq)
{
    PIN = pin;

    ledcAttach(PIN, engineFreq, engineResolution);

    ledcWrite(PIN, 0);

    maxValue = pow(2, engineResolution) - 1;
}

uint32_t Engine::calculatePower(double value)
{
    double constrainedValue = constrain(value, 0, 1.0);

    //else return (uint32_t)maxValue*( (1.0 - 0.3)*value + 0.3 );
    return (uint32_t)maxValue*value;
}

void Engine::update(double Power)
{
    ledcWrite(PIN, calculatePower(Power));
}

EnginesController::EnginesController()
    : IController()
{
    _EngineResolution = 8;
    _EngineFreq = 16000;
}

void EnginesController::setup()
{
    _FrontLeftEngine.setup(D3, _EngineResolution, _EngineFreq);
    _FrontRightEngine.setup(D2, _EngineResolution, _EngineFreq);
    _BackLeftEngine.setup(D1, _EngineResolution, _EngineFreq);
    _BackRightEngine.setup(D0, _EngineResolution, _EngineFreq);


    _CurrentRotationService = DeviceController::getInstance().getServiceOrCreate<Rotation>("CurrentRotation");
    _FocusRotationService = DeviceController::getInstance().getServiceOrCreate<Rotation>("FocusRotation");
    _BasePowerService = DeviceController::getInstance().getServiceOrCreate<double>("BasePower");
    }

void EnginesController::update()
{
    basePower = _BasePowerService->read();

    if(basePower < 0.05)
    {
        _FrontLeftEngine.update(0);
        _FrontRightEngine.update(0);
        _BackLeftEngine.update(0);
        _BackRightEngine.update(0);

        return;
    }

    Rotation currentRotation = _CurrentRotationService->read();
    Rotation focusRotation = _FocusRotationService->read();

    double errorRoll = focusRotation.X_Angle - currentRotation.X_Angle;
    double errorPitch = focusRotation.Y_Angle - currentRotation.Y_Angle;
    double errorYaw = focusRotation.Z_Angle - currentRotation.Z_Angle;

    pidRoll = errorRoll * Kp;
    pidPitch = errorPitch * Kp;
    pidYaw = errorYaw * Kp;

    _FrontLeftEngine.update(basePower + pidPitch + pidRoll - pidYaw);
    _FrontRightEngine.update(basePower + pidPitch - pidRoll + pidYaw);
    _BackLeftEngine.update(basePower - pidPitch + pidRoll + pidYaw);
    _BackRightEngine.update(basePower - pidPitch - pidRoll - pidYaw);
}