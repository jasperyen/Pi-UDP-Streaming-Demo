
CC = g++
CFLAGS = -std=c++14 -O3 -Wall

# PiCamera
# -lraspicam -lraspicam_cv -lmmal -lmmal_core -lmmal_util -L/opt/vc/lib
# libjpeg-turbo
# -I/opt/libjpeg-turbo/include -L/opt/libjpeg-turbo/lib32

#INCLS = -I/opt/libjpeg-turbo/include `python3 -m pybind11 --includes`
INCLS = -I/opt/libjpeg-turbo/include -I/usr/include/python3.5m

LDLIBS = -pthread -lturbojpeg -L/opt/libjpeg-turbo/lib32
LDCAMLIBS = -lraspicam -lraspicam_cv -lmmal -lmmal_core -lmmal_util -L/opt/vc/lib

PYSUFFIX = `python3-config --extension-suffix`
PYLDFLAGS = -fPIC -shared

TARGET = cpp_example py_handler

PCH_HEADER = header.h
PCH_OUT = header.h.gch

.PHONY : all
all : $(PCH_OUT) $(TARGET)

$(PCH_OUT) : $(PCH_HEADER)
	$(CC) -o $@ $^ $(CFLAGS)


cpp_example : example.o JpegEncoder.o StreamHandler.o
	$(CC) -o $@ $^ $(LDLIBS) $(LDCAMLIBS)

py_handler : PyInterface.o JpegEncoder.o StreamHandler.o
	$(CC) -o $@$(PYSUFFIX) $^ $(LDLIBS) $(PYLDFLAGS)


%.o: %.cpp %.h $(PCH_HEADER)
	$(CC) -c $^ $(CFLAGS) $(INCLS)


.PHONY : clean
clean :
	rm -rf *.o *.gch $(TARGET)$(PYSUFFIX)
