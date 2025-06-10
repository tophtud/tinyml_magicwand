#include <Arduino_LSM9DS1.h>
#include <MicroTFLite.h>
#include "model.h"  // xx -i 로 변환한 TFLite 모델

#define NUM_SAMPLES 119
#define TENSOR_ARENA_SIZE (32 * 1024)
alignas(16) byte tensorArena[TENSOR_ARENA_SIZE];

const char* GESTURES[] = {"up", "down", "turn"};
#define NUM_GESTURES (sizeof(GESTURES) / sizeof(GESTURES[0]))

void setup() {
  Serial.begin(9600);
  while (!Serial);

  if (!IMU.begin()) {
    Serial.println("❌ IMU init failed!");
    while (1);
  }
  Serial.println("✅ IMU ready. Type '시작' to run.");
  
  if (!ModelInit(model, tensorArena, TENSOR_ARENA_SIZE)) {
    Serial.println("❌ Model init failed!");
    while (1);
  }
  Serial.println("✅ Model loaded.");
}

void loop() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd == "시작") {
      Serial.println("⚡ 시작 명령 수신! IMU 측정 시작합니다...");
      runInference();
    }
  }
}

void runInference() {
  float aX, aY, aZ, gX, gY, gZ;

  for (int i = 0; i < NUM_SAMPLES; i++) {
    while (!IMU.accelerationAvailable() || !IMU.gyroscopeAvailable());
    IMU.readAcceleration(aX, aY, aZ);
    IMU.readGyroscope(gX, gY, gZ);
Serial.print("aX: "); Serial.print(aX);
Serial.print(" aY: "); Serial.print(aY);
Serial.print(" aZ: "); Serial.print(aZ);
Serial.print(" gX: "); Serial.print(gX);
Serial.print(" gY: "); Serial.print(gY);
Serial.print(" gZ: "); Serial.println(gZ);
    // 정규화
    aX = (aX + 4.0) / 8.0;
    aY = (aY + 4.0) / 8.0;
    aZ = (aZ + 4.0) / 8.0;
    gX = (gX + 2000.0) / 4000.0;
    gY = (gY + 2000.0) / 4000.0;
    gZ = (gZ + 2000.0) / 4000.0;

    ModelSetInput(aX, i * 6 + 0);
    ModelSetInput(aY, i * 6 + 1);
    ModelSetInput(aZ, i * 6 + 2);
    ModelSetInput(gX, i * 6 + 3);
    ModelSetInput(gY, i * 6 + 4);
    ModelSetInput(gZ, i * 6 + 5);

    delayMicroseconds(10000);
  }

  if (ModelRunInference()) {
    float maxScore = 0;
    int maxIndex = -1;

    Serial.println("📊 Softmax 출력값:");
    for (int i = 0; i < NUM_GESTURES; i++) {
      float score = ModelGetOutput(i);
      Serial.print(" - ");
      Serial.print(GESTURES[i]);
      Serial.print(": ");
      Serial.println(score, 4);  // 소수점 4자리까지 출력

      if (score > maxScore) {
        maxScore = score;
        maxIndex = i;
      }
    }

    if (maxScore > 0.2) {
      Serial.print("🎯 예측된 제스처: ");
      Serial.print(GESTURES[maxIndex]);
      Serial.print(" (정확도: ");
      Serial.print(maxScore * 100, 1);
      Serial.println("%)");
    } else {
      Serial.println("🤷‍♂️ 명확한 제스처가 감지되지 않음.");
    }

  } else {
    Serial.println("❌ 추론 실패");
  }

  Serial.println("--- 다시 '시작'을 입력하세요 ---");
}