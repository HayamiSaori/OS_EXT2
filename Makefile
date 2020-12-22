C = src

CMD = \
	$(C)/ls.c \
	$(C)/mkdir.c \
	$(C)/touch.c \
	$(C)/cp.c

CMD_O = \
	$(C)/ls \
	$(C)/mkdir \
	$(C)/touch \
	$(C)/cp

$(C)/shell.c:$(CMD)
	gcc $(C)/shell.c -o $(C)/shell
$(CMD_O):$(CMD)
	gcc $(CMD) -o $(CMD_O)

.PHONY:clean
clean:
	rm src/shell

.PHONY:reset
reset:
	rm src/*.o;rm src/disk

.PHONY:run
run:
	make;
	gcc $(C)/shell.c -o $(C)/shell;
	cd $(C);./shell