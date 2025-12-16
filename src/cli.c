#include <stdio.h>
#include <string.h>
#include <stdlib.h>     // <-- สำคัญ! สำหรับ atoi()
#include "../include/cli.h"

char* read_line(const char *prompt, char *buf, int bufsize) {
    if (prompt) {
        printf("%s", prompt);
        fflush(stdout);
    }

    if (!fgets(buf, bufsize, stdin)) {
        buf[0] = 0;
        return buf;
    }

    /* remove newline */
    size_t i;
    for (i = 0; i < bufsize && buf[i] != 0; ++i) {
        if (buf[i] == '\n') {
            buf[i] = 0;
            break;
        }
    }

    return buf;
}

int read_int(const char *prompt) {
    char buf[32];
    read_line(prompt, buf, sizeof(buf));
    return atoi(buf);   // ตอนนี้ไม่ error แล้ว
}

void show_main_menu(void) {
    puts("");
    puts("=== Technical Vocabulary Trainer ===");
    puts("1) Login");
    puts("2) Register");
    puts("3) Exit");
}

void show_admin_menu(void) {
    puts("");
    puts("--- Admin Menu ---");
    puts("1) Add word");
    puts("2) Delete word");
    puts("3) List words");
    puts("4) Logout");
}

void show_user_menu(void) {
    puts("");
    puts("--- User Menu ---");
    puts("1) Browse/Search");
    puts("2) Take Quiz");
    puts("3) Review Wrong Words");
    puts("4) View Progress / Leaderboard");
    puts("5) Logout");
}
