#files .o, executable, parametres
OBJS = main.o checks.o solve_functions.o solve.o
EXEC = crossw
ARGS = #nothing for this program

$(EXEC): $(OBJS)
	gcc $(OBJS) -o $(EXEC)

clean:
	@echo "Removing everything but the source files"
	rm -f $(EXEC) $(OBJS)
