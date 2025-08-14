NAME = Webserv

SRCS = main.cpp SRC/ConfigFile/Config.cpp SRC/ConfigFile/ServerConfig.cpp \
	   SRC/ConfigFile/LocationConfig.cpp SRC/ConfigFile/Syntax_server.cpp \
	   SRC/ConfigFile/Syntax_location.cpp SRC/Socket.cpp \
	   SRC/runServer.cpp \
	   SRC/Methodes/GetMethode.cpp SRC/req_res/Request.cpp \
	   SRC/req_res/Response.cpp 

CC = c++

FLAGS = -Wall -Wextra -Werror -std=c++98 -g

all: $(NAME)

$(NAME): $(SRCS)
	$(CC) $(FLAGS) $(SRCS) -o $(NAME)

fclean: clean
	rm -f $(NAME)

clean:

re: fclean all