import json
import math
import time


def create_targets(elapsed_seconds):
    """生成三个模拟目标的位置和速度。"""
    return [
        {
            "id": 1,
            "x_mm": int(1200 * math.sin(elapsed_seconds)),
            "y_mm": int(2500 + 500 * math.cos(elapsed_seconds)),
            "speed_cm_s": int(120 * math.cos(elapsed_seconds)),
        },
        {
            "id": 2,
            "x_mm": int(-1800 + elapsed_seconds * 200) % 3600 - 1800,
            "y_mm": 3800,
            "speed_cm_s": 20,
        },
        {
            "id": 3,
            "x_mm": int(800 * math.cos(elapsed_seconds * 0.6)),
            "y_mm": int(1500 + 600 * math.sin(elapsed_seconds * 0.6)),
            "speed_cm_s": int(48 * math.cos(elapsed_seconds * 0.6)),
        },
    ]


def main():
    start_time = time.time()

    try:
        while True:
            elapsed_seconds = time.time() - start_time

            radar_data = {
                "timestamp_ms": int(time.time() * 1000),
                "targets": create_targets(elapsed_seconds),
            }

            print(json.dumps(radar_data, ensure_ascii=False))
            time.sleep(0.1)

    except KeyboardInterrupt:
        print("\n模拟器已停止。")


if __name__ == "__main__":
    main()