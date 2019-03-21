# 设置c 语言的编译器
CC=gcc
CFLAGS=-g -Wall -fstack-protector-all

#common 需要的依赖
CommonOBJS=common.o

.PHONY:clean


client.o:client.c
	$(CC) $(CFLAGS) -c client.c -lpthread

# 创建client
client: $(CommonOBJS) client.o
	$(CC) $(CFLAGS) -o client $(CommonOBJS) client.o -lpthread

server.o:server.c
	$(CC) $(CFLAGS) -c server.c -lpthread


# 创建server
server: $(CommonOBJS) server.o
	$(CC) $(CFLAGS) -o server $(CommonOBJS) server.o -lpthread


$(CommonOBJS): common/common.c
	$(CC) $(CFLAGS) -c common/common.c -lpthread

clean:
	rm *.o
