CC=aarch64-linux-gnu-gcc
#CC=gcc 
CFLAG=-c
ALL=gpio_ctl
OBJS=socket.o gpio_func.o main.o

gpio_ctl:socket.o gpio_func.o main.o
	sudo $(CC) $(OBJS) -o $@	-lpthread

$(OBJS):%.o:%.c
	sudo $(CC) $(CFLAG) $< -o $@

.PHONY:clean
clean:
	sudo rm -rf $(ALL) $(OBJS)

