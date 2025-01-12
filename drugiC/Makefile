# Definiramo ime modula
obj-m += shofer.o

# Definiramo put do kernela i build direktorij
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

# Pravilo za kompilaciju modula
all:
	make -C $(KDIR) M=$(PWD) modules

# Čišćenje generiranih datoteka
clean:
	make -C $(KDIR) M=$(PWD) clean
