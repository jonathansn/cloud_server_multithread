EXEC=client
SRC= $(wildcard *.c)
OBJ= $(SRC:.c=.o)


build: greetings all finish

greetings: 
	@echo "$(EXEC) Compiling..."
finish:
	@echo  "$(EXEC) compiled!"

all: $(EXEC)

client: $(OBJ)
	@gcc -o $@ $^

%.o:%.c
	@gcc -c $< -o  $@

.PHONY: clean mrproper
clean: 
	@rm -rf *.o

mrproper: clean
	@rm -rf $(EXEC)
