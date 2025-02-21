#include <FEHUtility.h>
#include <FEHServo.h>
#include <FEHIO.h>
#include <FEHLCD.h>
#include <FEHMotor.h>

AnalogInputPin touchSensorLeft(FEHIO::P3_0); // sensor front left
AnalogInputPin touchSensorRight(FEHIO::P0_0); // sensor front right
AnalogInputPin sensor3(FEHIO::P0_2); // sensor backRight

FEHMotor drive_motor_left(FEHMotor::Motor0, 9.0);
FEHMotor drive_motor_right(FEHMotor::Motor1, 9.0);

int main(void)
{
    int x,y;
    while(!LCD.Touch(&x, &y))
    {
        // Wait for screen touched
    }
    //Drive forward
    drive_motor_left.SetPercent(50);
    drive_motor_right.SetPercent(50);
    while()

}
