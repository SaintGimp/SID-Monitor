import serial
import re
import matplotlib.pyplot as plt
import numpy as np

bufferSize = 512
frequencyRange = 50000

def GetData():
    dataSet = [0] * (bufferSize + 1)
    line = ''

    while not (re.search('begin', line)): 
        line = serial.readline().decode('utf-8')

    line = serial.readline().decode('utf-8')

    while not (re.search('end', line)): 
        dataMatch = re.search(r'\[(\d+)\].* - (\d+\.?\d+)', line)
        if dataMatch:
            index = int(dataMatch.group(1))
            data = float(dataMatch.group(2))
            dataSet[index] = data
            # if (data > 300):
            #     print(index)
        line = serial.readline().decode('utf-8')

    return np.array(dataSet)

serial = serial.Serial('/dev/cu.usbmodem14201')

plt.ion()
fig = plt.figure()
ax = fig.add_subplot(111)
ax.set_xlim(0, frequencyRange)
ax.set_ylim(0, 2048)

xData = np.linspace(0, frequencyRange, bufferSize + 1)
line1, = ax.plot(xData, GetData(), "r-") # Returns a tuple of line objects, thus the comma

plt.show(block=False)
plt.draw()

while True:
    line1.set_data(xData, GetData())
    plt.pause(0.001)

#print(*a, sep = "\n")