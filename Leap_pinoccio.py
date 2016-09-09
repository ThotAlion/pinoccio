import Leap
import time
import serial

ser = serial.Serial(
    port='COM9',
    baudrate=115200,
    timeout=0)

motor = 3
l = Leap.Controller()

while True:
    #print "debut frame"
    f = l.frame()
    #print "fin frame"
    for h in f.hands:
        if h.is_left:
            H = int(h.palm_position.y)
            line = "M"+str(motor)+"P"+str(H)+"\n"
            #print "debut write"
            ser.write(line)
            #print "fin write"
            #print line
            
    time.sleep(0.02)