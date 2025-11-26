"""
检测处理函数，用于rknnpool
"""
import cv2
import numpy as np
from detect import (
    letterbox, IMG_SIZE, CLASSES, OBJ_THRESH, NMS_THRESH,
    is_point_in_ellipse, ELLIPSE_CENTER_X, ELLIPSE_CENTER_Y,
    ELLIPSE_AXIS_A, ELLIPSE_AXIS_B, sender, filter_boxes, nms_boxes, xywh2xyxy
)


def myFunc(rknn, frame):
    """
    RKNN推理函数，用于rknnpool
    返回处理后的图像和检测结果
    """
    # 图像预处理
    original_shape = frame.shape[:2]  # (height, width)
    img, ratio, (dw, dh) = letterbox(frame, new_shape=(IMG_SIZE, IMG_SIZE))
    img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
    img = np.expand_dims(img, 0)
    
    # RKNN推理
    outputs = rknn.inference(inputs=[img])
    
    # 获取原始输出形状
    input0_shape = outputs[0].shape
    print(f"Debug: 输出形状: {input0_shape}, 总元素数: {outputs[0].size}")
    
    # 根据实际输出形状处理
    # 409600 = 80*80*64，所以可能是 (1, 64, 80, 80) 或 (64, 80, 80)
    if len(input0_shape) == 4:
        # 格式: (batch, channels, height, width)
        batch, channels, height, width = input0_shape
        output_data = outputs[0]
    elif len(input0_shape) == 3:
        # 格式: (channels, height, width)
        channels, height, width = input0_shape
        batch = 1
        output_data = np.expand_dims(outputs[0], 0)
    else:
        raise ValueError(f"不支持的输出维度: {input0_shape}")
    
    # 根据channels数判断格式并处理后处理
    # 64通道可能是YOLOv5格式: 3个anchor * (4坐标 + 1置信度 + 类别数)
    # 64/3 ≈ 21.3，可能是 3*(4+1+16)=63 或 3*(4+1+17)=66
    # 或者可能是其他格式，尝试按3个anchor处理
    
    # 转换为 (height, width, channels) 格式
    output_data = np.transpose(output_data, (0, 2, 3, 1))  # (batch, height, width, channels)
    output_data = output_data.squeeze(0)  # 移除batch维度 -> (height, width, channels)
    
    # 尝试按3个anchor处理，每个anchor约21通道
    # 64 = 3*21 + 1，可能是3个anchor，每个21通道，剩余1个通道
    # 或者可能是其他组合
    
    # 简化处理：假设是YOLOv5格式，3个anchor
    num_anchors = 3
    channels_per_anchor = channels // num_anchors  # 64//3 = 21
    
    if channels % num_anchors != 0:
        # 如果不能整除，尝试其他方式
        # 64可能是 2*32 或 4*16
        if channels == 64:
            # 尝试按2个anchor处理
            num_anchors = 2
            channels_per_anchor = 32
        else:
            num_anchors = 1
            channels_per_anchor = channels
    
    # reshape为 (height, width, anchors, channels_per_anchor)
    output_reshaped = output_data.reshape(height, width, num_anchors, channels_per_anchor)
    
    # 提取box信息: 前4个通道是坐标 (x, y, w, h)
    # 第5个通道是置信度
    # 后面是类别概率
    boxes_xywh = output_reshaped[..., :4]  # (height, width, anchors, 4)
    box_conf = output_reshaped[..., 4:5]  # (height, width, anchors, 1)
    box_cls = output_reshaped[..., 5:]     # (height, width, anchors, num_classes)
    
    # 创建grid坐标
    grid_y, grid_x = np.meshgrid(np.arange(height), np.arange(width), indexing='ij')
    grid = np.stack([grid_x, grid_y], axis=-1)  # (height, width, 2)
    grid = np.expand_dims(grid, axis=2)  # (height, width, 1, 2)
    grid = np.repeat(grid, num_anchors, axis=2)  # (height, width, anchors, 2)
    
    # YOLO格式: box_xy是相对于grid的偏移，需要加上grid坐标
    # 然后乘以stride (IMG_SIZE/height) 得到像素坐标
    boxes_xy = boxes_xywh[..., :2]  # (height, width, anchors, 2)
    boxes_xy = (boxes_xy * 2 - 0.5) + grid  # 转换为绝对坐标
    boxes_xy = boxes_xy * (IMG_SIZE / height)  # 缩放到640x640
    
    boxes_wh = boxes_xywh[..., 2:4]  # (height, width, anchors, 2)
    boxes_wh = (boxes_wh * 2) ** 2  # 转换为实际宽高
    boxes_wh = boxes_wh * (IMG_SIZE / height)  # 缩放到640x640
    
    # 合并为xywh格式
    boxes_xywh = np.concatenate([boxes_xy, boxes_wh], axis=-1)
    
    # 转换为标准格式进行处理
    # 展平为 (N, 4), (N, 1), (N, num_classes)
    boxes_xywh = boxes_xywh.reshape(-1, 4)
    box_conf = box_conf.reshape(-1)
    box_cls = box_cls.reshape(-1, box_cls.shape[-1])
    
    # 应用阈值过滤
    boxes, classes, scores = filter_boxes(boxes_xywh, box_conf, box_cls)
    
    # 转换为xyxy格式
    if boxes is not None and len(boxes) > 0:
        boxes = xywh2xyxy(boxes)
        
        # 过滤无效的box（宽度或高度为0或负数）
        valid_mask = (boxes[:, 2] > boxes[:, 0]) & (boxes[:, 3] > boxes[:, 1])
        boxes = boxes[valid_mask]
        classes = classes[valid_mask]
        scores = scores[valid_mask]
        
        if len(boxes) == 0:
            boxes = None
            classes = None
            scores = None
        else:
            # 将坐标从640x640转换回原始图像尺寸
            # letterbox会添加padding，需要减去padding并缩放
            ratio_w, ratio_h = ratio
            boxes[:, [0, 2]] = (boxes[:, [0, 2]] - dw) / ratio_w  # x坐标
            boxes[:, [1, 3]] = (boxes[:, [1, 3]] - dh) / ratio_h  # y坐标
            
            # 限制在图像范围内
            boxes[:, [0, 2]] = np.clip(boxes[:, [0, 2]], 0, original_shape[1])
            boxes[:, [1, 3]] = np.clip(boxes[:, [1, 3]], 0, original_shape[0])
            
            # 确保类别索引在有效范围内
            num_classes = len(CLASSES)
            classes = np.clip(classes, 0, num_classes - 1)
            
            # NMS
            keep = nms_boxes(boxes, scores)
            if len(keep) > 0:
                boxes = boxes[keep]
                classes = classes[keep]
                scores = scores[keep]
            else:
                boxes = None
                classes = None
                scores = None
    else:
        boxes = None
        classes = None
        scores = None
    
    # 绘制结果
    im_array = frame.copy()
    
    # 绘制椭圆屏蔽区域
    cv2.ellipse(im_array, 
               (ELLIPSE_CENTER_X, ELLIPSE_CENTER_Y),
               (ELLIPSE_AXIS_A, ELLIPSE_AXIS_B),
               0, 0, 360, (0, 0, 255), 2)  # 红色椭圆
    
    # 处理检测结果
    if boxes is not None and len(boxes) > 0:
        # 找到置信度最高的检测结果（不在屏蔽区域内）
        best_idx = None
        best_score = 0
        best_center = None
        best_class = None
        
        for i in range(len(boxes)):
            # boxes格式: [top, left, right, bottom]
            top, left, right, bottom = boxes[i]
            top, left, right, bottom = int(top), int(left), int(right), int(bottom)
            
            # 计算中心点坐标
            ux = int((top + right) / 2)
            uy = int((left + bottom) / 2)
            
            # 检查是否在椭圆屏蔽区域内
            if is_point_in_ellipse(ux, uy, ELLIPSE_CENTER_X, ELLIPSE_CENTER_Y, 
                                  ELLIPSE_AXIS_A, ELLIPSE_AXIS_B):
                # 在屏蔽区域内，用灰色标记
                cv2.rectangle(im_array, (top, left), (right, bottom), (128, 128, 128), 2)
                cv2.circle(im_array, (ux, uy), 4, (128, 128, 128), 5)
                cv2.putText(im_array, 'MASKED', (ux-30, uy-10),
                           cv2.FONT_HERSHEY_SIMPLEX, 0.4, (128, 128, 128), 1)
                continue  # 跳过屏蔽区域内的检测
            
            # 不在屏蔽区域内，正常处理
            
            # 确保类别索引在有效范围内
            class_idx = int(classes[i])
            if class_idx < 0 or class_idx >= len(CLASSES):
                class_name = f"class_{class_idx}"
            else:
                class_name = CLASSES[class_idx]
            
            # 绘制检测框
            cv2.rectangle(im_array, (top, left), (right, bottom), (255, 0, 0), 2)
            cv2.circle(im_array, (ux, uy), 4, (255, 255, 255), 5)  # 标出中心点
            cv2.putText(im_array, '{0} {1:.2f}'.format(class_name, scores[i]),
                       (top, left - 6),
                       cv2.FONT_HERSHEY_SIMPLEX,
                       0.6, (0, 0, 255), 2)
            
            # 选择置信度最高的检测结果发送
            if scores[i] > best_score:
                best_score = scores[i]
                best_idx = i
                best_center = (ux, uy)
                best_class = classes[i]
        
        # 发送置信度最高的检测结果（不在屏蔽区域内）
        if best_center is not None:
            ux, uy = best_center
            # 确保类别索引在有效范围内
            class_idx = int(best_class)
            if class_idx < 0 or class_idx >= len(CLASSES):
                # 如果类别索引无效，使用默认值
                class_idx = 0
            class_id = class_idx + 2  # 转换为ESP32的class_ID: 2,3,4,5
            sender(ux, uy, class_id)
            print(f'Sent: class={class_id}, pos=({ux}, {uy}), score={best_score:.2f}')
        else:
            # 没有有效检测结果，发送class_ID=0表示无垃圾
            sender(0, 0, 0)
    else:
        # 没有检测到任何物体，发送class_ID=0
        sender(0, 0, 0)
    
    return im_array

