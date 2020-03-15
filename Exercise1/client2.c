#include "array.h"
#include <stdio.h>
int main(int argc, char const *argv[])
{
    int result;
    if(init("vector1", 100) == 1) {puts("pass");};
    puts("here1");
    if(init("vector2", 200) == 1) {puts("pass");};
     puts("here2");
    if(set("vector1", 0, 40) == 0) {puts("pass");};
     puts("here3");
    if(set("vector1", 120, 30) == -1) {puts("pass");};
     puts("here4");
    if(init("vector1", 200) == -1) {puts("pass");};
     puts("here5");
    if(init("vector1", 100) == 0) {puts("pass");};
     puts("here6");
    if(get("vector1", 0, &result) == 0 && result == 40 ){puts("pass");};
     puts("here7");
    if(get("vector1", 140, &result) == -1){puts("pass");};
     puts("here8");
    if(destroy("vector1") == 1) {puts("pass");};
     puts("here9");
    if(destroy("vector") == -1) {puts("pass");};
     puts("here10");

    return 0;
}