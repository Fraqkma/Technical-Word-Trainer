#ifndef CLI_H
#define CLI_H
char* read_line(const char *prompt, char *buf, int bufsize);
int read_int(const char *prompt);
void show_main_menu(void);
void show_admin_menu(void);
void show_user_menu(void);
#endif
