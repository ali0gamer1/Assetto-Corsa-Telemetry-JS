
struct SPageFilePhysics
{
    int packetId;

    float gas;
    float brake;
    float fuel;

    int gear;
    int rpms;

    float steerAngle;
    float speedKmh;

    float velocity[3];
    float accG[3];

    float wheelSlip[4];
    float wheelLoad[4];
    float wheelsPressure[4];
    float wheelAngularSpeed[4];

    float tyreWear[4];
    float tyreDirtyLevel[4];
    float tyreCoreTemperature[4];

    float camberRAD[4];
    float suspensionTravel[4];

    float drs;
    float tc;

    float heading;
    float pitch;
    float roll;

    float cgHeight;

    float carDamage[5];

    int numberOfTyresOut;
    int pitLimiterOn;

    float abs;

    float kersCharge;
    float kersInput;

    int autoShifterOn;

    float rideHeight[2];

    float turboBoost;
    float ballast;
    float airDensity;

    float airTemp;
    float roadTemp;

    float localAngularVel[3];

    float finalFF;

    float performanceMeter;
    int engineBrake;

    int ersRecoveryLevel;
    int ersPowerLevel;
    int ersHeatCharging;
    int ersIsCharging;

    float kersCurrentKJ;

    int drsAvailable;
    int drsEnabled;

    float brakeTemp[4];

    float clutch;

    float tyreTempI[4];
    float tyreTempM[4];
    float tyreTempO[4];

    int isAIControlled;

    float tyreContactPoint[4][3];
    float tyreContactNormal[4][3];
    float tyreContactHeading[4][3];

    float brakeBias;

    float localVelocity[3];

    int p2pActivations;
    int p2pStatus;

    float currentMaxRpm;

    float mz[4];

    float fx[4];
    float fy[4];

    float slipRatio[4];
    float slipAngle[4];

    int tcinAction;
    int absInAction;

    float suspensionDamage[4];

    float tyreTemp[4];

    float waterTemp;
    float brakePressure[4];
    float frontBrakeCompound;
    float rearBrakeCompound;

    float padLife[4];
    float discLife[4];

    int ignitionOn;
    int starterEngineOn;
    int isEngineRunning;

    float kerbVibration;
    float slipVibrations;
    float gVibrations;
    float absVibrations;
};