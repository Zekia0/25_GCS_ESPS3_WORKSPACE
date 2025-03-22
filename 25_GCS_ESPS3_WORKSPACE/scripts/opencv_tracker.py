import sys
sys.path.append("c:/users/zekia/appdata/roaming/python/python38/site-packages")
import cv2

# 打开摄像头
cap = cv2.VideoCapture(0)  # 0代表第一个摄像头
if not cap.isOpened():
    print("无法打开摄像头或视频文件")
    sys.exit(1)

# 读取第一帧
ret, frame = cap.read()
if not ret:
    print("无法读取视频帧")
    sys.exit(1)

# 手动输入ROI坐标
x = 320
y = 240
width = 50
height = 50
bbox = (x, y, width, height)

# 创建KCF跟踪器
tracker = cv2.TrackerKCF_create()
tracker.init(frame, bbox)  # 初始化跟踪器

while True:
    ret, frame = cap.read()  # 每次读取一帧
    if not ret:
        break  # 如果没有读取到视频，退出循环

    # 更新跟踪器
    success, bbox = tracker.update(frame)

    if success:
        # 解包边界框坐标
        (x, y, w, h) = [int(v) for v in bbox]
        # 绘制矩形
        cv2.rectangle(frame, (x, y), (x + w, y + h), (255, 0, 0), 2, 1)

    # 显示帧
    cv2.imshow("Tracking", frame)

    # 按下'c'键退出
    if cv2.waitKey(1) & 0xFF == ord('c'):
        break

# 释放摄像头并关闭所有窗口
cap.release()
cv2.destroyAllWindows()
