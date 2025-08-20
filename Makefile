NAME = Webserv

SRCS = main.cpp SRC/ConfigFile/Config.cpp SRC/ConfigFile/ServerConfig.cpp \
	SRC/ConfigFile/LocationConfig.cpp SRC/ConfigFile/Syntax_server.cpp \
	SRC/ConfigFile/Syntax_location.cpp SRC/Socket.cpp\
	SRC/Methodes/GetMethode.cpp SRC/Methodes/PostMethode.cpp SRC/req_res/Request.cpp\
	SRC/req_res/Response.cpp SRC/req_res/ErrorResponse.cpp SRC/runServer.cpp \
	SRC/CGIandUpload/CGI.cpp SRC/CGIandUpload/Upload.cpp SRC/CGIandUpload/command_runner.cpp

CPP = c++

FLAGS = -Wall -Wextra -Werror -std=c++98 -g

OBJ = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CPP) $(FLAGS) $(?) -o $(NAME)

fclean: clean
	rm -f $(NAME)

clean:

re: fclean all