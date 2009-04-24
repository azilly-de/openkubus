int _cleanup(FILE *file, int ret);
void _update_passwd(FILE *file, char *num, int k);
int _chomp(char *str);
int _parse_passwd(FILE *file, const char *user, char **key, char **data, char **num);
