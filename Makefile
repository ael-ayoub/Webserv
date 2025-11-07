NAME = Webserv

SRCS = main.cpp SRC/ConfigFile/Config.cpp SRC/ConfigFile/ServerConfig.cpp \
	   SRC/ConfigFile/LocationConfig.cpp SRC/ConfigFile/Syntax_server.cpp \
	   SRC/ConfigFile/Syntax_location.cpp SRC/Socket.cpp\
	   SRC/Methodes/GetMethode.cpp SRC/req_res/Request.cpp\
	   SRC/Methodes/PostMethode.cpp \
	   SRC/req_res/Response.cpp SRC/req_res/ErrorResponse.cpp SRC/runServer.cpp \
	   SRC/Session/LoadUsers.cpp

OBJ = $(SRCS:.cpp=.o)

CC = c++

FLAGS = -Wall -Wextra -Werror -std=c++98

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(FLAGS) $(SRCS) -o $(NAME)

%.o:%.cpp
	c++ $(FLAGS) -c $< -o $@

fclean: clean
	rm -f $(NAME)

clean:
	rm -f $(OBJ)

re: fclean all