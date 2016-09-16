import csv
import sys
from PyQt4.QtGui import *
from PyQt4.QtCore import *
import serial

app = QApplication(sys.argv)

ser = serial.Serial(
    port='COM9',
    baudrate=115200,
    write_timeout=0,
    timeout=0)

# main window
mainW = QWidget()
mainLay = QHBoxLayout(mainW)

# file table

tapeTable = QTableView()
tapeDir = QFileSystemModel()
tapeDir.setRootPath(QDir.currentPath()+'/POSES/')
tapeTable.setModel(tapeDir)
tapeTable.setRootIndex(tapeDir.index(QDir.currentPath()+'/POSES/'))
tapeTable.horizontalHeader().setResizeMode(QHeaderView.ResizeToContents)
mainLay.addWidget(tapeTable)

#scroll bars
sliderList = []
for i in range(10):
    s = QSlider()
    s.setMinimum(0)
    s.setMaximum(10000)
    sliderList.append(s)
    mainLay.addWidget(s)

# save button
saveLay = QVBoxLayout()
saveButton = QPushButton("Save pose")
saveName = QLineEdit()
saveLay.addWidget(saveButton)
saveLay.addWidget(saveName)
mainLay.addLayout(saveLay)

# slots
def loadPose(i):
    fileName = tapeDir.filePath(i)
    r = csv.reader(open(fileName,'r'),delimiter = ';')
    a = r.next()
    saveName.setText(tapeDir.fileName(i))
    for i in range(10):
        sliderList[i].setValue(int(a[i]))
    update_pinoccio()
        
def update_pinoccio():
    ser.reset_output_buffer()
    ser.reset_input_buffer()
    for i in range(10):
        line = "M"+str(i+1)+"P"+str(sliderList[i].value())+"\n"
        print line
        ser.write(line)
        
def savePose():
    fileName = QDir.currentPath()+"/POSES/"+saveName.text()
    f = open(fileName,'w')
    r = csv.writer(f,delimiter = ';')
    line = []
    for i in range(10):
        line.append(sliderList[i].value())
    r.writerow(line)
    f.close()
    
# signals
mainW.connect(tapeTable,SIGNAL("clicked(QModelIndex)"),loadPose)
mainW.connect(saveButton,SIGNAL("clicked()"),savePose)
for i in range(10):
    mainW.connect(sliderList[i],SIGNAL("sliderReleased()"),update_pinoccio)

mainW.show()

sys.exit(app.exec_())