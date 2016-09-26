import Leap
import time
import serial
import numpy

mock = False

if mock == False:
    ser = serial.Serial(
        port='COM9',
        baudrate=115200,
        write_timeout=0,
        timeout=0)
    
    # init platform
    ser.reset_output_buffer()
    ser.reset_input_buffer()
    for i in range(10):
        line = "M"+str(i+1)+"P-1\n"
        ser.write(line)
    time.sleep(1)
    
    ser.reset_output_buffer()
    ser.reset_input_buffer()
    for i in range(10):
        line = "M"+str(i+1)+"P0\n"
        ser.write(line)
        time.sleep(1)
    print "Platform initialized !"
    time.sleep(1)

# right
A10 = numpy.array([50,-50,0])
A6 = numpy.array([600, -805,0])
A4 = numpy.array([600, 475,0])

# left
A5 = numpy.array([600,805,0])
A7 = numpy.array([600, -475,0])
A2 = numpy.array([50, 50,0])

b10 = 400
b6 = 600
b4 = 700
b5 = 530
b7 = 900
b2 = 450
L10p=0
L6p=0
L4p=0
L5p=0
L7p=0
L2p=0

L10c=0
L6c=0
L4c=0
L5c=0
L7c=0
L2c=0
# pulley gain in step/mm
K = 4*200/(numpy.pi*10)
a = 1
b = 1-a

l = Leap.Controller()
while True:
    #print "debut frame"
    f = l.frame()
    #print "fin frame"
    if mock == False:
        ser.reset_output_buffer()
        ser.reset_input_buffer()
    for h in f.hands:
        if h.is_right:
            MR = numpy.array([-h.palm_position.z+300,-1.5*h.palm_position.x,h.palm_position.y-800])
            # length computation
            L10 = numpy.linalg.norm(MR-A10)-b10
            L6 = numpy.linalg.norm(MR-A6)-b6
            L4 = numpy.linalg.norm(MR-A4)-b4
            if L10<0:
                L10=0
            if L6<0:
                L6=0
            if L4<0:
                L4=0
            
            # filtrage
            if abs(L10p-L10) > 5:
                L10c=L10
            L10p=L10c
            line = "M10P"+str(int(round(K*L10c)))+"\n"
            if mock == False:
                ser.write(line)
            
            if abs(L6p-L6) > 5:
                L6c=L6
            L6p=L6c
            line = "M6P"+str(int(round(K*L6c)))+"\n"
            if mock == False:
                ser.write(line)
            
            if abs(L4p-L4) > 5:
                L4c=L4
            L4p=L4c
            line = "M4P"+str(int(round(K*L4c)))+"\n"
            if mock == False:
                ser.write(line)
            
        if h.is_left:
            ML = numpy.array([-h.palm_position.z+300,-1.5*h.palm_position.x,h.palm_position.y-800])
            # length computation
            L5 = numpy.linalg.norm(ML-A5)-b5
            L7 = numpy.linalg.norm(ML-A7)-b7
            L2 = numpy.linalg.norm(ML-A2)-b2
            if L5<0:
                L5=0
            if L7<0:
                L7=0
            if L2<0:
                L2=0
            
            
            # filtrage
            
            if abs(L5p-L5) > 5:
                L5c=L5
            L5p=L5c
            line = "M5P"+str(int(round(K*L5c)))+"\n"
            if mock == False:
                ser.write(line)
            
            if abs(L7p-L7) > 5:
                L7c=L7
            L7p=L7c
            line = "M7P"+str(int(round(K*L7c)))+"\n"
            if mock == False:
                ser.write(line)
            
            if abs(L2p-L2) > 5:
                L2c=L2
            L2p=L2c
            line = "M2P"+str(int(round(K*L2c)))+"\n"
            if mock == False:
                ser.write(line)    
            
    print "R:",L10c,L6c,L4c
    print "L:",L5c,L7c,L2c
    if mock == False:
                t = time.time()
                line = "M8P"+str(int(round(2000*numpy.sin(t)+3000)))+"\n"
                ser.write(line)
                line = "M3P"+str(int(round(2000*numpy.sin(t+numpy.pi)+3000)))+"\n"
                ser.write(line)
    time.sleep(0.1)