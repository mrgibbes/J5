# JHLidarLite_V3_TX2
JHLidarLite_V3_TX2 - Testbuild

Scott Logan Smith
mrgibbes@gmail.com
Modified to work on the Jetson TX2 with the new (GARMIN) Lidar Lite V3

Forked from -> Jetson Hacks JHLidarLite_v2 https://github.com/jetsonhacks/JHLidarLite_V2.git

Interface for PulsedLight, Inc Lidar-Lite V3 Garmin to NVIDIA Jetson TX2 Development Kit over I2C.

http://pulsedlight3d.com

Requires:

$ sudo apt-get install libi2c-dev i2c-tools

The Lidar-Lite appears as 0x62 on i2c bus 0 on the Jetson TX2 with this wiring:

<blockquote><p>
VCC J21-2 ->   Lidar-Lite (+)<br>
GND J21-6 ->   Lidar-Lite (-)<br>
SCL J21-27 ->  Lidar-Lite (SCL)<br>
SDA J21-28 ->  Lidar-Lite (SDA)</p></blockquote>

//Not sure if true on high current 5v supplies... - mrgibbes
Lidar-Lite V2 requires a 680uF capacitor between 5V and GND. See documentation for wiring.
