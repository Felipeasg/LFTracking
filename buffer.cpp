#include "buffer.h"

Buffer::Buffer(char *array, int size)
{
    data_array = array;
    array_size = size - 1;
    length = 0;
    put_idx = 0;
    get_idx = 0;
}

Buffer::~Buffer()
{

}

int Buffer::putByte(char* ch)
{
    if (length > array_size)
        return -1;

    int ptr = (put_idx + get_idx) & array_size;

    data_array[ptr] = *ch;
    put_idx++;
    length++;

    return 1;
}

int Buffer::getByte(char* ch)
{
    if(put_idx > 0)
    {
        *ch = data_array[get_idx];
        put_idx--;
        length--;

        get_idx = (get_idx + 1) & array_size;

        return 1;

    }

    else
        return -1;
}

int Buffer::putN(char *data, int offset, int n)
{
    int put;


    if (n > ((array_size + 1) - length))
        n = array_size + 1 - length;


    put = n;

    int i = offset;
    for (; n > 0; n--)
    {

        int ptr = (put_idx + get_idx) & array_size;

        data_array[ptr] = data[i++];
        put_idx++;

    }

    length += put;

    return put;
}

int Buffer::getN(char *data, int offset, int n)
{
    int got;

    if (n > length)
        n = length;

    got = n;
    int i = offset;
    for (; n > 0; n--)
    {
        data[i++] = data_array[get_idx];

        put_idx = (put_idx - 1) & array_size;
        get_idx = (get_idx + 1) & array_size;
    }
    length -= got;

    return got;
}

int Buffer::getLength()
{
    return length;
}

