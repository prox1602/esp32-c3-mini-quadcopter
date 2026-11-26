#include "RobotManagementSystem.h"
#include "Rotation.h"

/// @brief It is responsible for controlling one single engine
class Engine
{
    int PIN; ///Engine's PWM pin

    int maxValue; ///the max value power can take

    /**
     * @brief it compute power from range [0, 1.0] to PWM
     * @return PWM value in uint32_t
     */
    uint32_t calculatePower(double value);
public:

    /**
     * @brief it sets up PWM control
     */
    void setup(int pin, int engineResolution, int engineFreq);

    /**
     * It sets the power to be received by the engine. 
     * @param Power in range [0, 1.0]
     */
    void update(double Power);
};

/// @brief It is responsible for controlling of all engines and for logic of flight
class EnginesController : public IController
{
    int _EngineResolution;
    int _EngineFreq;

    Engine _FrontLeftEngine;
    Engine _FrontRightEngine;
    Engine _BackLeftEngine;
    Engine _BackRightEngine;

    Service<Rotation>* _CurrentRotationService;
    Service<Rotation>* _FocusRotationService;
    Service<double>* _BasePowerService;

    double basePower = 0.0;
    double pidRoll = 0.0;
    double pidPitch = 0.0;
    double pidYaw = 0.0;

    double Kp = 0.015;

public:
    EnginesController();

    void setup() override;

    void update() override;
};