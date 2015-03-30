com:main.o protocal.o serial.o util.o pro_timer.o
	cc -o com main.o protocal.o serial.o util.o pro_timer.o -pthread -lrt
main.o:main.c protocal.h serial.h config.h util.h pro_timer.h
	cc -c main.c
serial.o:serial.c protocal.h serial.h config.h util.h pro_timer.h
	cc -c serial.c
protocal.o:protocal.c protocal.h serial.h config.h util.h pro_timer.h
	cc -c protocal.c
util.o:util.c protocal.h serial.h config.h util.h pro_timer.h
	cc -c util.c
pro_timer.o:pro_timer.c protocal.h serial.h config.h util.h pro_timer.h
	cc -c pro_timer.c
.PHONY:clean
clean:
	-rm -f *.o com
