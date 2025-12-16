#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include "../include/store.h"

#ifdef _WIN32
  #include <direct.h> /* for _mkdir on Windows */ //ChatGPT recommend
#endif

//Ensure data directory exists return 1 on success//
int ensure_data_dir(void) {
    struct stat st;
    if (stat("data", &st) == -1) {
    #ifdef _WIN32
        _mkdir("data");
    #else
        mkdir("data", 0755);
    #endif
    }
    return 1;
}

// check if user or admin if user set function is_admin to 1 and return 1 if credential match from file 
int users_verify(const char *username, const char *password, int *is_admin) {
    if (!username || !password || !is_admin) return 0;
    FILE *f = fopen("data/users.txt", "r");
    if (!f) return 0;

    char line[512];
    while (fgets(line, sizeof(line), f)) {
        if (line[0] == '#') continue;
        char user[128] = {0}, pass[128] = {0}, role[64] = {0};
        //check user pass role
        if (sscanf(line, " %127[^,],%127[^,],%63[^\n]", user, pass, role) >= 2) {
            if (strcmp(user, username) == 0 && strcmp(pass, password) == 0) {
                if (strcmp(role, "admin") == 0) *is_admin = 1;
                else *is_admin = 0;
                fclose(f);
                return 1;
            }
        }
    }
    fclose(f);
    return 0;
}

// register for new user
int users_register(const char *username, const char *password) {
    if (!username || !password) return 0;

    FILE *f = fopen("data/users.txt", "a+");
    if (!f) return 0;

    // check line existing
    rewind(f);
    char line[512];
    while (fgets(line, sizeof(line), f)) {
        if (line[0] == '#') continue;
        char user[128] = {0};
        if (sscanf(line, " %127[^,],%*s", user) == 1) {
            if (strcmp(user, username) == 0) {
                fclose(f);
                return 0;
            }
        }
    }

    fprintf(f, "%s,%s,user\n", username, password);
    fclose(f);
    return 1;
}

// ppend a wrong answer record for review 
int review_record(const char *username, const char *word) {
    if (!username || !word) return 0;
    FILE *f = fopen("data/review.txt", "a");
    if (!f) return 0;

    time_t t = time(NULL);
    fprintf(f, "%s|%s|%ld|1\n", username, word, (long)t);
    fclose(f);
    return 1;
}
