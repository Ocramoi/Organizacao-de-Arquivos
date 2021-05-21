##
# SCC025 - Trabalho 1
#
# @file *.c
# @version 0.1

ARQ_SAIDA=programaTrab

all:
	gcc *.c -o $(ARQ_SAIDA)

run:
	./$(ARQ_SAIDA)

debug:
	gcc *.c -o $(ARQ_SAIDA) -g

val:
	valgrind --leak-check=full --show-leak-kinds=all ./$(ARQ_SAIDA)

clear:
	rm $(ARQ_SAIDA) Entrega.zip

Entrega:
	zip Entrega.zip *.c *.h Makefile

# end
