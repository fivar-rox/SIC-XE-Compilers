## compiles and creates a parser
all: 190101061_lexer.l  190101061_parser.y
	flex 190101061_lexer.l
	yacc -d 190101061_parser.y
	g++ y.tab.c lex.yy.c -o parser

## cleans all the files which are created upon compilation of the parser, returns the folder into a fresh state
clean:
	rm -f parser lex.yy.c y.tab.c y.tab.h tokenizer symbol_table.txt

