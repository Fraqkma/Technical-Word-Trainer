#include <stdio.h>
#include <string.h>
#include "../include/cli.h"
#include "../include/vocab.h"
#include "../include/store.h"
#include "../include/quiz.h"

int main(void) {
    ensure_data_dir();//Check if the file is exist or not if not create the new one

    //Load vocabulary to fixed array and its a sturct data
    VocabEntry vocab[MAX_WORDS];
    int vcount = vocab_load("data/vocab.txt", vocab, MAX_WORDS);

    char uname[64], pwd[64];         //declare array to contains username and password
    while (1) {
        show_main_menu();            //Print menu
        int c = read_int("Choose: ");//Input choice
        if (c == 1) {                //Login
            read_line("Username: ", uname, sizeof(uname));
            read_line("Password: ", pwd, sizeof(pwd));
            int is_admin = 0;//flags

            if (users_verify(uname, pwd, &is_admin)) { //Recall function User verify เพื่อดูว่าเป็น admin หรือ user โดยการตั้ง flag ไว้
                printf("Welcome %s!\n", uname);
                if (is_admin) { //if admin equal to 1
                    int a;      //a int for choice
                    do {
                        show_admin_menu();
                        a = read_int("Choose: ");
                        //Choice 1 equal to add vocab
                        if (a == 1) {
                            char w[64], d[256], t[64];
                            //input word definition and tag and each one has malloc memory of its size
                            read_line("Word: ", w, sizeof(w));              
                            read_line("Definition: ", d, sizeof(d));        
                            read_line("Tags: ", t, sizeof(t));   

                            if (vocab_add(vocab, &vcount, w, d, t)) {   //Add vocab       
                                vocab_save("data/vocab.txt", vocab, vcount);
                                printf("Added.\n");
                            } else {                                    //Handle the exist word or full array
                                printf("Cannot add (exists or full).\n");
                            }
                        //Choose to delete the exist word
                        } else if (a == 2) {
                            char w[64];
                            read_line("Word to delete: ", w, sizeof(w));
                            //Delete exist vocab from recieve input w
                            if (vocab_delete(vocab, &vcount, w)) {
                                vocab_save("data/vocab.txt", vocab, vcount);
                                printf("Deleted.\n");
                            } else {
                                printf("Not found.\n");
                            }
                        //Show all listed word
                        } else if (a == 3) {
                            vocab_list(vocab, vcount);
                        }
                    } while (a != 4); //If choice = 4, exit program
                } else {              //if flag = 0 mean you are user
                    int uc;  //declare user choice
                    do {
                        show_user_menu();
                        uc = read_int("Choose: ");
                        if (uc == 1) { //search a word
                            char tag[64]; //declare a user choosen word
                            read_line("Enter tag to search (or empty to list all): ", tag, sizeof(tag));
                            if (tag[0] != 0) {
                                int i;
                                for (i = 0; i < vcount; i++) {
                                    if (strstr(vocab[i].tags, tag)) {
                                        printf("%s - %s\n", vocab[i].word, vocab[i].def);
                                    }
                                }
                            } else { //If you not input any word show all the vocab
                                vocab_list(vocab, vcount);
                            }
                        } else if (uc == 2) { //Run the quiz system
                            int n = read_int("Number of questions: ");
                            if (n > 0) quiz_run(vocab, vcount, uname, n);
                        } else if (uc == 3) { //Review your test
                            printf("Review file: data/review.txt\n");
                        } else if (uc == 4) { //Print leader board
                            extern void print_leaderboard(void);
                            print_leaderboard();
                        }
                    } while (uc != 5); //If you enter 5 exit program
                }
            } else {
                printf("Invalid credentials.\n");//Print error when choice is not in range
            }
        } else if (c == 2) { //Choice 2 registered account
            char u[64], p[64];
            read_line("Choose username: ", u, sizeof(u));
            read_line("Choose password: ", p, sizeof(p));
            if (users_register(u, p)) printf("Registered.\n");
            else printf("Register failed.\n");
        } else if (c == 3) { //Choice 3 Exit 
            break;
        }
    }

    return 0;
}
