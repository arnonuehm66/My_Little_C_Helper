# Preliminaries:
NAME = skeleton_main_c

CC       = clang
CFLAGS   = -Wall -Ofast -DNDEBUG
LIBS     =
DBCFLAGS = -Wall -O0 -g -DDEBUG

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

STRIP = strip

# Release
$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	$(STRIP) $@
	$(RM) $(OBJS)

# Debug
debug: CFLAGS = $(DBCFLAGS)
debug: $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $^ $(LIBS)

# Source to object for debug and release.
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $< $(LIBS)

# Make tidy.
clean:
	$(RM) $(NAME) $(OBJS)
