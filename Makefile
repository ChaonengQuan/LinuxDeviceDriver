obj-m += cards_driver.o

KDIR := /lib/modules/$(shell uname -r)/build

PWD := $(shell pwd)

all: blackjack
	gcc -o blackjack blackjack.o
	$(MAKE) -C $(KDIR) M=$(PWD) modules

blackjack.o: blackjack.c blackjack.h
	gcc -c blackjack.c
