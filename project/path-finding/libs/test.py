import numpy as np
import cv2
from robot import robot
from world import world
import serial

cap = cv2.VideoCapture(0)

w = world()
r = robot()

w.goal = '6'

ser = serial.Serial()
ser.baudrate = 9600
ser.port = '/dev/cu.usbserial-FT3W5E11'
#ser.port = 'COM5'
ser.open()

while(1):
    r.getpos(cap)
    #print r.pos
    w.start = r.node
    w.a_star()

    print w.plan
    if w.plan[0] == w.goal:
    	nodo_next = w.goal
    else:
    	nodo_next = w.plan[1]
    #print nodo_next
    
    r.checkstate(nodo_next, w.goal)
    r.send_msg(ser)

    print r.state

    key = cv2.waitKey(1) & 0xFF

    # detenerse al presionar q
    if key == ord("q"):
        break
    
ser.close()
cap.release()

cv2.destroyAllWindows()
