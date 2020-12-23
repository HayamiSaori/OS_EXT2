C = src

.PHONY:clean
clean:
	rm src/shell

.PHONY:reset
reset:
	rm $(C)/shell;rm $(C)/disk

.PHONY:run
run:
	gcc $(C)/shell.c -o $(C)/shell -w;cd $(C);./shell;
.PHONY:run_in_windows
run_in_windows:
	gcc $(C)/shell.c -o $(C)/shell -w;cd $(C);./shell;