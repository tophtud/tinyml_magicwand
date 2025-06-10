#include <Arduino_LSM9DS1.h>

bool capturing = false;
int sampleCount = 0;
bool headerPrinted = false;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  Serial.println("Type '시작' to begin, '정지' to stop.");
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    if (input == "시작") {
      capturing = true;
      sampleCount = 0;
      headerPrinted = false; // 헤더 다시 출력할 준비
      Serial.println("▶ IMU capture started...");
    }
  }

  if (capturing) {
    float aX, aY, aZ, gX, gY, gZ;

    if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
      IMU.readAcceleration(aX, aY, aZ);
      IMU.readGyroscope(gX, gY, gZ);

      // 🔹 헤더 1회 출력
      if (!headerPrinted) {
        Serial.println("aX,aY,aZ,gX,gY,gZ");
        headerPrinted = true;
      }

      Serial.print(aX, 3); Serial.print(',');
      Serial.print(aY, 3); Serial.print(',');
      Serial.print(aZ, 3); Serial.print(',');
      Serial.print(gX, 3); Serial.print(',');
      Serial.print(gY, 3); Serial.print(',');
      Serial.println(gZ, 3);

      sampleCount++;
      if (sampleCount >= 119) {
        capturing = false;
        Serial.println("■ IMU capture stopped.");
      }
    }
  }
}