#备注：25工创赛 验证坐标到角度转换脚本 
import math
import time as t
import turtle as draw

pos_x_centre = 160
pos_y_centre = 120
rouph_x = int(pos_x_centre+math.sqrt(300))
rouph_y = pos_y_centre
flag = 1

draw.setup(width=320, height=240)
draw.setworldcoordinates(0, 0, 320, 240)
pen = draw.Turtle()
pen.speed(0)
pen.penup()
pen.goto(160, 120)
pen.pendown()

def draw_circle():
    pen.goto(int(rouph_x), int(rouph_y))

def angle_calc(pos_x, pos_y):
    angle = 0
    distance = math.sqrt((pos_x - pos_x_centre) ** 2 + (pos_y - pos_y_centre) ** 2)
    if ((pos_x - pos_x_centre > 0) and (pos_y - pos_y_centre < 0)) or ((pos_x - pos_x_centre <= 0) and (pos_y - pos_y_centre < 0)):
        angle = math.acos((pos_x - pos_x_centre) / distance)
    elif ((pos_x - pos_x_centre >= 0) and (pos_y - pos_y_centre > 0)) or ((pos_x - pos_x_centre < 0) and (pos_y - pos_y_centre > 0)):
        angle = math.acos((-(pos_x - pos_x_centre) / distance))+math.pi
    return angle * 180 / math.pi

def output_angle_calc():
    global rouph_x
    global rouph_y
    theta=0
    global flag
    while True:
        if (flag == 1):
            theta = angle_calc(rouph_x, rouph_y)
            if (abs(rouph_x - pos_x_centre) < math.sqrt(300)):
                rouph_x = rouph_x - 1
            if (abs(rouph_x - pos_x_centre) > math.sqrt(300)):
                rouph_x = pos_x_centre-math.sqrt(300)
                flag = 2
            rouph_y = 120 - math.sqrt(300 - pow(rouph_x - 160, 2))
            t.sleep(0.5)

        if (flag == 2):
            theta = angle_calc(rouph_x, rouph_y)
            if (abs(rouph_x - pos_x_centre) < math.sqrt(300)):
                rouph_x = rouph_x + 1
            if (abs(rouph_x - pos_x_centre) > math.sqrt(300)):
                rouph_x = pos_x_centre+math.sqrt(300)
                flag = 1
            rouph_y = 120 + math.sqrt(300 - pow(rouph_x - 160, 2))
            t.sleep(0.5)

        draw_circle()
        print("theta: %f\n" % theta)
        print("rouph_x: %f\n" % int(rouph_x))
        print("rouph_y: %f\n" % int(rouph_y))
        print("----------------------\n")

if __name__ == '__main__':
    output_angle_calc()
