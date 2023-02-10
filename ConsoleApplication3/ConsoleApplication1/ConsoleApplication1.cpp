// ConsoleApplication1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

//包含zmq的头文件
#include <zmq.h>
#include "stdio.h"

#if 0
int main(int argc, char* argv[])
{
    void* pCtx = NULL;
    void* pSock = NULL;
    const char* pAddr = "tcp://192.168.246.1:7766";

    //创建context，zmq的socket 需要在context上进行创建
    if ((pCtx = zmq_ctx_new()) == NULL)
    {
        return 0;
    }
    //创建zmq socket ，socket目前有6中属性 ，这里使用dealer方式
    //具体使用方式请参考zmq官方文档（zmq手册）
    if ((pSock = zmq_socket(pCtx, ZMQ_DEALER)) == NULL)
    {
        zmq_ctx_destroy(pCtx);
        return 0;
    }
    int iRcvTimeout = 5000;// millsecond
    //设置zmq的接收超时时间为5秒
    if (zmq_setsockopt(pSock, ZMQ_RCVTIMEO, &iRcvTimeout, sizeof(iRcvTimeout)) < 0)
    {
        zmq_close(pSock);
        zmq_ctx_destroy(pCtx);
        return 0;
    }
    //绑定地址 tcp://*:7766
    //也就是使用tcp协议进行通信，使用网络端口 7766
    if (zmq_bind(pSock, pAddr) < 0)
    {
        zmq_close(pSock);
        zmq_ctx_destroy(pCtx);
        return 0;
    }
    printf("bind at : %s\n", pAddr);
    while (1)
    {
        char szMsg[1024] = { 0 };
        printf("waitting...\n");
        errno = 0;
        //循环等待接收到来的消息，当超过5秒没有接到消息时，
        //zmq_recv函数返回错误信息 ，并使用zmq_strerror函数进行错误定位
        if (zmq_recv(pSock, szMsg, sizeof(szMsg), 0) < 0)
        {
            printf("error = %s\n", zmq_strerror(errno));
            continue;
        }
        printf("received message : %s\n", szMsg);
    }

    return 0;
}
#endif

////  对了使用ZMQ之前必须要有那么几样东西libzmq.lib，zhelpers.hpp，zmq.h，zmq.hpp。这些都可以在ZMQ的官网下载。

#if 0
#include <zmq.hpp>
#include <string>
#include <iostream>
#include <windows.h>
#include<zhelpers.hpp>
using namespace std;

// 因此抛弃了zmq，选择了netty。
//// MQTT等发布、订阅概念有所不同。
//// MQTT在订阅时支持通过 “#“ 符号通配订阅多个主题。无意中发现zeromq也是默认支持这一特性的。


DWORD WINAPI MyThread_client(LPVOID lpParamter)
{
    zmq::context_t context(1);
    //建立套接字
    zmq::socket_t socket(context, ZMQ_REQ);

    std::cout << "Connecting to hello world server..." << std::endl;
    //连接服务器
    socket.connect("tcp://localhost:5555");

    for (int request_nbr = 0; request_nbr != 10; request_nbr++) {
        s_send(socket, "hello");
        std::cout << "Client1 Received :" << s_recv(socket) << std::endl;

        Sleep(1000);
    }
    return 0;

}

DWORD WINAPI MyThread_client1(LPVOID lpParamter)
{
    zmq::context_t context(1);
    //建立套接字
    zmq::socket_t socket(context, ZMQ_REQ);

    std::cout << "Connecting to hello world server..." << std::endl;
    //连接服务器
    socket.connect("tcp://localhost:5555");

    for (int request_nbr = 0; request_nbr != 10; request_nbr++) {
        s_send(socket, "SB");
        std::cout << "Client2 Received :" << s_recv(socket) << std::endl;

        Sleep(1000);
    }
    return 0;

}

DWORD WINAPI MyThread_servce(LPVOID lpParamter)
{
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_REP);
    //绑定端口
    socket.bind("tcp://*:5555");

    while (true) {
        std::cout << "Servce Received: " << s_recv(socket) << std::endl;
        s_send(socket, "world");
    }
}

int main()
{

    HANDLE hThread1 = CreateThread(NULL, 0, MyThread_client, NULL, 0, NULL);
    HANDLE hThread2 = CreateThread(NULL, 0, MyThread_servce, NULL, 0, NULL);

    HANDLE hThread3 = CreateThread(NULL, 0, MyThread_client1, NULL, 0, NULL);

    while (1);
    return 0;
}
#endif

#if 0
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
// #include <unistd.h>
#include <zmq.h>

// 向socket发送数据, 数据为string
static int s_send(void* socket, char* string);
// 从socket接收数据, 并将数据以字符串的形式返回
static char* s_recv(void* socket);

int main()
{
    // 1.创建上下文
    void* context = zmq_ctx_new();

    // 2.创建、绑定套接字
    void* responder = zmq_socket(context, ZMQ_REP);
    zmq_bind(responder, "tcp://127.0.0.1:5555");

    int rc;
    // 3.循环接收数据、发送数据
    while (1)
    {
        // 4.接收数据
        char* request = s_recv(responder);
        assert(request != NULL);
        printf("Request: %s\n", request);
        free(request);

        // 休眠1秒再继续回复
       // usleep(1);

        // 5.回送数据
        char* reply = (char*)("World");
        rc = s_send(responder, reply);
        assert(rc > 0);
    }

    // 6.关闭套接字、销毁上下文
    zmq_close(responder);
    zmq_ctx_destroy(context);

    return 0;
}

static int s_send(void* socket, char* string)
{
    int rc;

    zmq_msg_t msg;
    zmq_msg_init_size(&msg, 5);
    memcpy(zmq_msg_data(&msg), string, strlen(string));

    rc = zmq_msg_send(&msg, socket, 0);
    zmq_msg_close(&msg);

    return rc;
}

static char* s_recv(void* socket)
{
    int rc;
    zmq_msg_t msg;
    zmq_msg_init(&msg);

    rc = zmq_msg_recv(&msg, socket, 0);
    if (rc == -1)
        return NULL;

    char* string = (char*)malloc(rc + 1);
    memcpy(string, zmq_msg_data(&msg), rc);
    zmq_msg_close(&msg);

    string[rc] = 0;
    return string;
}
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <zmq.h>

// 向socket发送数据, 数据为string
static int s_send(void* socket, char* string);
// 从socket接收数据, 并将数据以字符串的形式返回
static char* s_recv(void* socket);

int main()
{
    int rc;
    // 1.初始化上下文
    void* context = zmq_ctx_new();

    // 2.创建套接字、连接代理的DEALER端
    void* responder = zmq_socket(context, ZMQ_REP);
    rc = zmq_connect(responder, "tcp://localhost:5560");
    if (rc == -1)
    {
        perror("zmq_connect");
        zmq_close(responder);
        zmq_ctx_destroy(context);
        return -1;
    }

    // 3.循环接收、响应
    while (1)
    {
        // 4.先等待接收数据
        char* request = s_recv(responder);
        if (request == NULL)
        {
            perror("s_recv");
            free(request);
            zmq_close(responder);
            zmq_ctx_destroy(context);
            return -1;
        }
        printf("Request: %s\n", request);
        free(request);

        // 休眠1秒再进行响应
        sleep(1);

        // 5.响应
        rc = s_send(responder, "World");
        if (rc < 0)
        {
            perror("s_send");
            zmq_close(responder);
            zmq_ctx_destroy(context);
            return -1;
        }
    }

    // 6.关闭套接字、销毁上下文
    zmq_close(responder);
    zmq_ctx_destroy(context);

    return 0;
}

static int s_send(void* socket, char* string)
{
    int rc;

    zmq_msg_t msg;
    zmq_msg_init_size(&msg, 5);
    memcpy(zmq_msg_data(&msg), string, strlen(string));

    rc = zmq_msg_send(&msg, socket, 0);
    zmq_msg_close(&msg);

    return rc;
}

static char* s_recv(void* socket)
{
    int rc;
    zmq_msg_t msg;
    zmq_msg_init(&msg);

    rc = zmq_msg_recv(&msg, socket, 0);
    if (rc == -1)
        return NULL;

    char* string = (char*)malloc(rc + 1);
    memcpy(string, zmq_msg_data(&msg), rc);
    zmq_msg_close(&msg);

    string[rc] = 0;
    return string;
}

////////////////////////////////////// 代理服务器
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zmq.h>

int main()
{
    int rc;
    // 1.初始化上下文
    void* context = zmq_ctx_new();

    // 2.创建、绑定套接字
    void* frontend = zmq_socket(context, ZMQ_ROUTER);
    void* backend = zmq_socket(context, ZMQ_DEALER);
    // ZMQ_ROUTER绑定到5559, 接收客户端的请求
    rc = zmq_bind(frontend, "tcp://*:5559");
    if (rc == -1)
    {
        perror("zmq_bind");
        zmq_close(frontend);
        zmq_close(backend);
        zmq_ctx_destroy(context);
        return -1;
    }
    // ZMQ_DEALER绑定到5560, 接收服务端的回复
    rc = zmq_bind(backend, "tcp://*:5560");
    if (rc == -1)
    {
        perror("zmq_bind");
        zmq_close(frontend);
        zmq_close(backend);
        zmq_ctx_destroy(context);
        return -1;
    }
    // 3.初始化轮询集合
    zmq_pollitem_t items[] = {
        { frontend, 0, ZMQ_POLLIN, 0 },
        { backend, 0, ZMQ_POLLIN, 0 }
    };

    // 4.在套接字上切换消息
    while (1)
    {
        zmq_msg_t msg;
        //多部分消息检测
        int more;

        // 5.调用zmq_poll轮询消息
        rc = zmq_poll(items, 2, -1);
        //zmq_poll出错
        if (rc == -1)
        {
            perror("zmq_poll");
            zmq_close(frontend);
            zmq_close(backend);
            zmq_ctx_destroy(context);
            return -1;
        }
        //zmq_poll超时
        else if (rc == 0)
            continue;
        else
        {
            // 6.如果ROUTER套接字有数据来
            if (items[0].revents & ZMQ_POLLIN)
            {
                while (1)
                {
                    // 从ROUTER上接收数据, 这么数据是客户端发送过来的"Hello"
                    zmq_msg_init(&msg);
                    zmq_msg_recv(&msg, frontend, 0);

                    // 查看是否是接收多部分消息, 如果后面还有数据要接收, 那么more会被置为1
                    size_t more_size = sizeof(more);
                    zmq_getsockopt(frontend, ZMQ_RCVMORE, &more, &more_size);

                    // 接收"Hello"之后, 将数据发送到DEALER上, DEALER会将"Hello"发送给服务端
                    zmq_msg_send(&msg, backend, more ? ZMQ_SNDMORE : 0);
                    zmq_msg_close(&msg);

                    // 如果没有多部分数据可以接收了, 那么退出循环
                    if (!more)
                        break;
                }
            }
            // 7.如果DEALER套接字有数据来
            if (items[1].revents & ZMQ_POLLIN)
            {

                while (1)
                {
                    // 接收服务端的响应"World"
                    zmq_msg_init(&msg);
                    zmq_msg_recv(&msg, backend, 0);

                    // 查看是否是接收多部分消息, 如果后面还有数据要接收, 那么more会被置为1
                    size_t more_size = sizeof(more);
                    zmq_getsockopt(backend, ZMQ_RCVMORE, &more, &more_size);

                    // 接收"World"之后, 将数据发送到ROUTER上, ROUTER会将"World"发送给客户端
                    zmq_msg_send(&msg, frontend, more ? ZMQ_SNDMORE : 0);
                    zmq_msg_close(&msg);

                    // 如果没有多部分数据可以接收了, 那么退出循环
                    if (!more)
                        break;
                }
            }
        }
    }

    // 8.关闭套接字、销毁上下文
    zmq_close(frontend);
    zmq_close(backend);
    zmq_ctx_destroy(context);

    return 0;
}