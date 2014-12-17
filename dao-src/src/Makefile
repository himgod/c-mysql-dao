OBJS=DAO.o
CC=gcc
XX=g++
CFLAGS=-Wall -O -g
LDFLAGS=-lmysqlclient

TARGET=DAO

#所有C文件编译为.o文件，其中$<代表依靠$^列表中第一个依靠的文件
#$@扩展成当前规则的目的文件

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@
%.o:%.cpp
	$(XX) $(CFLAGS) -c $< -o $@

#函数wildcard表示产生一个以.c结尾的文件列表，然后存入变量
SOURCES=$(wildcard *.c *.cpp)
#匹配替换,有三个参数。第一个是一个需要匹配的式样，
#第二个表示用什么来替换，第三个需要被处理的由空格分割的列表
OBJS=$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SOURCES)))

$(TARGET):$(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $(TARGET)
	chmod a+x $(TARGET)

#DAO:${OBJS}
#	${CC} ${OBJS} ${LDFLAGS} -o DAO

#DAO.o:DAO.h DAO.c
#	${CC} ${CFLAGS} -c DAO.c -o DAO.o

clean:
	rm -rf *.o DAO

