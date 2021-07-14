boolean parseState() {
  unsigned long pm25 = serialRxBuf[3] * 256 ^ 3 + serialRxBuf[4] * 256 ^ 2 + serialRxBuf[5] * 256 ^ 1 + serialRxBuf[6];

  Serial.print("Received PM 2.5 reading: ");
  Serial.println(pm25);

  if (pm25 > 0) {
    state.measurements[state.measurementIdx] = pm25;

    state.measurementIdx = (state.measurementIdx + 1) % 5;

    if (state.measurementIdx == 0) {
      unsigned long avgPM25 = 0;
      boolean invalid = false;

      for (int i = 0; i < 5; i++) {
        if (state.measurements[i] == 0) {
          invalid = true;
        } else {
          avgPM25 += state.measurements[i];
        }
      }

      if (invalid == false) {
        state.avgPM25 = avgPM25 / 5;

        Serial.print("New Avg PM25: "); Serial.println(state.avgPM25);
      }
    }
  }

  clearRxBuf();
}

int rxBufIdx = 0;

void clearRxBuf() {
  //Clear everything for the next message
  memset(serialRxBuf, 0, sizeof(serialRxBuf));
  rxBufIdx = 0;
}



void handleUart() {

  if (particleSensorSerial.available()) {
    Serial.print("Receiving:");
  }

  int prevIdx = rxBufIdx;

  while (particleSensorSerial.available()) {
    serialRxBuf[rxBufIdx++] = particleSensorSerial.read();
    Serial.print(".");

    //Without this delay, receiving data breaks for reasons that are beyond me
    delay(15);

    if (rxBufIdx >= 64) {
      clearRxBuf();
    }
  }

  if (prevIdx != rxBufIdx) {
    Serial.println("Done.");
  }



  if (serialRxBuf[0] == 0x16 && serialRxBuf[1] == 0x11 && serialRxBuf[2] == 0x0b) {
    parseState();

    Serial.print("Current measurements: ");
    Serial.print(state.measurements[0]); Serial.print(", ");
    Serial.print(state.measurements[1]); Serial.print(", ");
    Serial.print(state.measurements[2]); Serial.print(", ");
    Serial.print(state.measurements[3]); Serial.print(", ");
    Serial.print(state.measurements[4]); Serial.print("\n");
  } else {
    clearRxBuf();
  }
}
