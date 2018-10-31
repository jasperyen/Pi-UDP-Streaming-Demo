## 环境说明

#### 相依库
> OpenCV-3.4.3 [https://github.com/opencv/opencv](https://github.com/opencv/opencv)
>
> raspicam [https://github.com/cedricve/raspicam](https://github.com/cedricve/raspicam)
>
> libjpeg-turbo-1.5.3 [https://github.com/libjpeg-turbo/libjpeg-turbo](https://github.com/libjpeg-turbo/libjpeg-turbo)
>
> pybind11-2.2.4 [https://github.com/pybind/pybind11](https://github.com/pybind/pybind11)

#### Dockerfile
包含所有编译所需环境 :
[Dockerfile](https://github.com/jasperyen/Pi-UDP-Streaming-Demo/blob/master/raspberry%20pi%20source%20code/docker/Dockerfile)

```
docker run -it --device=/dev/vchiq jasper1996826/turbojpeg-pybind11:python3.5
```

