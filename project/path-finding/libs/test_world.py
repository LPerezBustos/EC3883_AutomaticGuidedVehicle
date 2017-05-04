import numpy as np
import cv2
from robot import robot
from world import world
cap = cv2.VideoCapture(0)

r = robot()
r.getpos(cap)
r.checkstate()
print r.pos

w = world()
w.start = r.node
w.goal = '12'

w.a_star()

print w.plan

cap.release()

cv2.destroyAllWindows()
