#include <mqueue.h>
#define SERVERQ "/SERVER"
int main(int argc, char const *argv[])
{
    mq_unlink(SERVERQ);
    return 0;
}
