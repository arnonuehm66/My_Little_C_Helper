# Preliminaries
NAME = skeleton_main_c

CC = gcc
CFLAGS = -Wall -Ofast -DNDEBUG
LIBS = -lpcre2-8 -lcrypto
DBCFLAGS = -Wall -O0 -g -DDEBUG

STRIP = strip

# Release
$(NAME): main.c
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)
	$(STRIP) $@

# Debug
debug: main.c
	$(CC) $(DBCFLAGS) -o $(NAME) $< $(LIBS)

# Make tidy
clean:
	$(RM) $(NAME)
