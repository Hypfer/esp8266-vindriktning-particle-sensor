#pragma once

#include <SoftwareSerial.h>

#include "Types.h"

namespace SerialCom {
    constexpr static const uint8_t PIN_UART_RX = 4; // D2 on Wemos D1 Mini
    constexpr static const uint8_t PIN_UART_TX = 13; // UNUSED

    SoftwareSerial sensorSerial(PIN_UART_RX, PIN_UART_TX);

    uint8_t serialRxBuf[255];
    uint8_t rxBufIdx = 0;

    void setup() { sensorSerial.begin(9600); }

    void clearRxBuf() {
        // Clear everything for the next message
        memset(serialRxBuf, 0, sizeof(serialRxBuf));
        rxBufIdx = 0;
    }

    void parseState(particleSensorState_t& state) {
        /**
         *         MSB  DF 3     DF 4  LSB
         * uint16_t = xxxxxxxx xxxxxxxx
         */
        const uint16_t pm25 = (serialRxBuf[5] << 8) | serialRxBuf[6];

        Serial.printf("Received PM 2.5 reading: %d\n", pm25);

        if (pm25 > 0) {
            state.measurements[state.measurementIdx] = pm25;

            state.measurementIdx = (state.measurementIdx + 1) % 5;

            if (state.measurementIdx == 0) {
                float avgPM25 = 0.0f;
                bool valid = true;

                for (uint8_t i = 0; i < 5; ++i) {
                    if (state.measurements[i] == 0) {
                        valid = false;
                        break;
                    } else {
                        avgPM25 += state.measurements[i] / 5.0f;
                    }
                }

                if (valid) {
                    state.avgPM25 = avgPM25;

                    Serial.printf("New Avg PM25: %d\n", state.avgPM25);
                }
            }
        }

        clearRxBuf();
    }

    void handleUart(particleSensorState_t& state) {
        if (!sensorSerial.available()) {
            return;
        }

        Serial.print("Receiving:");
        while (sensorSerial.available()) {
            serialRxBuf[rxBufIdx++] = sensorSerial.read();
            Serial.print(".");

            // Without this delay, receiving data breaks for reasons that are beyond me
            delay(15);

            if (rxBufIdx >= 64) {
                clearRxBuf();
            }
        }
        Serial.println("Done.");

        if (serialRxBuf[0] == 0x16 && serialRxBuf[1] == 0x11 && serialRxBuf[2] == 0x0B) {
            parseState(state);

            Serial.printf("Current measurements: %d, %d, %d, %d, %d", 
                state.measurements[0], 
                state.measurements[1],
                state.measurements[2], 
                state.measurements[3], 
                state.measurements[4]);
        } else {
            clearRxBuf();
        }
    }
} // namespace SerialCom
