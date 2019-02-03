CC = g++
SOURCE = main.cpp
OUT = out
FLAGS = -W -Wall -Werror -std=c++14

$(OUT): $(SOURCE)
	$(CC) $(FLAGS) $(LDFLAGS) $(SOURCE) -o $(OUT)
