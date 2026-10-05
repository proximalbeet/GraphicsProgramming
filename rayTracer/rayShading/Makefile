# target: dependencies
#     command

# variable to define flags once and reuse them
FLAGS=-g -Wall -std=c99

# Compiles c code into excecutible
ray3: rayShading.c
	gcc ${FLAGS} -o ray3 rayShading.c -lm

# Removes executible and any produced images
.PHONY: clean
clean:
	rm -f ray3 *.png
