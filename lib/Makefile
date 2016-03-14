# Makefile template for shared library

CXX = g++
RM = rm -f  # rm command
LFLAGS = -L../../external/jpeg/lib64 -ljpeg
INCS +=-I../../external/jpeg/src
CFLAGS = -Wall -fPIC -O3 -g -m64 $(INCS)

SRCS = image.cpp image_jpeg.cpp stringutils.cpp jpeg_interface.cpp 
OBJS = $(SRCS:.cpp=.o)
LIBJPEG_OBJS = image_jpeg.o jpeg_interface.o 

TESTSRCS = main.cpp image.cpp image_jpeg.cpp stringutils.cpp
TESTOBJS = $(TESTSRCS:.cpp=.o)

.PHONY: all
all: libimgJpeg.so libiview.so

.PHONY: jpeg
jpeg: libimgJpeg.so

.PHONY: lib
lib: libiview.so

.PHONY: test
test: debug.exe

.PHONY: install
install:
	cp *.so ../bin

libimgJpeg.so: ${LIBJPEG_OBJS}
	$(CXX) -shared -Wl,-soname,libimgJpeg.so -I../../external/jpeg/src $(LFLAGS) -o $@ $^

libiview.so: ${OBJS}
	$(CXX) -shared -Wl,-soname,libiview.so -I../../external/jpeg/src $(LFLAGS) -o $@ $^

debug.exe: ${TESTOBJS}
	$(CXX) -I../../external/jpeg/src $(LFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CFLAGS) -c $<

.PHONY: clean
clean:
	-${RM} ${OBJS} *~ *.so *.o debug.exe
