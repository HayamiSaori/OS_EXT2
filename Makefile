C = src
CMD = \
	$C/shell.c \
	$C/ls.c \
	$C/mkdir.c \
	/*$C/cd.c \*/
	$C/touch.c \
	$C/cp.c \
	$C/shutdown

main.c:$(CMD)
	gcc main.c -o main

.PHONY:clean
clean:
	