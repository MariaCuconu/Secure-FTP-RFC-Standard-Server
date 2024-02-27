CC	=	gcc

CFLAGS	=	-W -Wall -I./headers

RM      =	rm -f

NAME    =	server

SRCS    =	src/main.c \
		src/manage_connections.c \
		src/utils.c \
		src/manage_list.c \
		src/signal.c \
		src/manage_user.c \
		src/manage_file.c \
		src/manage_channels.c \
		src/commands_parser.c

OBJS    =       $(SRCS:.c=.o)

all:            $(NAME)

$(NAME):	$(OBJS)
		$(CC) $(OBJS) -o $(NAME)
clean:
		$(RM) $(OBJS)
fclean:		clean
		$(RM) $(NAME)
re:		fclean all

.PHONY:		all clean fclean re

