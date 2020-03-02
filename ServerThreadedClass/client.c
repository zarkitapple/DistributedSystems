#include <mqueue.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#define MAXSIZE 256
struct request {
    int a; /* op. 1 */
    int b; /* op. 2 */
    char q_name[MAXSIZE]; /* client queue name – this is
        where the server sends the reply to */
};
int main(void) {
    mqd_t q_server; /* server message queue */
    mqd_t q_client; /* client message queue */
    struct request req;
    int res;
    struct mq_attr attr;
    attr.mq_maxmsg = 5;
    attr.mq_msgsize = sizeof(int);
    sprintf(req.q_name,"/CLIENT_%d",getpid());
    q_client = mq_open(req.q_name, O_CREAT|O_RDONLY, 0700, &attr);
    q_server = mq_open("/SERVER", O_WRONLY);
    /* fill in request */
    req.a = 5; req.b = 2; 
    mq_send(q_server, &req, sizeof(struct request), 0);
    mq_receive(q_client, &res, sizeof(int), 0);
    printf("Result %d\n", res);
    mq_close(q_server);
    mq_close(q_client);
    mq_unlink(req.q_name);
    return 0;
}
