char *splitStr(const char *str, const int from, const int to);
int itoa(char *str, int value);
int itoa_formated(char str[], int value, int size, char default_char);
int str_size(char *str, int limit);
int slice_to_char(int size, char *str, int limit, char *buffer, int from, char to);
int slice(const char *txt_str, char *buffer_str, const int from, const int to);
int str_start_with(const int str1_size, const char *str1, const int str2_size, const char *str2);