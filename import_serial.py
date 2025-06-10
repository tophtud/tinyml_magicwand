import serial
import time
import os

# 포트 설정
PORT = 'COM5'           # 포트 번호 확인
BAUD_RATE = 115200
GESTURE_NAME = 'gesture'  # 파일 이름 prefix

# 파일 저장 폴더 (데스크탑의 'w 저장')
desktop_path = os.path.join(os.path.expanduser("~"), "Desktop")
save_folder = os.path.join(desktop_path, "slope")

# 폴더가 없으면 생성
os.makedirs(save_folder, exist_ok=True)

# 시리얼 연결
ser = serial.Serial(PORT, BAUD_RATE, timeout=1)
time.sleep(2)  # 아두이노 리셋 대기

print(f"✅ 연결됨: {PORT} / 저장 폴더: {save_folder}")

def get_next_filename():
    i = 1
    while True:
        filename = os.path.join(save_folder, f"{GESTURE_NAME}_{i:03}.csv")
        if not os.path.exists(filename):
            return filename
        i += 1

while True:
    input("▶ Enter를 눌러 '시작' 전송 → 제스처 수행")
    ser.write("시작\n".encode('utf-8'))

    filename = get_next_filename()
    print(f"💾 저장 중: {filename}")

    with open(filename, "w", encoding='utf-8') as f:
        while True:
            line = ser.readline().decode('utf-8').strip()
            if line:
                print(line)
                f.write(line + "\n")
            if "Done" in line or "■" in line:  # 아두이노가 종료 신호 보낼 경우
                break

    print("✅ 저장 완료\n")
