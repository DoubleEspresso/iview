# Makefile template for shared library

CXX = g++
RM = rm -f  # rm command
LFLAGS = -L../../external/jpeg/lib64 -ljpeg -L../../external/math/bin64 -lfftw3 -L../../external/tif/lib64 -ltiff
INCS +=-I../../external/jpeg/src -I../../external/math/fftw/api -I../../external/tif/libtiff
CFLAGS = -Wall -fPIC -O3 -m64 $(INCS) # -g for debug

SRCS = image.cpp image_jpeg.cpp stringutils.cpp jpeg_interface.cpp imlib_interface.cpp image_tiff.cpp
SRCSCL = mainCL.cpp image_cl.cpp image_jpeg.cpp stringutils.cpp

OBJS = $(SRCS:.cpp=.o)
OBJSCL = $(SRCSCL:.cpp=.o)

LIBTIFF_OBJS = image_tiff.o tiff_interface.o
LIBJPEG_OBJS = image_jpeg.o jpeg_interface.o 
LIBIMAGE_OBJS = image.o image_jpeg.o imlib_interface.o

TESTSRCS = main.cpp image.cpp image_jpeg.cpp stringutils.cpp image_tiff.cpp
TESTOBJS = $(TESTSRCS:.cpp=.o)

.PHONY: all
all: libimgJpeg.so libimage.so

.PHONY: jpeg
jpeg: libimgJpeg.so

.PHONY: tiff
tiff: libimgTiff.so

.PHONY: lib
lib: libiview.so

.PHONY: test
test: debug.exe

.PHONY: testCL
testCL: debugCL.exe

.PHONY: install
install:
	cp *.so ../bin

#$(CXX) -shared -Wl,-soname,libimgJpeg.so -I../../external/jpeg/src $(LFLAGS) -o $@ $^

libimgJpeg.so: ${LIBJPEG_OBJS}
	$(CXX) $(INCS) -shared -Wl,-soname,libimgJpeg.so -o $@ $^ $(LFLAGS)

libimgTiff.so: ${LIBTIFF_OBJS}
	$(CXX) $(INCS) -shared -Wl,-soname,libimgTiff.so -o $@ $^ $(LFLAGS)

libiview.so: ${OBJS}
	$(CXX) $(INCS) -shared -Wl,-soname,libiview.so -o $@ $^ $(LFLAGS)

libimage.so: ${LIBIMAGE_OBJS}
	$(CXX) $(INCS) -shared -Wl,-soname,libimage.so -o $@ $^ $(LFLAGS)

debug.exe: ${TESTOBJS}
	$(CXX) $(INCS) -o $@ $^ $(LFLAGS)

debugCL.exe: ${OBJSCL}
	$(CXX) $(INCS) -o $@ $^ $(LFLAGS) -lOpenCL

%.o: %.cpp
	$(CXX) $(CFLAGS) -c $<

.PHONY: clean
clean:
	-${RM} ${OBJS} *~ *.so *.o debug.exe debugCL.exe
