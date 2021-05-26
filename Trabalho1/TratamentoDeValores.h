#ifndef __TRATAMENTODEVALORES_H_
#define __TRATAMENTODEVALORES_H_

/* Trata data de [original] para a saída no formato correto */
char *trataData(char *original);

/* Remove aspas exteriores à string [original] com máximo de [limite] caracteres */
char *trataAspas(char *original, unsigned int limite);

#endif // __TRATAMENTODEVALORES_H_
