\
#include <stdio.h>
#include <string.h>
#include "../include/vocab.h"

/* Load vocab from file into array. Simple parsing using sscanf. */
int vocab_load(const char *path, VocabEntry list[], int max) {
    FILE *f = fopen(path, "r");
    if (!f) return 0;
    char line[512];
    int count = 0;
    while (fgets(line, sizeof(line), f) && count < max) {
        if (line[0] == '#') continue;
        /* parse word|definition|tags */
        char w[MAX_WORD_LEN], d[MAX_DEF_LEN], t[MAX_TAGS_LEN];
        w[0]=d[0]=t[0]=0;
        /* sscanf reads up to '|' using scanset */
        sscanf(line, " %63[^|]|%255[^|]|%63[^\n]", w, d, t);
        if (w[0] == 0) continue;
        strncpy(list[count].word, w, MAX_WORD_LEN-1); list[count].word[MAX_WORD_LEN-1]=0;
        strncpy(list[count].def, d, MAX_DEF_LEN-1); list[count].def[MAX_DEF_LEN-1]=0;
        strncpy(list[count].tags, t, MAX_TAGS_LEN-1); list[count].tags[MAX_TAGS_LEN-1]=0;
        count++;
    }
    fclose(f);
    return count;
}

/* Save array back to file */
int vocab_save(const char *path, VocabEntry list[], int count) {
    FILE *f = fopen(path, "w");
    if (!f) return 0;
    fprintf(f, "# word|definition|tags\n");
    int i;
    for (i = 0; i < count; ++i) {
        fprintf(f, "%s|%s|%s\n", list[i].word, list[i].def, list[i].tags);
    }
    fclose(f);
    return 1;
}

int vocab_find(VocabEntry list[], int count, const char *word) {
    int i;
    for (i = 0; i < count; ++i) {
        if (strcmp(list[i].word, word) == 0) return i;
    }
    return -1;
}

int vocab_add(VocabEntry list[], int *count, const char *word, const char *def, const char *tags) {
    if (*count >= MAX_WORDS) return 0;
    if (vocab_find(list, *count, word) != -1) return 0; /* already exists */
    strncpy(list[*count].word, word, MAX_WORD_LEN-1); list[*count].word[MAX_WORD_LEN-1]=0;
    strncpy(list[*count].def, def, MAX_DEF_LEN-1); list[*count].def[MAX_DEF_LEN-1]=0;
    strncpy(list[*count].tags, tags, MAX_TAGS_LEN-1); list[*count].tags[MAX_TAGS_LEN-1]=0;
    (*count)++;
    return 1;
}

int vocab_delete(VocabEntry list[], int *count, const char *word) {
    int idx = vocab_find(list, *count, word);
    if (idx == -1) return 0;
    /* move last element here to keep contiguous array */
    if (idx != (*count)-1) list[idx] = list[(*count)-1];
    (*count)--;
    return 1;
}

void vocab_list(VocabEntry list[], int count) {
    int i;
    if (count == 0) { printf("No vocabulary.\n"); return; }
    puts("");
    puts("--- Vocabulary ---");
    for (i = 0; i < count; ++i) {
        printf("%d) %s - %s [%s]\n", i+1, list[i].word, list[i].def, list[i].tags);
    }
}
