#include "array.h"

int main(int argc, char const *argv[])
{
   
    int output;
    char * vector1 = "vector1";
    int elements1 = 100;

    char * vector2 = "vector2";
    int elements2 = 200;


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
    
    int position1 = 0;
    int value1 = 40;

    int position2 = 120;
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
    
    output = set(vector2,position2,value2);
    switch (output)
    {
    case 0:
        printf("Value %d successfully inseted at positon %d of vecotr %s\n",value2,position2,vector2);
        break;
    case -1:
        printf("Error when inserting value %d in positon %d of vector %s\n",value2,position2,vector2);
        break;
    }

    int value;
    int position = 0;
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
    position = 120;
     output = get(vector2,position,&value);
    switch (output)
    {
    case 0:
        printf("Value %d successfully rerieved at positon %d of vecotr %s\n",value,position,vector2);
        break;
    case -1:
        printf("Error when retriving value %d at positon %d of vector %s\n",value,position,vector2);
        break;
    }
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
    output = destroy(vector1);
    switch (output)
    {
    case 1:
        printf("Vector %s succesfully deleted\n",vector1);
        break;
    case -1:
        printf("Error when deleting vector %s\n",vector1);
        break;
    } 

    output = destroy("vector");
    switch (output)
    {
    case 1:
        printf("Vector %s succesfully deleted\n","vector");
        break;
    case -1:
        printf("Error when deleting vector %s\n","vector");
        break;
    } 

    return 0;
}
