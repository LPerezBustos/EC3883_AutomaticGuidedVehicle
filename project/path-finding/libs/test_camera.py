import numpy as np
import cv2
from robot import robot
cap = cv2.VideoCapture(0)

while(1):
    r = robot()
    r.getpos(cap)
    r.pos
    print r.pos

    key = cv2.waitKey(1) & 0xFF

    # detenerse al presionar q
    if key == ord("q"):
        break
    
cap.release()

cv2.destroyAllWindows()
