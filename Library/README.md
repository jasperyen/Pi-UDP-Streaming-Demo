### 使用说明

#### libturbojpeg.so.o
> JPEG 压缩库  
> 移至 /usr/lib/libturbojpeg.so.0

#### py_handler.so
> python2 UDP 影像串流库  
> 移至 python 代码相同目录

#### py_handler.cpython-35m-arm-linux-gnueabihf.so
> python3 UDP 影像串流库  
> 移至 python 代码相同目录

---

### Python 范例代码
```
import cv2
import numpy as np
import py_handler

cap = cv2.VideoCapture(0)

width = int(cap.get(3))
height = int(cap.get(4))

# StreamHandler( <width>, <height>, <jpeg quality>, <ip>, <port>, <package size> )
sender = py_handler.PyStreamHandler(width, height, 60, "192.168.3.103", 17788, 65000)
# sender = py_handler.PyStreamHandler(width, height, 60, "192.168.3.103", 17788, "192.168.3.100", 17788, 65000)

while(True):
    ret, frame = cap.read()
    sender.sendBGRImage(frame)
    
```
