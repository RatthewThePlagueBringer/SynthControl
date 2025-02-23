#include <string.h> // for memset

// helper printer
#include <stdio.h>
void printMsg(char* msg, int len)
{
	for (int i = 0; i < len * 4; i++)
	{
		printf("%c (%d)\t", msg[i], msg[i]);
		if (!((i + 1) % 4)) putchar('\n');
	}
}

//  since we're working on the kernel level, i don't want to nitroduce
// anything more complicated than like an array, so when creating an osc message
// with commands we use the first byte for metadata and the rest for the message.

// append an int to the message
char* OSCmsgAppendInt(char* msg, int in)
{
	if (msg[1] >= msg[2])
	{
		puts("Not enough space! Create a larger buffer.");
		return;
	}
	msg[msg[0]++] = 'i';
	memcpy(&msg[msg[1]++*4], &in, sizeof(int));
}
// append a float to the message
char* OSCmsgAppendFloat(char* msg, float in)
{
	if (msg[1] >= msg[2])
	{
		puts("Not enough space! Create a larger buffer.");
		return;
	}
	msg[msg[0]++] = 'f';
	memcpy(&msg[msg[1]++ * 4], &in, sizeof(int));
}
// append a string to the message
char* OSCmsgAppendString(char* msg, char* in)
{
	// standard c copies don't seem to be quite sufficient here
	int len = 0;
	while (in[len] != '\0') len++;
	if (msg[1] + len / 4 >= msg[2])
	{
		puts("Not enough space! Create a larger buffer.");
		return;
	}
	
	msg[msg[0]++] = 's';
	memcpy(&msg[msg[1] * 4], in, sizeof(char)*len);
	msg[1] += len / 4 + 1;
}

// the first int size is used for metadata about the message
void OSCmsgAddMetadata(char* msg, int ilen, int intendedArgs)
{
	memset(msg, 0, ilen*4);
	msg[0] = 4;						// the arg type offset, in chars
	msg[1] = (intendedArgs)/4 + 2;	// the value offset, in ints
	msg[2] = ilen;					// total length, for len warnings
}

// the actual sender function (unimplemented)
void sendMessage(char* address, int* port, char* msg, char* len) 
{
	printf("Queued send to %s;%d, message body:\n\n", address, port);
	printMsg(msg+4, len-1);
};

#define OSC_BUFFER_SIZE_INTS 8

int main()
{
	// create the buffer
	char buffer[OSC_BUFFER_SIZE_INTS*4];
	//add info to it
	OSCmsgAddMetadata(buffer, OSC_BUFFER_SIZE_INTS, 4);
	OSCmsgAppendInt(buffer, 100000);
	OSCmsgAppendFloat(buffer, 100000000);
	OSCmsgAppendString(buffer, "hello!");
	OSCmsgAppendInt(buffer, 65);

	sendMessage("127.0.0.1", 8000, buffer, OSC_BUFFER_SIZE_INTS);

	return 0;
}