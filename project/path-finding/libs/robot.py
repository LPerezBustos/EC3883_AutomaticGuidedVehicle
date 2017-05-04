from collections import deque
import numpy as np
import cv2
import serial

class robot(object):

    def __init__(self):
        self.id = 0xFF
        self.pos = [0,0]
        self.vel = [0,0]
        self.node = '1'
        self.id = 'L'
        # estados = "STOP", "ATRAS","DER", "IZQ", "GO"
        self.cond = 0
        self.state = "STOP"
        self.dt = 0.1
    	self.lb = (10, 100, 100)
    	self.ub = (30, 255, 255)
        
    def getpos(self,camera):
        # obtiene la imagen del espacio de trabajo
        (grabbed, frame) = camera.read()
        frame = cv2.resize(frame, (500, 500))
        ancho = 500
        largo = 500
        #frame = imutils.resize(frame, width=600)
        blurred = cv2.GaussianBlur(frame, (11, 11), 0)
        hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
               
        # calcula la mascara del vehiculo
        mask = cv2.inRange(hsv, self.lb, self.ub)
        mask = cv2.erode(mask, None, iterations=2)
        mask = cv2.dilate(mask, None, iterations=2)

        res = cv2.bitwise_and(frame,frame,mask = mask)

        # obtiene los contornos de la mascara
        cnts = cv2.findContours(mask.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)[-2]
        x = 0
        y = 0
        # calcula el centro del circulo
        if len(cnts) > 0:
            # halla el maximo circulo en la mascara
            c = max(cnts, key=cv2.contourArea)
            ((x, y), radius) = cv2.minEnclosingCircle(c)
        
        # calcula la velocidad del vehiculo
        dx = x - self.pos[0]
        dy = y - self.pos[1]
        self.vel = [dx/self.dt, dy/self.dt]

        # actualiza la posicion del vehiculo
        self.pos = [x, y]

        self.node = 0

        #NODO 1
        if (self.pos[0] >= ((0*ancho)/4)) and (self.pos[0] <= ((1*ancho)/4)) and (self.pos[1] >= ((0*largo)/3)) and (self.pos[1] <= ((1*largo)/3)) :
            self.node = "1"

         #NODO 8
        if (self.pos[0] >= ((0*ancho)/4)) and (self.pos[0] <= ((1*ancho)/4)) and (self.pos[1] >= ((1*largo)/3)) and (self.pos[1] <= ((2*largo)/3)) :
            self.node = "8"

         #NODO 9
        if (self.pos[0] >= ((0*ancho)/4)) and (self.pos[0] <= ((1*ancho)/4)) and (self.pos[1] >= ((2*largo)/3)) and (self.pos[1] <= ((3*largo)/3)) :
            self.node = "9"

         #NODO 2
        if (self.pos[0] >= ((1*ancho)/4)) and (self.pos[0] <= ((2*ancho)/4)) and (self.pos[1] >= ((0*largo)/3)) and (self.pos[1] <= ((1*largo)/3)) :
            self.node = "2"

         #NODO 7
        if (self.pos[0] >= ((1*ancho)/4)) and (self.pos[0] <= ((2*ancho)/4)) and (self.pos[1] >= ((1*largo)/3)) and (self.pos[1] <= ((2*largo)/3)) :
            self.node = "7"

         #NODO 10
        if (self.pos[0] >= ((1*ancho)/4)) and (self.pos[0] <= ((2*ancho)/4)) and (self.pos[1] >= ((2*largo)/3)) and (self.pos[1] <= ((3*largo)/3)) :
            self.node = "10"

        #NODO 3
        if (self.pos[0] >= ((2*ancho)/4)) and (self.pos[0] <= ((3*ancho)/4)) and (self.pos[1] >= ((0*largo)/3)) and (self.pos[1] <= ((1*largo)/3)) :
            self.node = "3"

         #NODO 6
        if (self.pos[0] >= ((2*ancho)/4)) and (self.pos[0] <= ((3*ancho)/4)) and (self.pos[1] >= ((1*largo)/3)) and (self.pos[1] <= ((2*largo)/3)) :
            self.node = "6"

         #NODO 11
        if (self.pos[0] >= ((2*ancho)/4)) and (self.pos[0] <= ((3*ancho)/4)) and (self.pos[1] >= ((2*largo)/3)) and (self.pos[1] <= ((3*largo)/3)) :
            self.node = "11"
            
         #NODO 4
        if (self.pos[0] >= ((3*ancho)/4)) and (self.pos[0] <= ((4*ancho)/4)) and (self.pos[1] >= ((0*largo)/3)) and (self.pos[1] <= ((1*largo)/3)) :
            self.node = "4"

         #NODO 5
        if (self.pos[0] >= ((3*ancho)/4)) and (self.pos[0] <= ((4*ancho)/4)) and (self.pos[1] >= ((1*largo)/3)) and (self.pos[1] <= ((2*largo)/3)) :
            self.node = "5"

         #NODO 12
        if (self.pos[0] >= ((3*ancho)/4)) and (self.pos[0] <= ((4*ancho)/4)) and (self.pos[1] >= ((2*largo)/3)) and (self.pos[1] <= ((3*largo)/3)) :
            self.node = "12"

        #Show frame
        cv2.rectangle(res,(((0*ancho)/4),((0*largo)/3)),(((1*ancho)/4),((1*largo)/3)),(0,255,0),2)
        cv2.rectangle(res,(((1*ancho)/4),((0*largo)/3)),(((2*ancho)/4),((1*largo)/3)),(0,255,0),2)
        cv2.rectangle(res,(((2*ancho)/4),((0*largo)/3)),(((3*ancho)/4),((1*largo)/3)),(0,255,0),2)
        cv2.rectangle(res,(((3*ancho)/4),((0*largo)/3)),(((4*ancho)/4),((1*largo)/3)),(0,255,0),2)
        cv2.rectangle(res,(((0*ancho)/4),((1*largo)/3)),(((1*ancho)/4),((2*largo)/3)),(0,255,0),2)
        cv2.rectangle(res,(((1*ancho)/4),((1*largo)/3)),(((2*ancho)/4),((2*largo)/3)),(0,255,0),2)
        cv2.rectangle(res,(((2*ancho)/4),((1*largo)/3)),(((3*ancho)/4),((2*largo)/3)),(0,255,0),2)
        cv2.rectangle(res,(((3*ancho)/4),((1*largo)/3)),(((4*ancho)/4),((2*largo)/3)),(0,255,0),2)
        cv2.rectangle(res,(((0*ancho)/4),((2*largo)/3)),(((1*ancho)/4),((3*largo)/3)),(0,255,0),2)
        cv2.rectangle(res,(((1*ancho)/4),((2*largo)/3)),(((2*ancho)/4),((3*largo)/3)),(0,255,0),2)
        cv2.rectangle(res,(((2*ancho)/4),((2*largo)/3)),(((3*ancho)/4),((3*largo)/3)),(0,255,0),2)
        cv2.rectangle(res,(((3*ancho)/4),((2*largo)/3)),(((4*ancho)/4),((3*largo)/3)),(0,255,0),2)

        #Show frame
        cv2.imshow('res',res)
       
    def checkstate(self, next_node, goal):
        # detecta la accion a tomar
        if self.pos == [0,0]:
            self.state = "STOP"
        elif self.node == goal:
            self.state = "STOP"
        elif self.node == "1":
            if next_node == "2":
                self.state = "GO"
            elif next_node == "8":
                self.state = "DER"
        elif self.node == "2":
            if next_node == "1":
                self.state = "ATRAS"
            elif next_node == "7":
                self.state = "DER"
            elif next_node == "3":
                self.state = "GO"
        elif self.node == "3":
            if next_node == "2":
                self.state = "ATRAS"
            elif next_node == "6":
                self.state = "DER"
            elif next_node == "4":
                self.state = "GO"
        elif self.node == "4":
            if next_node == "5":
                self.state = "DER"
            elif next_node == "3": 
                self.state = "ATRAS"
        elif self.node == "5":
            if next_node == "4":
                self.state = "IZQ"
            elif next_node == "6":
                self.state = "ATRAS"
            elif next_node == "12":
                self.state = "DER"
        elif self.node == "6":
            if next_node == "5":
                self.state = "GO"
            elif next_node == "3":
                self.state = "IZQ"
            elif next_node == "11":
                self.state = "DER"
            elif next_node == "7":
                self.state = "ATRAS"
        elif self.node == "7":
            if next_node == "6":
                self.state = "GO"
            elif next_node == "2":
                self.state = "IZQ"
            elif next_node == "10":
                self.state = "DER"
            elif next_node == "8":
                self.state = "ATRAS"
        elif self.node == "8":
            if next_node == "7":
                self.state = "GO"
            elif next_node == "1":
                self.state = "IZQ"
            elif next_node == "9":
                self.state = "DER"
        elif self.node == "9":
            if next_node == "10":
                self.state = "GO"
            elif next_node == "8":
                self.state = "IZQ"
        elif self.node == "10":
            if next_node == "11":
                self.state = "GO"
            elif next_node == "7":
                self.state = "IZQ"
            elif next_node == "9":
                self.state = "ATRAS"
        elif self.node == "11":
            if next_node == "12":
                self.state = "GO"
            elif next_node == "6":
                self.state = "IZQ"
            elif next_node == "10":
                self.state = "ATRAS"
        elif self.node == "12":
            if next_node == "5":
                self.state = "IZQ"
            elif next_node == "11":
                self.state = "ATRAS"
	
    def send_msg(self, port):
        # chequea la tasa de baudios

        if self.state == "STOP":
            msg = "S"
        elif self.state == "ATRAS":
            msg = "A"
        elif self.state == "DER":
            msg = "D"
        elif self.state == "IZQ":
            msg = "I"
        elif self.state == "GO":
            msg = "G"
        else:
            msg = "S"

        # incializa la conexion
        if self.cond == 0:
            # envia la se\nal de sincronizacion
            port.write("A")
            if port.in_waiting > 0:
                # espera el acknowledge
                ack = port.read();
                print "Recibi respuesta " + ack
                if ack == "C":
                    port.write(self.id)
                    port.write(msg)
                    print "ACK"                
                    print msg
                    self.cond = 1
            else:
                # no se detecta el acknowledge
                print "no conectado"
        else:
            # envia el mensaje por la conexion inicializada
            port.write(self.id)
            port.write(msg)
            port.flush()

            msg_rec = port.read()
            msg_rec2 = port.read()
            print "conectado"

            print msg_rec + msg_rec2