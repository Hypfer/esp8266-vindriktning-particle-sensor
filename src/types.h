#pragma once

struct particleSensorState_t
{
    unsigned long avgPM25;
    unsigned long measurements[5] = {0, 0, 0, 0, 0};
    int measurementIdx = 0;
};
