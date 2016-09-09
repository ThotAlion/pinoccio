import Leap
import time
import serial

ser = serial.Serial(
    port='COM9',
    baudrate=115200,
    write_timeout=0,
    timeout=0)

Hp=0
l = Leap.Controller()
while True:
    #print "debut frame"
    f = l.frame()
    #print "fin frame"
    for h in f.hands:
        if h.is_right:
            H = h.palm_position.y*5
            ser.reset_output_buffer()
            ser.reset_input_buffer()
            # filtrage
            if Hp > 0:
                Hc=int(round(0.01*H+0.99*Hp))
                print Hc
            else:
                Hc=H
            Hp=Hc
            motor = 3
            line = "M"+str(motor)+"P"+str(Hc)+"\n"
            ser.write(line)
            
    time.sleep(0.01)