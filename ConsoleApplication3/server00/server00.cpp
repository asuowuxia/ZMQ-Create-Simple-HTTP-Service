// server00.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

//
// Hello World server
// Binds REP socket to tcp://*:5555
// Expects "Hello" from client, replies with "World"
//
#include <zmq.h>
#include <stdio.h>
//#include <unistd.h>
#include <Windows.h>
#include <string.h>
#include <assert.h>

int main() {

	void* ctx = zmq_ctx_new();
	assert(ctx);
	/* Create ZMQ_STREAM socket */
	void* socket = zmq_socket(ctx, ZMQ_STREAM);
	assert(socket);
	int rc = zmq_bind(socket, "tcp://*:8080");
	assert(rc == 0);
	/* Data structure to hold the ZMQ_STREAM routing id */
	uint8_t routing_id[256];
	size_t routing_id_size = 256;
	/* Data structure to hold the ZMQ_STREAM received data */
	uint8_t raw[256];
	size_t raw_size = 256;
	while (1) {
		/* Get HTTP request; routing id frame and then request */
		routing_id_size = zmq_recv(socket, routing_id, 256, 0);
		// assert(routing_id_size > 0);
		do {
			raw_size = zmq_recv(socket, raw, 256, 0);

			assert(raw_size >= 0);
		} while (raw_size == 256);
		/* Prepares the response */
		char http_response[] =
			"HTTP/1.0 200 OK\r\n"
			"Content-Type: text/plain\r\n"
			"\r\n"
			"Hello, World!";
		/* Sends the routing id frame followed by the response */
		zmq_send(socket, routing_id, routing_id_size, ZMQ_SNDMORE);
		zmq_send(socket, http_response, strlen(http_response), 0);
		/* Closes the connection by sending the routing id frame followed by a zero response */
		zmq_send(socket, routing_id, routing_id_size, ZMQ_SNDMORE);
		zmq_send(socket, 0, 0, 0);
	}
	zmq_close(socket);
	zmq_ctx_destroy(ctx);

	return 0;
}

