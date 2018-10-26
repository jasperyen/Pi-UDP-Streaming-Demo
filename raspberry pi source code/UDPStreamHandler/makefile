all : run.o

CC = g++
CFLAGS = -std=c++11 -pthread `pkg-config --cflags --libs opencv`
PICAMFLAGS = -L/opt/vc/lib -I/usr/local/include/ -lraspicam -lraspicam_cv -lmmal -lmmal_core -lmmal_util
JPEGFLAGS = -L/opt/libjpeg-turbo/lib32 -I/opt/libjpeg-turbo/include/ -lturbojpeg

run.o : main.o JpegEncoder.o StreamHandler.o
	$(CC) -o $@ $^ $(CFLAGS) $(PICAMFLAGS) $(JPEGFLAGS)

main.o : main.cpp
	$(CC) -c $^ $(CFLAGS) $(PICAMFLAGS) $(JPEGFLAGS)

JpegEncoder.o : JpegEncoder.cpp JpegEncoder.h
	$(CC) -c $^ $(CFLAGS) $(JPEGFLAGS)

StreamHandler.o : StreamHandler.cpp StreamHandler.h
	$(CC) -c $^ $(CFLAGS) $(JPEGFLAGS)

clean :
	rm -rf *.o *.gch
