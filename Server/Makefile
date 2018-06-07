<<<<<<< HEAD
LDFLAGS=-lpthread
EXEC=server
SRC= $(wildcard *.c)
OBJ= $(SRC:.c=.o)


build: greetings all finish

greetings: 
	@echo "$(EXEC) Compiling..."
finish:
	@echo  "$(EXEC) compiled!"

all: $(EXEC)

server: $(OBJ)
	@gcc -o $@ $^ $(LDFLAGS)

%.o:%.c
	@gcc -c $< -o  $@

.PHONY: clean mrproper
clean: 
	@rm -rf *.o

mrproper: clean
	@rm -rf $(EXEC)
=======
LDFLAGS=-lpthread
EXEC=server
SRC= $(wildcard *.c)
OBJ= $(SRC:.c=.o)


build: greetings all finish

greetings: 
	@echo "$(EXEC) Compiling..."
finish:
	@echo  "$(EXEC) compiled!"

all: $(EXEC)

server: $(OBJ)
	@gcc -o $@ $^ $(LDFLAGS)

%.o:%.c
	@gcc -c $< -o  $@

.PHONY: clean mrproper
clean: 
	@rm -rf *.o

mrproper: clean
	@rm -rf $(EXEC)
>>>>>>> afc5d668ae2471ef0347a0003e3cdfb93a34613d
