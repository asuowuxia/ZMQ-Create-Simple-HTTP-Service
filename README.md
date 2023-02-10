# ZMQ-Create-Simple-HTTP-Service
看到github上很多上传的代码，简直是炸药包，随时把新来的github小白炸的粉身碎骨。
![image](https://user-images.githubusercontent.com/13326017/218037244-6baf20b3-41a1-438b-98a3-3662461c4365.png)
上图debug下面是windows平台下项目生成的exe
server00是源码目录

其中最扯淡，全网无人说清楚（随提供编译博客，但你他妈倒是把库给我阿），无人慷慨提供的ZeroMQ库文件，就在源码目录server00文件夹中
zeroMQ库文件：1、include头文件 （zmq.h zmqutil.h) 2、动态符号导出库（.dll） 3、链接库函数（.lib)
![image](https://user-images.githubusercontent.com/13326017/218038148-d49b7010-3da1-46b4-b90c-ae600bf28d12.png)

其中动态符号导出文件(.dll)，对于Windows平台来说需要可执行程序(.exe)去查找符号，所以需要跟.exe放在一处，如下图
![image](https://user-images.githubusercontent.com/13326017/218038558-0c9a3a88-1f26-42b1-9f9e-98c31396157e.png)
