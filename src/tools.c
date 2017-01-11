#include <stdlib.h>
#include "tools.h"

char *splitStr(const char *str, const int from, const int to){
	char *value = malloc(sizeof(char) * (to - from));

	int indexValue = 0;
	int indexStr = from;
	for (indexStr = from; indexStr < to; ++indexStr){
		value[indexValue] = str[indexStr];
		indexValue++;
	}

	return value;
}

int str_size(char *str, int limit){
	int size = 0;
	while(str[size]!='\0' && size < limit) size++;
	return size;
}

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

int itoa_formated(char str[], int value, int size, char default_char){
	int str_length = itoa(str, value);
	
	for (int i = size - 1; i >= str_length; --i)
		str[i] = str[(i + str_length)%size];

	for (int i = 0; i < size - str_length; ++i)
		str[i] = default_char;
	
	return str_length;
}

int slice_to_char(int size, char *str, int limit, char *buffer, int from, char to){
	int buffer_size = 0, i = from;
	while(str[i] != to && i < size && buffer_size < limit){
		buffer[buffer_size] = str[i];
		i++;
		buffer_size++;
	}
	return buffer_size;
}

int slice(const char *txt_str, char *buffer_str, const int from, const int to){
	int i, size = 0;
	for (i = from; i < to; ++i)
		buffer_str[size++] = txt_str[i];
	return size;
}


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