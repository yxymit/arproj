all: arproj
INCLUDE=-I/Users/yxy/course/6.869/project/aruco-1.2.4/src/
LIB=/usr/local/lib/libopencv_calib3d.dylib /usr/local/lib/libopencv_contrib.dylib /usr/local/lib/libopencv_core.dylib /usr/local/lib/libopencv_features2d.dylib /usr/local/lib/libopencv_flann.dylib /usr/local/lib/libopencv_gpu.dylib /usr/local/lib/libopencv_highgui.dylib /usr/local/lib/libopencv_imgproc.dylib /usr/local/lib/libopencv_legacy.dylib /usr/local/lib/libopencv_ml.dylib /usr/local/lib/libopencv_nonfree.dylib /usr/local/lib/libopencv_objdetect.dylib /usr/local/lib/libopencv_ocl.dylib /usr/local/lib/libopencv_photo.dylib /usr/local/lib/libopencv_stitching.dylib /usr/local/lib/libopencv_superres.dylib /usr/local/lib/libopencv_ts.dylib /usr/local/lib/libopencv_video.dylib /usr/local/lib/libopencv_videostab.dylib /Users/yxy/course/6.869/project/aruco-1.2.4/build/src/libaruco.1.2.4.dylib

arproj: arproj.o
	g++ $(INCLUDE) $(LIB) -o arproj arproj.o

arproj.o: arproj.cpp
	g++ $(INCLUDE) -c -o arproj.o arproj.cpp

clean:
	rm arproj
