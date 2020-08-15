#include <Arduino.h>

/**
 * Class for handling battery percentage indicator LED.
 */

#ifndef BATTERY_MONITOR_H
#define BATTERY_MONITOR_H

// LiPo Battery charge percentages with corresponding voltage
#define BAT_INCREMENTS 5
#define PERCENT_100 4.20
#define PERCENT_95  4.15
#define PERCENT_90  4.11
#define PERCENT_85  4.08
#define PERCENT_80  4.02
#define PERCENT_75  3.98
#define PERCENT_70  3.95
#define PERCENT_65  3.91
#define PERCENT_60  3.87
#define PERCENT_55  3.85
#define PERCENT_50  3.84
#define PERCENT_45  3.82
#define PERCENT_40  3.80
#define PERCENT_35  3.79
#define PERCENT_30  3.77
#define PERCENT_25  3.75
#define PERCENT_20  3.73
#define PERCENT_15  3.71
#define PERCENT_10  3.69
#define PERCENT_5   3.61
#define PERCENT_0   3.40


class BatteryMonitor {
    public:
        int chargeRemaining();

    private:
        static float voltageArr[21];
        float analogToVoltage(int analogValue);
        
};

#endif // BATTERY_MONITOR_H
