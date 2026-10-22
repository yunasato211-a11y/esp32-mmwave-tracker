import asyncio
import json
import time

from websockets.asyncio.server import serve

from radar_simulator import create_targets


async def stream_radar_data(websocket):
    """持续向连接的网页发送模拟雷达数据。"""
    start_time = time.time()
    print("网页已连接。")

    try:
        while True:
            elapsed_seconds = time.time() - start_time

            radar_data = {
                "timestamp_ms": int(time.time() * 1000),
                "targets": create_targets(elapsed_seconds),
            }

            await websocket.send(
                json.dumps(radar_data, ensure_ascii=False)
            )
            await asyncio.sleep(0.1)

    finally:
        print("网页已断开。")


async def main():
    server = await serve(
        stream_radar_data,
        "localhost",
        8765,
    )

    print("雷达 WebSocket 服务器已启动：")
    print("ws://localhost:8765")
    print("按 Ctrl + C 停止服务器。")

    await server.serve_forever()


if __name__ == "__main__":
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        print("\n服务器已停止。")