#include <math.h>
#include "BatteryMonitor.h"

float BatteryMonitor::voltageArr[] = {
            PERCENT_5,
            PERCENT_10,
            PERCENT_15,
            PERCENT_20,
            PERCENT_25,
            PERCENT_30,
            PERCENT_35,
            PERCENT_40,
            PERCENT_45,
            PERCENT_50,
            PERCENT_55,
            PERCENT_60,
            PERCENT_65,
            PERCENT_70,
            PERCENT_75,
            PERCENT_80,
            PERCENT_85,
            PERCENT_90,
            PERCENT_95,
            PERCENT_100
        };


// Calculate the remaining battery percentage of a LiPo cell.
int BatteryMonitor::chargeRemaining() {
    float voltage = analogToVoltage(analogRead(A0));

    int percent = 0;
    for (int i = 1; i < 21; i++) {
        if (voltage < voltageArr[i]) { 
            break;
        }
        else {
            percent += 5;
        }
    }

    return percent;
}


// Convert an analog reading to equivalent voltage level.
// N.B. Assumes that the underlying hardware supports 10-bit analog readings (0-1023)
float BatteryMonitor::analogToVoltage(int analogValue) {
    float voltage = analogValue * (5.0 / 1023.0);
    return voltage;
}
