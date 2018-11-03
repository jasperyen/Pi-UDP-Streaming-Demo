
CC = g++
CFLAGS = -std=c++14 -O3 -Wall

# PiCamera
# -lraspicam -lraspicam_cv -lmmal -lmmal_core -lmmal_util -L/opt/vc/lib
# libjpeg-turbo
# -I/opt/libjpeg-turbo/include -L/opt/libjpeg-turbo/lib32

INCLS = -I/opt/libjpeg-turbo/include
PY2INCLS = -I/usr/include/python2.7
PY3INCLS = -I/usr/include/python3.5m

LDLIBS = -pthread -lturbojpeg -L/opt/libjpeg-turbo/lib32
LDCAMLIBS = -lraspicam -lraspicam_cv -lmmal -lmmal_core -lmmal_util -L/opt/vc/lib

PY2SUFFIX = .so
PY3SUFFIX = .cpython-35m-arm-linux-gnueabihf.so
PYLDFLAGS = -fPIC -shared

TARGET = cpp_example py_handler$(PY2SUFFIX) py_handler$(PY3SUFFIX)

PCH_HEADER = header.h
PCH_OUT = header.h.gch

.PHONY : all
all : $(PCH_OUT) $(TARGET)

$(PCH_OUT) : $(PCH_HEADER)
	$(CC) -o $@ $^ $(CFLAGS)


cpp_example : example.o JpegEncoder.o StreamHandler.o
	$(CC) -o $@ $^ $(LDLIBS) $(LDCAMLIBS)

py_handler$(PY2SUFFIX) : PyInterface2.o JpegEncoder.o StreamHandler.o
	$(CC) -o $@ $^ $(LDLIBS) $(PYLDFLAGS)

py_handler$(PY3SUFFIX) : PyInterface3.o JpegEncoder.o StreamHandler.o
	$(CC) -o $@ $^ $(LDLIBS) $(PYLDFLAGS)

PyInterface2.o : PyInterface.cpp PyInterface.h $(PCH_HEADER)
	$(CC) -c $^ $(CFLAGS) $(INCLS) $(PY2INCLS)
	mv PyInterface.o $@

PyInterface3.o : PyInterface.cpp PyInterface.h $(PCH_HEADER)
	$(CC) -c $^ $(CFLAGS) $(INCLS) $(PY3INCLS)
	mv PyInterface.o $@

%.o: %.cpp %.h $(PCH_HEADER)
	$(CC) -c $^ $(CFLAGS) $(INCLS)


.PHONY : clean
clean :
	rm -rf *.o *.gch $(TARGET)$(PYSUFFIX)
