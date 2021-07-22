NAME = skeleton_main

CC = gcc
CFLAGS = -Wall -Ofast -DNDEBUG
LIBS = -lpcre2-8 -lcrypto
DBCFLAGS = -Wall -g -DDEBUG
STRIP = strip
NAME = skeleton_main_c

$(NAME): main.c
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)
	$(STRIP) $@

debug: main.c
	$(CC) $(DBCFLAGS) -o $(NAME) $< $(LIBS)

clean:
	$(RM) $(NAME)
