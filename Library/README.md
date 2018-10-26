### 函式庫說明

#### libturbojpeg.so.o
> JPEG 壓縮函式庫  
> 移至 /usr/lib/libturbojpeg.so.0

#### py_stream_handler.cpython-35m-arm-linux-gnueabihf.so
> UDP 影像串流函式庫  
> 移至 python 代碼相同目錄

---

### Python 範例代碼
```
import cv2
import numpy as np
import py_stream_handler

cap = cv2.VideoCapture(0)
width = cap.get(3)
height = cap.get(4)

# StreamHandler( <width>, <height>, <jpeg quality>, <ip>, <port>, <package size> )
sender = py_stream_handler.StreamHandler(width, height, 60, "192.168.1.1", 17788, 65000)

while(True):
    ret, frame = cap.read()
    sender.sendBGRImage(frame)

```
