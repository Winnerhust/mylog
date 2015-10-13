CC=g++ -Wall -g

APP=logtest

all:${APP}
	
log.o:log.cpp
	${CC} log.cpp -c
${APP}:main.cpp
	${CC} main.cpp -o ${APP} log.o

clean:
	rm -rf *.o ${APP} ${APP}.exe
