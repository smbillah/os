obj-m += sys_xjob.o

sys_xjob-y := sioq.o consumer.o buf.o utility.o main.o


all: xhw3 xjob

xhw3: xhw3.c
	gcc -Wall -Werror xhw3.c -o xhw3

xjob:
	make -Wall -Werror -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f xhw3
