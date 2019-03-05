# 设置c 语言的编译器
CC=gcc
CFLAGS=-g -Wall

#common 需要的依赖
CommonOBJS=common.o

.PHONY:clean

# 创建server
client: $(CommonOBJS)
	$(CC) $(CFLAGS) -c client.c -lpthread
	$(CC) $(CFLAGS) -o client $(CommonOBJS) client.o -lpthread

$(CommonOBJS): common/common.c
	$(CC) $(CFLAGS) -c common/common.c -lpthread

clean:
	rm *.o
