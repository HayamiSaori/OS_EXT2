C = src

CMD = \
	$(C)/ls.c \
	$(C)/mkdir.c \
	$(C)/touch.c \
	$(C)/cp.c \
	$(C)/shutdown

# $(C)/shell.c:$(CMD)
# 	gcc shell.c -o shell.o

.PHONY:clean
clean:
	rm src/*.o;

.PHONY:reset
reset:
	rm src/*.o;rm src/disk
	
.PHONY:run
run:
	gcc $(C)/shell.c -o $(C)/shell.o;cd $(C);./shell.o