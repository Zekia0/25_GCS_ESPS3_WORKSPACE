import cv2
import numpy as np
import serial 
import time 
import sys
sys.path.append('/usr/local/lib/python3.10/dist-packages/wiringpi-2.60.1-py3.10-linux-aarch64.egg/')
import wiringpi as wp
from wiringpi import GPIO

GPIO_PIN = 7

class_id = 6
center_x = 640
center_y = 480

wp.wiringPiSetup()
wp.pinMode(GPIO_PIN, wp.INPUT)

serial_port1 = '/dev/ttyS2' 
baud_rate = 115200 
ser1 = serial.Serial(serial_port1, baud_rate, timeout=0)  # 串口初始化

OBJ_THRESH = 0.45
NMS_THRESH = 0.45
IMG_SIZE = 640

CLASSES = ("else","harmful","kitchen","recyclables")

# 椭圆屏蔽区域参数
# 小椭圆（内椭圆）- 屏蔽区域
ELLIPSE_INNER_CENTER_X = 290  # 小椭圆中心X坐标
ELLIPSE_INNER_CENTER_Y = 180  # 小椭圆中心Y坐标
ELLIPSE_INNER_AXIS_A = 105    # 小椭圆长轴
ELLIPSE_INNER_AXIS_B = 80     # 小椭圆短轴

# 大椭圆（外椭圆）- 屏蔽区域
ELLIPSE_OUTER_CENTER_X = 300  # 大椭圆中心X坐标
ELLIPSE_OUTER_CENTER_Y = 220  # 大椭圆中心Y坐标
ELLIPSE_OUTER_AXIS_A = 280    # 大椭圆长轴
ELLIPSE_OUTER_AXIS_B = 260    # 大椭圆短轴


def is_point_in_ellipse(x, y, center_x, center_y, axis_a, axis_b):
    """
    判断点(x, y)是否在椭圆内
    椭圆方程: ((x-cx)/a)^2 + ((y-cy)/b)^2 <= 1
    """
    dx = (x - center_x) / axis_a
    dy = (y - center_y) / axis_b
    return (dx * dx + dy * dy) <= 1.0


def is_point_in_detection_zone(x, y):
    """
    判断点(x, y)是否在识别区域内
    识别区域：大椭圆内且小椭圆外的环形区域
    返回True表示在识别区域内，False表示在屏蔽区域内
    """
    # 检查是否在小椭圆内
    in_inner_ellipse = is_point_in_ellipse(x, y, 
                                           ELLIPSE_INNER_CENTER_X, ELLIPSE_INNER_CENTER_Y,
                                           ELLIPSE_INNER_AXIS_A, ELLIPSE_INNER_AXIS_B)
    
    # 检查是否在大椭圆内
    in_outer_ellipse = is_point_in_ellipse(x, y,
                                           ELLIPSE_OUTER_CENTER_X, ELLIPSE_OUTER_CENTER_Y,
                                           ELLIPSE_OUTER_AXIS_A, ELLIPSE_OUTER_AXIS_B)
    
    # 识别区域：在大椭圆内且在小椭圆外
    return in_outer_ellipse and not in_inner_ellipse


def int_to_bytes(value, length, byte_order='big'):
    # 添加类型转换确保为Python int
    return int(value).to_bytes(length, byte_order)


def sender(posx, posy, class_type):
    # 添加数值类型转换
    data_x = int_to_bytes(int(posx), 2, 'big')
    data_y = int_to_bytes(int(posy), 2, 'big')

    class_id = int_to_bytes(int(class_type), 1, 'little')
    
    frame_head = b'\x21\x2c'
    frame_foot = b'\x5b'

    data_pack = frame_head + class_id + data_x + data_y + frame_foot
    ser1.write(data_pack) 


def draw(image, boxes, scores, classes, ratio, padding):
    for box, score, cl in zip(boxes, scores, classes):
        top, left, right, bottom = box
        # 已存在的中心坐标计算
        global center_x
        global center_y
        global class_id

        center_x = int((top + right) / 2)
        center_y = int((left + bottom) / 2)
        
        # 将坐标从模型输出尺寸转换回原始图像尺寸
        center_x_orig = int((center_x - padding[0]) / ratio[0])
        center_y_orig = int((center_y - padding[1]) / ratio[1])
        
        # 检查是否在识别区域内（环形区域）
        if not is_point_in_detection_zone(center_x_orig, center_y_orig):
            # 不在识别区域内（在屏蔽区域内），跳过不处理
            continue

        if(cl==0):
            class_id=5
        elif(cl==1):
            class_id=4
        elif(cl==2):
            class_id=3
        elif(cl==3):
            class_id=2
        
        # 修改第三个参数为类别索引cl
        # 先执行发送
        sender(center_x, center_y, class_id)
        print("发送成功")
        print(center_x,center_y,class_id)
        # 发送完成后执行校验
            
        top = (top - padding[0])/ratio[0]
        left = (left - padding[1])/ratio[1]
        right = (right - padding[0])/ratio[0]
        bottom = (bottom - padding[1])/ratio[1]
        # print('class: {}, score: {}'.format(CLASSES[cl], score))
        # print('box coordinate left,top,right,down: [{}, {}, {}, {}]'.format(top, left, right, bottom))
        top = int(top)
        left = int(left)

        cv2.rectangle(image, (top, left), (int(right), int(bottom)), (255, 0, 0), 2)
        cv2.putText(image, '{0} {1:.2f}'.format(CLASSES[cl], score),
                    (top, left - 6),
                    cv2.FONT_HERSHEY_SIMPLEX,
                    0.6, (0, 0, 255), 2)


def filter_boxes(boxes, box_confidences, box_class_probs):
    """Filter boxes with object threshold.
    """
    box_confidences = box_confidences.reshape(-1)
    candidate, class_num = box_class_probs.shape

    class_max_score = np.max(box_class_probs, axis=-1)
    classes = np.argmax(box_class_probs, axis=-1)

    _class_pos = np.where(class_max_score* box_confidences >= OBJ_THRESH)
    scores = (class_max_score* box_confidences)[_class_pos]

    boxes = boxes[_class_pos]
    classes = classes[_class_pos]

    return boxes, classes, scores

def nms_boxes(boxes, scores):
    """Suppress non-maximal boxes.
    # Returns
        keep: ndarray, index of effective boxes.
    """
    x = boxes[:, 0]
    y = boxes[:, 1]
    w = boxes[:, 2] - boxes[:, 0]
    h = boxes[:, 3] - boxes[:, 1]

    areas = w * h
    order = scores.argsort()[::-1]

    keep = []
    while order.size > 0:
        i = order[0]
        keep.append(i)

        xx1 = np.maximum(x[i], x[order[1:]])
        yy1 = np.maximum(y[i], y[order[1:]])
        xx2 = np.minimum(x[i] + w[i], x[order[1:]] + w[order[1:]])
        yy2 = np.minimum(y[i] + h[i], y[order[1:]] + h[order[1:]])

        w1 = np.maximum(0.0, xx2 - xx1 + 0.00001)
        h1 = np.maximum(0.0, yy2 - yy1 + 0.00001)
        inter = w1 * h1

        ovr = inter / (areas[i] + areas[order[1:]] - inter)
        inds = np.where(ovr <= NMS_THRESH)[0]
        order = order[inds + 1]
    keep = np.array(keep)
    return keep


def dfl(position):
    # Distribution Focal Loss (DFL)
    # x = np.array(position)
    n,c,h,w = position.shape
    p_num = 4
    mc = c//p_num
    y = position.reshape(n,p_num,mc,h,w)
    
    # Vectorized softmax
    e_y = np.exp(y - np.max(y, axis=2, keepdims=True))  # subtract max for numerical stability
    y = e_y / np.sum(e_y, axis=2, keepdims=True)
    
    acc_metrix = np.arange(mc).reshape(1,1,mc,1,1)
    y = (y*acc_metrix).sum(2)
    return y
    

def box_process(position):
    grid_h, grid_w = position.shape[2:4]
    col, row = np.meshgrid(np.arange(0, grid_w), np.arange(0, grid_h))
    col = col.reshape(1, 1, grid_h, grid_w)
    row = row.reshape(1, 1, grid_h, grid_w)
    grid = np.concatenate((col, row), axis=1)
    stride = np.array([IMG_SIZE//grid_h, IMG_SIZE//grid_w]).reshape(1,2,1,1)

    position = dfl(position)
    box_xy  = grid +0.5 -position[:,0:2,:,:]
    box_xy2 = grid +0.5 +position[:,2:4,:,:]
    xyxy = np.concatenate((box_xy*stride, box_xy2*stride), axis=1)

    return xyxy

def yolov8_post_process(input_data):
    boxes, scores, classes_conf = [], [], []
    defualt_branch=3
    pair_per_branch = len(input_data)//defualt_branch
    # Python 忽略 score_sum 输出
    for i in range(defualt_branch):
        boxes.append(box_process(input_data[pair_per_branch*i]))
        classes_conf.append(input_data[pair_per_branch*i+1])
        scores.append(np.ones_like(input_data[pair_per_branch*i+1][:,:1,:,:], dtype=np.float32))

    def sp_flatten(_in):
        ch = _in.shape[1]
        _in = _in.transpose(0,2,3,1)
        return _in.reshape(-1, ch)

    boxes = [sp_flatten(_v) for _v in boxes]
    classes_conf = [sp_flatten(_v) for _v in classes_conf]
    scores = [sp_flatten(_v) for _v in scores]

    boxes = np.concatenate(boxes)
    classes_conf = np.concatenate(classes_conf)
    scores = np.concatenate(scores)

    # filter according to threshold
    boxes, classes, scores = filter_boxes(boxes, scores, classes_conf)

    # nms
    nboxes, nclasses, nscores = [], [], []
    for c in set(classes):
        inds = np.where(classes == c)
        b = boxes[inds]
        c = classes[inds]
        s = scores[inds]
        keep = nms_boxes(b, s)

        if len(keep) != 0:
            nboxes.append(b[keep])
            nclasses.append(c[keep])
            nscores.append(s[keep])

    if not nclasses and not nscores:
        return None, None, None

    boxes = np.concatenate(nboxes)
    classes = np.concatenate(nclasses)
    scores = np.concatenate(nscores)

    return boxes, classes, scores



def letterbox(im, new_shape=(640, 640), color=(0, 0, 0)):
    shape = im.shape[:2]  # current shape [height, width]
    if isinstance(new_shape, int):
        new_shape = (new_shape, new_shape)

    r = min(new_shape[0] / shape[0], new_shape[1] / shape[1])

    ratio = r, r  # width, height ratios
    new_unpad = int(round(shape[1] * r)), int(round(shape[0] * r))
    dw, dh = new_shape[1] - new_unpad[0], new_shape[0] - \
        new_unpad[1]  # wh padding

    dw /= 2  # divide padding into 2 sides
    dh /= 2

    if shape[::-1] != new_unpad:  # resize
        im = cv2.resize(im, new_unpad, interpolation=cv2.INTER_LINEAR)
    top, bottom = int(round(dh - 0.1)), int(round(dh + 0.1))
    left, right = int(round(dw - 0.1)), int(round(dw + 0.1))
    im = cv2.copyMakeBorder(im, top, bottom, left, right,
                            cv2.BORDER_CONSTANT, value=color)  # add border
    #return im
    return im, ratio, (left, top)

def myFunc(rknn_lite, IMG):
    IMG2 = cv2.cvtColor(IMG, cv2.COLOR_BGR2RGB)
    # 等比例缩放
    IMG2, ratio, padding = letterbox(IMG2)
    # 强制放缩
    # IMG2 = cv2.resize(IMG, (IMG_SIZE, IMG_SIZE))
    IMG2 = np.expand_dims(IMG2, 0)
    
    outputs = rknn_lite.inference(inputs=[IMG2],data_format=['nhwc'])

    #print("oups1",len(outputs))
    #print("oups2",outputs[0].shape)

    boxes, classes, scores = yolov8_post_process(outputs)

    # 绘制椭圆屏蔽区域
    # 绘制小椭圆（内椭圆）- 红色
    cv2.ellipse(IMG, 
               (ELLIPSE_INNER_CENTER_X, ELLIPSE_INNER_CENTER_Y),
               (ELLIPSE_INNER_AXIS_A, ELLIPSE_INNER_AXIS_B),
               0, 0, 360, (0, 0, 255), 2)  # 红色椭圆
    
    # 绘制大椭圆（外椭圆）- 红色
    cv2.ellipse(IMG, 
               (ELLIPSE_OUTER_CENTER_X, ELLIPSE_OUTER_CENTER_Y),
               (ELLIPSE_OUTER_AXIS_A, ELLIPSE_OUTER_AXIS_B),
               0, 0, 360, (0, 0, 255), 2)  # 红色椭圆

    if wp.digitalRead(GPIO_PIN)==wp.HIGH:
        if boxes is not None:
            draw(IMG, boxes, scores, classes, ratio, padding)
    elif wp.digitalRead(GPIO_PIN)==wp.LOW:
        global center_x
        global center_y
        global class_id
        center_x = 640
        center_y = 480
        class_id = 6
        sender(center_x, center_y, class_id)
        print("暂休状态....")

    return IMG
