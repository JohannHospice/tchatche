#include <stdlib.h>

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