default: hello hello_user hello.class times_tables

hello: hello.c
	gcc -o hello hello.c -Wall -Werror -ggdb

hello_user: hello_user.c
	gcc -o hello_user hello_user.c -Wall -Werror -ggdb

times_tables: times_tables.c
	gcc -o times_tables times_tables.c -Wall -Werror -ggdb

hello.class: hello.java
	javac hello.java

clean:
	rm -f hello hello_user hello.class times_tables *~
