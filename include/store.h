#ifndef STORE_H
#define STORE_H
int ensure_data_dir(void);
int users_verify(const char *username, const char *password, int *is_admin);
int users_register(const char *username, const char *password);
int review_record(const char *username, const char *word);
#endif
