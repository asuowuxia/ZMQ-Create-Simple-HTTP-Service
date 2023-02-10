// ConsoleApplication3.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "zmq.h"

// #pragma comment(lib, "libzmq-v143-mt-sgd-4_3_5.lib")

//包含zmq的头文件
#include "stdio.h"

#if 0
int main(int argc, char* argv[])
{
    void* pCtx = NULL;
    void* pSock = NULL;
    //使用tcp协议进行通信，需要连接的目标机器IP地址为192.168.1.2
    //通信使用的网络端口 为7766
    const char* pAddr = "tcp://tcp://192.168.246.1:7766";

    //创建context
    if ((pCtx = zmq_ctx_new()) == NULL)
    {
        return 0;
    }
    //创建socket
    if ((pSock = zmq_socket(pCtx, ZMQ_DEALER)) == NULL)
    {
        zmq_ctx_destroy(pCtx);
        return 0;
    }
    int iSndTimeout = 5000;// millsecond
    //设置接收超时
    if (zmq_setsockopt(pSock, ZMQ_RCVTIMEO, &iSndTimeout, sizeof(iSndTimeout)) < 0)
    {
        zmq_close(pSock);
        zmq_ctx_destroy(pCtx);
        return 0;
    }
    //连接目标IP192.168.1.2，端口7766
    if (zmq_connect(pSock, pAddr) < 0)
    {
        zmq_close(pSock);
        zmq_ctx_destroy(pCtx);
        return 0;
    }
    //循环发送消息
    while (1)
    {
        static int i = 0;
        char szMsg[1024] = { 0 };
        snprintf(szMsg, sizeof(szMsg), "hello world : %3d", i++);
        printf("Enter to send...\n");
        if (zmq_send(pSock, szMsg, sizeof(szMsg), 0) < 0)
        {
            fprintf(stderr, "send message faild\n");
            continue;
        }
        printf("send message : [%s] succeed\n", szMsg);
        getchar();
    }

    return 0;
}
#endif

#if 0
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <zmq.h>

// 向socket发送数据, 数据为string
static int s_send(void* socket, char* string);
// 从socket接收数据, 并将数据以字符串的形式返回
static char* s_recv(void* socket);

int main()
{
    // 1.创建上下文
    void* context = zmq_ctx_new();

    // 2.创建套接字、连接服务器
    void* requester = zmq_socket(context, ZMQ_REQ);
    zmq_connect(requester, "tcp://127.0.0.1:5555");

    int rc;
    // 3.循环发送数据、接收数据
    while (1)
    {
        // 4.发送数据
        char* request = (char*)("Hello");
        rc = s_send(requester, request);
        assert(rc > 0);

        // 5.接收回复数据
        char* reply = s_recv(requester);
        assert(reply != NULL);
        printf("Reply: %s\n", reply);
        free(reply);
    }

    // 6.关闭套接字、销毁上下文
    zmq_close(requester);
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

    // 2.创建套接字、连接代理的ROUTER端
    void* requester = zmq_socket(context, ZMQ_REQ);
    rc = zmq_connect(requester, "tcp://localhost:5559");
    if (rc == -1)
    {
        perror("zmq_connect");
        zmq_close(requester);
        zmq_ctx_destroy(context);
        return -1;
    }

    // 3.循环发送、接收数据(10次)
    int request_nbr;
    for (request_nbr = 0; request_nbr < 10; request_nbr++)
    {
        // 4.先发送数据
        rc = s_send(requester, "Hello");
        if (rc < 0)
        {
            perror("s_send");
            zmq_close(requester);
            zmq_ctx_destroy(context);
            return -1;
        }

        // 5.等待响应
        char* reply = s_recv(requester);
        if (reply == NULL)
        {
            perror("s_recv");
            free(reply);
            zmq_close(requester);
            zmq_ctx_destroy(context);
            return -1;
        }
        printf("Reply[%d]: %s\n", request_nbr + 1, reply);
        free(reply);
    }

    // 6.关闭套接字、销毁上下文
    zmq_close(requester);
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

/*
或者是一种有效的代替常规saocket通讯的方法。

1）下载地址：http://zeromq.org/distro:microsoft-windows

2）按照说明。获取相应的版本，并安装。

3 ) 安装目录中，include文件集中下有两个包含头文件：zmq.h   zmq.utils

3 ) 库文件,根据操作系统选择相应的文件。

    例）：lib/ libzmq-v110-mt-gd-4_0_4.lib

               bin/ libzmq-v110-mt-gd-4_0_4.dll

4）qtcreator 导入：

特别的需要把*.lib文件改名（不知为什么）：libzmq-v110-mt-gd-4_0_4.lib  改为：lzmq-v110-mt-gd-4_0_4.lib

 然后在工程文件(*.pro)添加：


 */
