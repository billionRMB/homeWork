# 设置c 语言的编译器
CC=gcc
CFLAGS=-g -Wall

#common 需要的依赖
CommonOBJS=socklib.o

.PHONY:clean

# 创建server
server: $(CommonOBJS)
	$(CC) $(CFLAGS) -c socketServer.c -lpthread
	$(CC) $(CFLAGS) -o server $(CommonOBJS) socketServer.o -lpthread

# 创建client
client: $(CommonOBJS)
	$(CC) $(CFLAGS) -c socketClient.c -lpthread
	$(CC) $(CFLAGS) -o client $(CommonOBJS) socketClient.o -lpthread

$(CommonOBJS): common/socklib.c
	$(CC) $(CFLAGS) -c common/socklib.c -lpthread

clean:
	rm *.o
