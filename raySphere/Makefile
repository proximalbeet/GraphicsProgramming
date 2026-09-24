# target: dependencies
#     command

# variable to define flags once and reuse them
FLAGS=-g -Wall -std=c99

# Compiles c code into excecutible
ray2: raySphere.c
	gcc ${FLAGS} -o ray2 raySphere.c -lm

# Removes executible and any produced images
.PHONY: clean
clean:
	rm -f ray2 *.png
