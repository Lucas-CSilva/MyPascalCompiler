CFLAGS = -I. -g

relocatable = mypas.o lexer.o parser.o keywords.o symtab.o

mypas: $(relocatable)
	$(CC) -o mypas $(relocatable)

clean:
	$(RM) *.o

mostlyclean: clean
	$(RM) *~ mypas