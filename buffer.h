#ifndef BUFFER_H
#define BUFFER_H


class Buffer
{

public:
    Buffer(char* array, int size);
    ~Buffer();

    int putByte(char* ch);
    int getByte(char* ch);
    int putN(char* data, int offset, int n);
    int getN(char* data, int offset, int n);
    int getLength();

private:
    char* data_array;
    int array_size;
    int get_idx;
    int put_idx;
    int length;
};

#endif // BUFFER_H
