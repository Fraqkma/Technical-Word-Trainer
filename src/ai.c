/* src/ai.c
   Simple rule-based analyzer:
   - Reads data/review.txt lines: username|word|timestamp|count
   - Counts how many times this username had each 'word' in review
   - Reads data/vocab.txt to map words -> tags
   - Counts tag frequencies for this user's wrong words
   - Prints top words and top tags, and recommends other words from top tags
*/

#include <stdio.h>
#include <string.h>
#include "../include/ai.h"

#define MAX_WORDS 1000
#define MAX_WORD_LEN 128
#define MAX_DEF_LEN 512
#define MAX_TAG_LEN 64
#define MAX_RECS 10

/* simple structure to hold vocab info loaded from file */
typedef struct {
    char word[MAX_WORD_LEN];
    char def[MAX_DEF_LEN];
    char tags[MAX_TAG_LEN];
} VocabSimple;

/* Helper: load vocab file into array, return count */
static int load_vocab_simple(const char *vocab_path, VocabSimple arr[], int max) {
    FILE *f = fopen(vocab_path, "r");
    if (!f) return 0;
    char line[1024];
    int count = 0;
    while (fgets(line, sizeof(line), f) && count < max) {
        if (line[0] == '#') continue;
        char w[MAX_WORD_LEN] = {0}, d[MAX_DEF_LEN] = {0}, t[MAX_TAG_LEN] = {0};
        /* parse "word|def|tags" */
        if (sscanf(line, " %127[^|]|%511[^|]|%63[^\n]", w, d, t) >= 1) {
            strncpy(arr[count].word, w, MAX_WORD_LEN-1);
            strncpy(arr[count].def, d, MAX_DEF_LEN-1);
            strncpy(arr[count].tags, t, MAX_TAG_LEN-1);
            count++;
        }
    }
    fclose(f);
    return count;
}

/* Helper: find vocab index by word, or -1 */
static int vocab_index_by_word(VocabSimple arr[], int n, const char *word) {
    for (int i = 0; i < n; ++i) {
        if (strcmp(arr[i].word, word) == 0) return i;
    }
    return -1;
}

/* main AI function */
void ai_recommend_for_user(const char *username, const char *vocab_path, const char *review_path) {
    if (!username) return;

    /* load vocab */
    VocabSimple vocab[MAX_WORDS];
    int vcount = load_vocab_simple(vocab_path, vocab, MAX_WORDS);

    /* arrays to count wrongs per word (index into vocab) */
    int wrong_count[MAX_WORDS];
    for (int i=0;i<MAX_WORDS;i++) wrong_count[i]=0;

    /* If a reviewed word is not in vocab, store separately */
    char other_words[MAX_WORDS][MAX_WORD_LEN];
    int other_count[MAX_WORDS];
    int other_n = 0;
    for (int i=0;i<MAX_WORDS;i++) other_count[i]=0;

    /* read review file */
    FILE *rf = fopen(review_path, "r");
    if (!rf) {
        printf("No review data found (file %s missing).\n", review_path);
        return;
    }

    char line[512];
    while (fgets(line, sizeof(line), rf)) {
        if (line[0] == '#') continue;
        char user[128] = {0}, word[128] = {0};
        long ts = 0;
        int cnt = 0;
        /* expected format: username|word|timestamp|count */
        if (sscanf(line, " %127[^|]|%127[^|]|%ld|%d", user, word, &ts, &cnt) >= 2) {
            if (strcmp(user, username) == 0) {
                /* find word in vocab */
                int vi = vocab_index_by_word(vocab, vcount, word);
                if (vi >= 0) {
                    wrong_count[vi] += (cnt>0?cnt:1);
                } else {
                    /* record other word */
                    int found = -1;
                    for (int k=0;k<other_n;k++) if (strcmp(other_words[k], word)==0) { found=k; break; }
                    if (found>=0) other_count[found] += (cnt>0?cnt:1);
                    else {
                        if (other_n < MAX_WORDS) {
                            strncpy(other_words[other_n], word, MAX_WORD_LEN-1);
                            other_count[other_n] = (cnt>0?cnt:1);
                            other_n++;
                        }
                    }
                }
            }
        }
    }
    fclose(rf);

    /* aggregate tag counts using wrong_count */
    typedef struct { char tag[MAX_TAG_LEN]; int count; } TagCount;
    TagCount tags[MAX_WORDS];
    int tag_n = 0;
    for (int i=0;i<vcount;i++) {
        if (wrong_count[i] > 0 && vocab[i].tags[0]) {
            /* tags may be comma separated; split by comma or space */
            char tagcopy[MAX_TAG_LEN];
            strncpy(tagcopy, vocab[i].tags, MAX_TAG_LEN-1); tagcopy[MAX_TAG_LEN-1]=0;
            char *p = tagcopy;
            char *tok;
            while ((tok = strtok(p, ",; ")) != NULL) {
                p = NULL;
                /* trim */
                while (tok[0]==' ') tok++;
                int found = -1;
                for (int t=0;t<tag_n;t++) if (strcmp(tags[t].tag, tok)==0) { found = t; break; }
                if (found>=0) tags[found].count += wrong_count[i];
                else {
                    if (tag_n < MAX_WORDS) {
                        strncpy(tags[tag_n].tag, tok, MAX_TAG_LEN-1);
                        tags[tag_n].count = wrong_count[i];
                        tag_n++;
                    }
                }
            }
        }
    }

    /* find top wrong words (indices) */
    int top_idx[MAX_RECS];
    int top_n = 0;
    for (int i=0;i<vcount;i++) {
        if (wrong_count[i] <= 0) continue;
        /* insert into top list (simple selection) */
        int pos = top_n;
        while (pos > 0 && wrong_count[top_idx[pos-1]] < wrong_count[i]) pos--;
        if (top_n < MAX_RECS) {
            for (int k=top_n;k>pos;k--) top_idx[k]=top_idx[k-1];
            top_idx[pos]=i;
            top_n++;
        } else if (pos < top_n) {
            for (int k=top_n-1;k>pos;k--) top_idx[k]=top_idx[k-1];
            top_idx[pos]=i;
        }
    }

    /* find top tags */
    int top_tag_idx[MAX_RECS];
    int top_tag_n = 0;
    for (int i=0;i<tag_n;i++) {
        int pos = top_tag_n;
        while (pos > 0 && tags[top_tag_idx[pos-1]].count < tags[i].count) pos--;
        if (top_tag_n < MAX_RECS) {
            for (int k=top_tag_n;k>pos;k--) top_tag_idx[k]=top_tag_idx[k-1];
            top_tag_idx[pos]=i;
            top_tag_n++;
        } else if (pos < top_tag_n) {
            for (int k=top_tag_n-1;k>pos;k--) top_tag_idx[k]=top_tag_idx[k-1];
            top_tag_idx[pos]=i;
        }
    }

    /* PRINT RESULTS */
    printf("\n=== AI Review Analysis for %s ===\n", username);

    if (top_n == 0 && other_n == 0) {
        printf("No review history found for this user.\n");
        return;
    }

    if (top_n > 0) {
        printf("Words you often miss:\n");
        for (int i=0;i<top_n;i++) {
            int idx = top_idx[i];
            printf(" - %s (wrong count: %d)\n", vocab[idx].word, wrong_count[idx]);
        }
    }

    if (other_n > 0) {
        printf("Other words not in vocab file (from review):\n");
        for (int i=0;i<other_n && i<MAX_RECS;i++) {
            printf(" - %s (wrong count: %d)\n", other_words[i], other_count[i]);
        }
    }

    if (top_tag_n > 0) {
        printf("\nTags you struggle with most:\n");
        for (int i=0;i<top_tag_n;i++) {
            int ti = top_tag_idx[i];
            printf(" - %s (score: %d)\n", tags[ti].tag, tags[ti].count);
        }

        /* Recommend words from top tag (take up to 5) */
        printf("\nRecommended words to review from top tag(s):\n");
        int rec_shown = 0;
        for (int t=0;t<top_tag_n && rec_shown < 10; t++) {
            const char *thistag = tags[top_tag_idx[t]].tag;
            for (int i=0;i<vcount && rec_shown < 10;i++) {
                if (vocab[i].tags && strstr(vocab[i].tags, thistag) != NULL) {
                    /* skip words already in top wrong list (optional) */
                    int already = 0;
                    for (int k=0;k<top_n;k++) if (strcmp(vocab[top_idx[k]].word, vocab[i].word)==0) { already=1; break; }
                    if (!already) {
                        printf(" - %s : %s\n", vocab[i].word, vocab[i].def);
                        rec_shown++;
                    }
                }
            }
        }
    }

    printf("\nAI Suggestion: Focus on practicing the top tags above. Try a targeted quiz on those topics.\n");
}
