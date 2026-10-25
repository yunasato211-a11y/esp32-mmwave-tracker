#include <Arduino.h>

constexpr int RADAR_RX_PIN = 4;
constexpr int RADAR_TX_PIN = 5;
constexpr uint32_t RADAR_BAUD = 256000;
constexpr size_t FRAME_SIZE = 30;

uint8_t frame[FRAME_SIZE];
size_t frameIndex = 0;
uint32_t lastPrintTime = 0;

int16_t decodeSignedValue(const uint8_t low, const uint8_t high) {
    uint16_t raw = low | (static_cast<uint16_t>(high) << 8);
    int16_t magnitude = raw & 0x7FFF;

    return (raw & 0x8000) ? magnitude : -magnitude;
}

uint16_t decodeUnsignedValue(const uint8_t low, const uint8_t high) {
    return low | (static_cast<uint16_t>(high) << 8);
}

void printTargets() {
    if (millis() - lastPrintTime < 500) {
        return;
    }

    lastPrintTime = millis();
    bool foundTarget = false;

    for (int targetIndex = 0; targetIndex < 3; targetIndex++) {
        const int offset = 4 + targetIndex * 8;

        int16_t x = decodeSignedValue(frame[offset], frame[offset + 1]);
        int16_t y = decodeSignedValue(frame[offset + 2], frame[offset + 3]);
        int16_t speed =
            decodeSignedValue(frame[offset + 4], frame[offset + 5]);
        uint16_t resolution =
            decodeUnsignedValue(frame[offset + 6], frame[offset + 7]);

        if (x == 0 && y == 0 && speed == 0 && resolution == 0) {
            continue;
        }

        foundTarget = true;

        Serial.printf(
            "Target %d: X=%d mm, Y=%d mm, speed=%d cm/s, resolution=%u mm\n",
            targetIndex + 1,
            x,
            y,
            speed,
            resolution
        );
    }

    if (!foundTarget) {
        Serial.println("No target");
    }

    Serial.println();
}

void processRadarByte(uint8_t value) {
    constexpr uint8_t HEADER[] = {0xAA, 0xFF, 0x03, 0x00};

    if (frameIndex < 4) {
        if (value == HEADER[frameIndex]) {
            frame[frameIndex++] = value;
        } else {
            frameIndex = 0;

            if (value == HEADER[0]) {
                frame[frameIndex++] = value;
            }
        }

        return;
    }

    frame[frameIndex++] = value;

    if (frameIndex == FRAME_SIZE) {
        if (frame[28] == 0x55 && frame[29] == 0xCC) {
            printTargets();
        }

        frameIndex = 0;
    }
}

void setup() {
    Serial.begin(115200);
    delay(1500);

    Serial1.begin(
        RADAR_BAUD,
        SERIAL_8N1,
        RADAR_RX_PIN,
        RADAR_TX_PIN
    );

    Serial.println("LD2450 target parser started");
}

void loop() {
    while (Serial1.available() > 0) {
        processRadarByte(Serial1.read());
    }
}