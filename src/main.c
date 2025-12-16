#include <stdio.h>
#include <string.h>
#include "../include/ai.h"
#include "../include/cli.h"
#include "../include/vocab.h"
#include "../include/store.h"
#include "../include/quiz.h"

int main(void) {
    ensure_data_dir();

    /* load vocabulary into fixed array */
    VocabEntry vocab[MAX_WORDS];
    int vcount = vocab_load("data/vocab.txt", vocab, MAX_WORDS);

    char uname[64], pwd[64];
    while (1) {
        show_main_menu();
        int c = read_int("Choose: ");
        if (c == 1) {
            read_line("Username: ", uname, sizeof(uname));
            read_line("Password: ", pwd, sizeof(pwd));
            int is_admin = 0;
            if (users_verify(uname, pwd, &is_admin)) {
                printf("Welcome %s!\n", uname);
                if (is_admin) {
                    int a;
                    do {
                        show_admin_menu();
                        a = read_int("Choose: ");
                        if (a == 1) {
                            char w[64], d[256], t[64];
                            read_line("Word: ", w, sizeof(w));
                            read_line("Definition: ", d, sizeof(d));
                            read_line("Tags: ", t, sizeof(t));
                            if (vocab_add(vocab, &vcount, w, d, t)) {
                                vocab_save("data/vocab.txt", vocab, vcount);
                                printf("Added.\n");
                            } else {
                                printf("Cannot add (exists or full).\n");
                            }
                        } else if (a == 2) {
                            char w[64];
                            read_line("Word to delete: ", w, sizeof(w));
                            if (vocab_delete(vocab, &vcount, w)) {
                                vocab_save("data/vocab.txt", vocab, vcount);
                                printf("Deleted.\n");
                            } else {
                                printf("Not found.\n");
                            }
                        } else if (a == 3) {
                            vocab_list(vocab, vcount);
                        }
                    } while (a != 4);
                } else {
                    int uc;
                    do {
                        show_user_menu();
                        uc = read_int("Choose: ");
                        if (uc == 1) {
                            char tag[64];
                            read_line("Enter tag to search (or empty to list all): ", tag, sizeof(tag));
                            if (tag[0] != 0) {
                                int i;
                                for (i = 0; i < vcount; i++) {
                                    if (strstr(vocab[i].tags, tag)) {
                                        printf("%s - %s\n", vocab[i].word, vocab[i].def);
                                    }
                                }
                            } else {
                                vocab_list(vocab, vcount);
                            }
                        } else if (uc == 2) {
                            int n = read_int("Number of questions: ");
                            if (n > 0) quiz_run(vocab, vcount, uname, n);
                        } else if (uc == 3) {
                            ai_recommend_for_user(uname,"data/vocab.txt","data/review.txt");
                        } else if (uc == 4) {
                            extern void print_leaderboard(void);
                            print_leaderboard();
                        }
                    } while (uc != 5);
                }
            } else {
                printf("Invalid credentials.\n");
            }
        } else if (c == 2) {
            char u[64], p[64];
            read_line("Choose username: ", u, sizeof(u));
            read_line("Choose password: ", p, sizeof(p));
            if (users_register(u, p)) printf("Registered.\n");
            else printf("Register failed.\n");
        } else if (c == 3) {
            break;
        }
    }

    return 0;
}
