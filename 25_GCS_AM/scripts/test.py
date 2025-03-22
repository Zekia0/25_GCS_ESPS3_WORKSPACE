import math

poscenterx = 160
poscentery = 120
def angle_change(posx,posy):
    deltax = posx - poscenterx
    deltay = poscentery - posy
    angle = math.atan2(deltay, deltax)
    angle = angle * 180 / math.pi; 
    return angle

if __name__ == '__main__':
    print(angle_change(90,121))