# target: dependencies
#     command

# variable to define flags once and reuse them
FLAGS=-g -Wall -std=c99

# Compiles c code into excecutible
ray1: 2dRaytracer.c
	gcc ${FLAGS} -o ray1 2dRaytracer.c -lm

# Removes executible and any produced images
.PHONY: clean
clean:
	rm -f ray1 *.png
