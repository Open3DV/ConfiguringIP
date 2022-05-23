# ConfiguringIP

#### 介绍
此项目是针对使用网络连接的linux嵌入式设备的网络发现机制的实现。可以在设备的初始IP地址未知的情况下，对设备的IP进行配置。

#### 软件架构
firmware/                嵌入式设备中的服务器代码
sdk/                     上位机（PC）的软件开发包
configuring_ip/          上位机命令行程序
configuring_network_gui/ 上位机GUI程序


#### 安装教程

1.  将firmware/中的程序在嵌入式设备中编译，并设置为开机自动运行。在/etc/rc.local文件中增加一行
    [path-to-the-executable]/configuring_ip &
2.  在/etc/dhcp/dhclient.conf文件中，将timeout改为60。当网络中不存在DHCP服务器时，嵌入式设备的DHCP机制尝试60秒仍没有获取到IP，即转为AVAHI机制协商IP。协商出的IP在169.254.x.x网段。

#### 使用说明

嵌入式设备的IP配置只能是【静态IP】和【自动获取IP】这两者之一。设备出厂时，全部设置为【自动获取IP】。
当设置为【自动获取IP】时，情况又分为两种：【网络中有路由器】和【网络中没有路由器】。
1.  【自动获取IP】且【网络中有路由器】：此时路由器将给设备分配一个IP。上位机通过get-list命令可以获取到网络中在线的设备的MAC和IP。
2.  【自动获取IP】且【网络中没有路由器】：此时设备无法获取IP，上电约1分钟后，设备转换为AVAHI机制协商IP。上位机通过get-list命令可以获取到网络中在线的设备的MAC和IP。
3.  【静态IP】：此时设备无法获取IP。上位机在同一个网络中，同样无法获取IP，所以上位机也需要设置成静态IP，网段需要在192.168.x.x。上位机设置好静态IP后，可通过get-list命令获取到网络中在线的设备的MAC和IP。
