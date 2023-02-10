// client00.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <zmq.h>
#include <stdio.h>
//#include <unistd.h>
#include <Windows.h>
#include <string.h>

int main() {
	void* context = zmq_init(1);
	// Socket to talk to clients
	void* responder = zmq_socket(context, ZMQ_REP);
	zmq_bind(responder, "tcp://*:5555");
	while (1) {
		// Wait for next request from client
		zmq_msg_t request;
		zmq_msg_init(&request);
		zmq_recv(responder, &request, 0, 0);
		printf("Received request: [%s]\n",
			(char*)zmq_msg_data(&request));
		zmq_msg_close(&request);

		// Do some 'work'
		for (int i = 0; i < 50; i++);

		// Send reply back to client
		zmq_msg_t reply;
		zmq_msg_init_size(&reply, 6);
		memcpy((void*)zmq_msg_data(&reply), "World", 6);
		zmq_send(responder, &reply, 0, 0);
		zmq_msg_close(&reply);
	}
	zmq_term(context);
	return 0;
}
