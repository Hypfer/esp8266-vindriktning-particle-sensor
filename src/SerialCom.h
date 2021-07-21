#pragma once

#include <SoftwareSerial.h>

#include "Types.h"

namespace SerialCom
{
    constexpr static const uint8_t PIN_UART_RX = 4; // D2 on Wemos D1 Mini
    constexpr static const uint8_t PIN_UART_TX = 13; // UNUSED

    SoftwareSerial sensorSerial(PIN_UART_RX, PIN_UART_TX);

    uint8_t serialRxBuf[255];
    uint8_t rxBufIdx = 0;

    void setup() { sensorSerial.begin(9600); }

    void clearRxBuf()
    {
        // Clear everything for the next message
        memset(serialRxBuf, 0, sizeof(serialRxBuf));
        rxBufIdx = 0;
    }

    boolean parseState(particleSensorState_t& state)
    {
        unsigned long pm25 = serialRxBuf[5] * 256 + serialRxBuf[6];

        Serial.print("Received PM 2.5 reading: ");
        Serial.println(pm25);

        if (pm25 > 0)
        {
            state.measurements[state.measurementIdx] = pm25;

            state.measurementIdx = (state.measurementIdx + 1) % 5;

            if (state.measurementIdx == 0)
            {
                unsigned long avgPM25 = 0;
                boolean invalid = false;

                for (int i = 0; i < 5; i++)
                {
                    if (state.measurements[i] == 0)
                    {
                        invalid = true;
                    }
                    else
                    {
                        avgPM25 += state.measurements[i];
                    }
                }

                if (invalid == false)
                {
                    state.avgPM25 = avgPM25 / 5;

                    Serial.print("New Avg PM25: ");
                    Serial.println(state.avgPM25);
                }
            }
        }

        clearRxBuf();
    }

    void handleUart(particleSensorState_t& state)
    {

        if (sensorSerial.available())
        {
            Serial.print("Receiving:");
        }

        int prevIdx = rxBufIdx;

        while (sensorSerial.available())
        {
            serialRxBuf[rxBufIdx++] = sensorSerial.read();
            Serial.print(".");

            // Without this delay, receiving data breaks for reasons that are beyond me
            delay(15);

            if (rxBufIdx >= 64)
            {
                clearRxBuf();
            }
        }

        if (prevIdx != rxBufIdx)
        {
            Serial.println("Done.");
        }

        if (serialRxBuf[0] == 0x16 && serialRxBuf[1] == 0x11 && serialRxBuf[2] == 0x0b)
        {
            parseState(state);

            Serial.print("Current measurements: ");
            Serial.print(state.measurements[0]);
            Serial.print(", ");
            Serial.print(state.measurements[1]);
            Serial.print(", ");
            Serial.print(state.measurements[2]);
            Serial.print(", ");
            Serial.print(state.measurements[3]);
            Serial.print(", ");
            Serial.print(state.measurements[4]);
            Serial.print("\n");
        }
        else
        {
            clearRxBuf();
        }
    }
} // namespace SerialCom
