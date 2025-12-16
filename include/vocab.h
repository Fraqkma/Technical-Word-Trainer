#ifndef VOCAB_H
#define VOCAB_H
#define MAX_WORDS 1000
#define MAX_WORD_LEN 64
#define MAX_DEF_LEN 256
#define MAX_TAGS_LEN 64

typedef struct {
    char word[MAX_WORD_LEN];
    char def[MAX_DEF_LEN];
    char tags[MAX_TAGS_LEN];
} VocabEntry;

int vocab_load(const char *path, VocabEntry list[], int max);
int vocab_save(const char *path, VocabEntry list[], int count);
int vocab_find(VocabEntry list[], int count, const char *word); // returns index or -1
int vocab_add(VocabEntry list[], int *count, const char *word, const char *def, const char *tags);
int vocab_delete(VocabEntry list[], int *count, const char *word);
void vocab_list(VocabEntry list[], int count);
#endif
