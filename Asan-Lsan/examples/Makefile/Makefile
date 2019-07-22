cc = gcc
prom = test
deps = $(shell find ./ -name "*.h")
src = $(shell find ./ -name "*.c")
obj = $(src:%.c=%.o) 

$(prom): $(obj)
	$(cc) -o $(prom) $(src) -fsanitize=address -fno-omit-frame-pointer -fsanitize=address

#%.o: %.c $(deps)
#	$(cc) -c $< -o $@ -g -O2 -fsanitize=address -fno-omit-frame-pointer -fsanitize=address

clean:
	rm -rf $(obj) $(prom)
