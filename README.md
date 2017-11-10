# J5
Johnny 5 - Testbuild

The purpose of this Git repository is to sandbox code while developing. The goal is to create easy to use Libraries that can be used in conjuntion with the Robot Operating System (ROS) for robotics development and education.

I will attempt to create the Libraries in C or C++ and port the code to the Jetson, PI, and Arduino platforms where possible.

Current Subdirectories include:

PI (currently tested from version 2 up to 3b)
Jetson (written for the TX2 but should be compatible with the TX1.  TK1 versions will require a gpio library substitution)

Current goals.

- Working TX1 and TX2 GPIO Library. - Done Thanks Jetson Hacks
- Working UART Library for Infered Time of flight and Sonar Sensors. - Done Thanks ROS
- Working I2C Library for LIDAR sensors. - Done Thanks Jetson Hacks

- Recompile LSD_SLAM to work with the Jetson TX2 development board. NOT Done Thanks God
- Recompile OSD_SLAM_GPU on the Jetson TX2 development board.  https://github.com/yunchih/ORB-SLAM2-GPU2016-final
