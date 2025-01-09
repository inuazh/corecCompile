CC = gcc
LEX = flex
YACC = bison
CFLAGS = -Wall -g -Wno-unused-function
TARGET = corec


OBJS = lex.yy.o corec.tab.o main.o


all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) 

lex.yy.c: corec.l corec.tab.h
	$(LEX) -o $@ $<

corec.tab.c corec.tab.h: corec.y
	$(YACC) -d -o corec.tab.c $<

main.o: main.c corec.tab.h
	$(CC) $(CFLAGS) -c $<

corec.tab.o: corec.tab.c corec.tab.h
	$(CC) $(CFLAGS) -c $<

lex.yy.o: lex.yy.c corec.tab.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) lex.yy.c corec.tab.c corec.tab.h $(TARGET)

.PHONY: all clean
