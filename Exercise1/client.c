#include "array.h"




int main(int argc, char const *argv[])
{
   
    int output;
    char * vector1 = "vector1";
    int elements1 = 10;

    char * vector2 = "vector2";
    int elements2 = 5;

    char * vector3 = "vHctor54";
    int elements3 = 2;

    puts("Init (vector1, 10);");
    output = init(vector1,elements1);
    switch (output)
    {
    case 1:
        printf("Vector %s of %d elements created succesfully\n",vector1,elements1);
        break;
    case 0:
        printf("Vector %s had already been created\n",vector1);
        break;
    case -1:
        printf("Error when creating vector %s\n",vector1);
        break;
    }
    
    puts("Init (vector2, 5);");
    output = init(vector2,elements2);

    switch (output)
    {
    case 1:
        printf("Vector %s of %d elements created succesfully\n",vector2,elements2);
        break;
    case 0:
        printf("Vector %s had already been created\n",vector2);
        break;
    case -1:
        printf("Error when creating vector %s\n",vector2);
        break;
    }

    puts("Init (vector3, 2);");
    output = init(vector3,elements3);

    switch (output)
    {
    case 1:
        printf("Vector %s of %d elements created succesfully\n",vector3,elements3);
        break;
    case 0:
        printf("Vector %s had already been created\n",vector3);
        break;
    case -1:
        printf("Error when creating vector %s\n",vector3);
        break;
    }
    
    int position1 = 0;
    int value1 = 40;

    int position2 = 2;
    int value2 = 30;

   output = set(vector1,position1,value1);
    switch (output)
    {
    case 0:
        printf("Value %d successfully inseted at positon %d of vecotr %s\n",value1,position1,vector1);
        break;
    case -1:
        printf("Error when inserting value %d in positon %d of vector %s\n",value1,position1,vector1);
        break;
    } 
    
    output = set(vector3,position2,value2);
    switch (output)
    {
    case 0:
        printf("Value %d successfully inseted at positon %d of vecotr %s\n",value2,position2,vector3);
        break;
    case -1:
        printf("Error when inserting value %d in positon %d of vector %s\n",value2,position2,vector3);
        break;
    }

    int value;
    int position = 5;
     output = get(vector1,position,&value);
    switch (output)
    {
    case 0:
        printf("Value %d successfully rerieved at positon %d of vecotr %s\n",value,position,vector1);
        break;
    case -1:
        printf("Error when retriving value %d at positon %d of vector %s\n",value,position,vector1);
        break;
    }
    position = 100;
     output = get(vector1,position,&value);
    switch (output)
    {
    case 0:
        printf("Value %d successfully rerieved at positon %d of vecotr %s\n",value,position,vector1);
        break;
    case -1:
        printf("Error when retriving value %d at positon %d of vector %s\n",value,position,vector1);
        break;
    }

    output = destroy(vector2);
    switch (output)
    {
    case 1:
        printf("Vector %s succesfully deleted\n",vector2);
        break;
    case -1:
        printf("Error when deleting vector %s\n",vector2);
        break;
    } 
    return 0;
}
