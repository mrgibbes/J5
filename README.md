# J5
Johny 5 - Testbuild

The purpose of this Git repository is to sandbox code while developing. The goal is to create easy to use Libraries that can be used in conjuntion with the Robot Operating System (ROS) for robotics development and education.

I will attempt to create the Libraries in C or C++ and port the code to the Jetson, PI, and Arduino platforms where possible.

Current Subdirectories include:

PI (currently tested from version 2 up to 3b)
Jetson (written for the TX2 but should be compatible with the TX1.  TK1 versions will require a gpio library substitution)

Current goals.

- Rebuild and document build process for working ORB/LSD SLAM with GPU support using newer OpenCV (3.3+) on ROS Kinetic, Ubuntu Xenial
- Attempt and document build process for working ORB/LSD SLAM with GPU support using newer OpenCV4Tegra (NVidia maintained port for Jetson series boards)
- Test and tune performance of SLAM alternatives.
- Compile, build, and document working Hough Tranform algorithm layerd on OpenCV and gstreamer for raw camera input.
- Document Ultimate GPS breakout on the TX2 over UART using gpsd and cgps.
- Create MPI communication framework for Raspberry PI Stepper Motor Driver controller interface. (Allows me to offload Stepper Controller work to the PI)
- Make binarary C interface for GPS.

Completed goals.

- Working TX1 and TX2 GPIO Library. - Done Thanks Jetson Hacks
- Working UART Library for Infered Time of flight and Sonar Sensors. - Done Thanks ROS
- Working I2C Library for LIDAR sensors. - Done Thanks Jetson Hacks
- Working GPS breakout interface. - Done Thanks ???

- Recompile LSD_SLAM to work with the Jetson TX2 development board. Done Thanks https://github.com/kevin-george/lsd_slam
- Recompile ORB_SLAM_GPU on the Jetson TX2 development board. - Done Thanks https://github.com/yunchih/ORB-SLAM2-GPU2016-final
