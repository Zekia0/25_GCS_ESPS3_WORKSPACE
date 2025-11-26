import cv2
import time
import random
import numpy as np
from rknnpool import rknnPoolExecutor
from old import myFunc


cap = cv2.VideoCapture(0)
fps = 0
prev_time = time.time()
frames = 0                                                                                                      
modelPath = '/home/orangepi/Workspace/25GCS_OrangePi/test_other.rknn'
# 线程数, 增大可提高帧率
TPEs = 2
# 初始化rknn池
 
pool = rknnPoolExecutor(
    rknnModel=modelPath,
    TPEs=TPEs,
    func=myFunc)

# 初始化异步所需要的帧
if (cap.isOpened()):
    for i in range(TPEs + 1):
        ret, frame = cap.read()
        if not ret:
            cap.release()
            del pool
            exit(-1)
        hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
        hsv[:,:,2] = np.clip(hsv[:,:,2] * 0.1, 0, 255)
        frame = cv2.cvtColor(hsv, cv2.COLOR_HSV2BGR)    
        pool.put(frame)

frames, loopTime, initTime = 0, time.time(), time.time()

# 初始化物体跟踪器
object_trackers = {}

while (cap.isOpened()):
    frames += 1
    ret, frame = cap.read()
    if not ret:
        break
    frames += 1
    current_time = time.time()
    if current_time - prev_time >= 1.0:
        fps = frames / (current_time - prev_time)
        frames = 0
        prev_time = current_time
    #print(frame.shape)
    pool.put(frame)
    frame, flag = pool.get()
    if flag == False:
        break
    #print(frame.shape)
    cv2.putText(frame, f"FPS: {fps:.2f}", (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)
    cv2.namedWindow('yolov8', cv2.WINDOW_KEEPRATIO)
    #cv2.namedWindow('yolov8', cv2.WINDOW_AUTOSIZE)
    cv2.imshow('yolov8', frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break
    if frames % 30 == 0:
        #print( 30 / (time.time() - loopTime))
        loopTime = time.time()

  
# 释放cap和rknn线程池
cap.release()
cv2.destroyAllWindows()
pool.release()

