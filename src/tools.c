#include <stdlib.h>
#include "tools.h"

/**
 * renvoie taille d'une chaine de caractere
 * @param str
 * @param limit
 * @return
 */
int str_size(char *str, int limit){
	int size = 0;
	while(str[size]!='\0' && size < limit) size++;
	return size;
}

/**
 * copie une string dans une autre
 * @param str1
 * @param str2
 * @param size
 */
void str_cpy(char * str1, char * str2, int size){
	for(int i = 0; i < size; i++)
		str1[i] = str2[i];
	str1[size] = '\0';
}

/**
 * convertir entier en chaine de caractere
 * la chaine str passée en 1er argument sera modifier par rapport a l'entier value en 2nd argument
 * retourne la longueur de str
 * @param str
 * @param value
 * @return
 */
int itoa(char *str, int value){
	if (str == NULL)
		return -1;

	char buffer[11];
	int pos = 0, outpos = 0, neg = 0;
	
	if (value < 0) {
		neg = 1;
		value = -value;
	}

	do {
		buffer[pos++] = '0' + value % 10;
		value /= 10;
	} while (value > 0);

	if (neg == 1)
		buffer[pos++] = '-';

	while(outpos < pos){
		str[outpos] = buffer[pos - outpos - 1];
		outpos ++;
	}
	str[outpos] = '\0';
	return pos;
}

/**
 * ecrir dans la chaine de caractere passé en parametre la conversion d'un entier
 * de plus permet d'appliquer un certain format, cad la longueur et les caractere par default
 * @param str
 * @param value
 * @param size
 * @param default_char
 * @return
 */
int itoa_formated(char str[], int value, int size, char default_char){
	int str_length = itoa(str, value);
	
	for (int i = size - 1; i >= str_length; --i)
		str[i] = str[(i + str_length)%size];

	for (int i = 0; i < size - str_length; ++i)
		str[i] = default_char;
	
	return str_length;
}

/**
 * ecrit dans le buffer une portion de str allant d'un certain indice à un certain caractere
 * @param size
 * @param str
 * @param limit
 * @param buffer
 * @param from
 * @param to
 * @return
 */
int slice_to_char(int size, char *str, int limit, char *buffer, int from, char to){
	int buffer_size = 0, i = from;
	while(str[i] != to && i < size && buffer_size < limit){
		buffer[buffer_size] = str[i];
		i++;
		buffer_size++;
	}
	return buffer_size;
}

/**
 * ecrit dans str2 une portion de str1 allant d'un certain indice à un autre
 * @param str1
 * @param str2
 * @param from
 * @param to
 * @return
 */
int slice(const char *str1, char *str2, const int from, const int to){
	int i, size = 0;
	for (i = from; i < to; ++i)
		str2[size++] = str1[i];
	return size;
}

/**
 * compare deux chaines de caractere en verifiant si str1 commence avec str2
 * @param str1_size
 * @param str1
 * @param str2_size
 * @param str2
 * @return
 */
int str_start_with(const int str1_size, const char *str1, const int str2_size, const char *str2){
	if(str1_size < str2_size)
		return -1;
	int i = 0, ok = 0;
	while(i < str2_size && ok == 0){
		ok = str1[i] == str2[i] ? 0 : -1;
		++i;
	} 
	return ok;
}