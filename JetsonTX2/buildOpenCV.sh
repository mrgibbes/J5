#!/bin/bash
# License: MIT. See license file in root directory
# Copyright(c) JetsonHacks (2017)
cd $HOME
sudo apt-get install -y \
    libglew-dev \
    libtiff5-dev \
    zlib1g-dev \
    libjpeg-dev \
    libpng12-dev \
    libjasper-dev \
    libavcodec-dev \
    libavformat-dev \
    libavutil-dev \
    libpostproc-dev \
    libswscale-dev \
    libeigen3-dev \
    libtbb-dev \
    libgtk2.0-dev \
    cmake \
    pkg-config

#MrGibbes
#libgstreamer-plugins-good1.0-dev
#libgstreamer-plugins-good1.0-0
#libgstreamer-plugins-base1.0-dev
#libgstreamer-plugins-base1.0-0
#libgstreamer-plugins-base0.10-dev
#libgstreamer-plugins-base0.10-0
#libgstreamer0.10-dev
#libgstreamer0.10-0
#libgstreamer1.0-dev
#libgstreamer1.0-0

#gir1.2-gst-plugins-base-0.10
#gir1.2-gstreamer-0.10

#-DWITH_OPENGL=ON \

# Python 2.7
sudo apt-get install -y python-dev python-numpy python-py python-pytest -y
# GStreamer support

#MrGibbes - Was sudo apt-get install -y libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev
sudo apt-get install -y libgstreamer1.0-dev libgstreamer-plugins-good1.0-dev 

git clone https://github.com/opencv/opencv.git
cd opencv
git checkout -b v3.3.0 3.3.0
# This is for the test data
cd $HOME
git clone https://github.com/opencv/opencv_extra.git
cd opencv_extra
git checkout -b v3.3.0 3.3.0

cd $HOME/opencv
mkdir build
cd build
# Jetson TX2 
cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr \
    -DBUILD_PNG=ON \
    -DBUILD_TIFF=ON \
    -DBUILD_TBB=OFF \
    -DBUILD_JPEG=ON \
    -DBUILD_JASPER=OFF \
    -DBUILD_ZLIB=OFF \
    -DBUILD_EXAMPLES=OFF \
    -DBUILD_opencv_java=OFF \
    -DBUILD_opencv_python2=OFF \
    -DBUILD_opencv_python3=ON \
    -DENABLE_PRECOMPILED_HEADERS=OFF \
    -DWITH_OPENGL=ON \
    -DWITH_OPENCL=OFF \
    -DWITH_OPENMP=OFF \
    -DWITH_FFMPEG=ON \
    -DWITH_GSTREAMER=ON \
    -DWITH_GSTREAMER_0_10=ON \
    -DWITH_CUDA=ON \
    -DWITH_GTK=ON \
    -DWITH_VTK=OFF \
    -DWITH_TBB=ON \
    -DWITH_1394=OFF \
    -DWITH_OPENEXR=OFF \
    -DCUDA_TOOLKIT_ROOT_DIR=/usr/local/cuda-8.0 \
    -DCUDA_ARCH_BIN=6.2 \
    -DCUDA_ARCH_PTX="" \
    -DINSTALL_C_EXAMPLES=ON \
    -DINSTALL_TESTS=ON \
    -DOPENCV_TEST_DATA_PATH=../opencv_extra/testdata \
    ../

# Consider using all 6 cores; $ sudo nvpmodel -m 2 or $ sudo nvpmodel -m 0
make -j4
