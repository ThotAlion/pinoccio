import Leap
import time
import serial
import numpy

ser = serial.Serial(
    port='COM9',
    baudrate=115200,
    write_timeout=0,
    timeout=0)

    
A1 = numpy.array([510,-790,0])
A2 = numpy.array([560, -90,0])
A3 = numpy.array([130, -70,0])
b1 = 500
b2 = 500
b3 = 500
L1p=0
L2p=0
L3p=0

K = 2

l = Leap.Controller()
while True:
    #print "debut frame"
    f = l.frame()
    #print "fin frame"
    for h in f.hands:
        if h.is_right:
            M = numpy.array([-h.palm_position.z+300,-h.palm_position.x,h.palm_position.y-1200])
            print M
            # lenght computation
            L1 = numpy.linalg.norm(M-A1)-b1
            L2 = numpy.linalg.norm(M-A2)-b2
            L3 = numpy.linalg.norm(M-A3)-b3
            if L1<0:
                L1=0
            if L2<0:
                L2=0
            if L3<0:
                L3=0
            print L1,L2,L3
            ser.reset_output_buffer()
            ser.reset_input_buffer()
            
            # filtrage
            if L1p > 0:
                L1c=0.01*L1+0.99*L1p
            else:
                L1c=L1
            L1p=L1c
            line = "M3P"+str(int(round(K*L1c)))+"\n"
            ser.write(line)
            
            if L2p > 0:
                L2c=0.01*L2+0.99*L2p
            else:
                L2c=L2
            L2p=L2c
            line = "M5P"+str(int(round(K*L2c)))+"\n"
            ser.write(line)
            
            if L3p > 0:
                L3c=0.01*L3+0.99*L3p
            else:
                L3c=L3
            L3p=L3c
            line = "M4P"+str(int(round(K*L3c)))+"\n"
            ser.write(line)
            
    time.sleep(0.01)