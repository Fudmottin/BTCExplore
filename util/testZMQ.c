#include <stdio.h>
#include <string.h>
#include <zmq.h>

int main() {
    void *context = zmq_ctx_new();
    void *subscriber = zmq_socket(context, ZMQ_SUB);

    zmq_connect(subscriber, "tcp://127.0.0.1:28332");  // Connect to the ZMQ endpoint
    zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, "", 0);  // Subscribe to all messages

    while (1) {
        zmq_msg_t message;
        zmq_msg_init(&message);
        zmq_recv(subscriber, &message, 0, 0);

        // Process the message
        char *messageStr = (char *)zmq_msg_data(&message);
        printf("Received message: %s\n", messageStr);

        zmq_msg_close(&message);
    }

    zmq_close(subscriber);
    zmq_ctx_destroy(context);

    return 0;
}

