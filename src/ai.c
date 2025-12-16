/* src/ai.c
   Simple rule-based analyzer:
   - Reads data/review.txt lines: username|word|timestamp|count
   - Counts how many times this username had each 'word' in review
   - Reads data/vocab.txt to map words -> tags
   - Counts tag frequencies for this user's wrong words
   - Prints top words and top tags, and recommends other words from top tags
*/

#include <stdio.h> // Provides standard input/output functions 
#include <string.h>  // Provides string handling functions 
#include "../include/ai.h" // Header file for the AI module 
/* Maximum limits used throughout the program */
#define MAX_WORDS 1000     
#define MAX_WORD_LEN 128
#define MAX_DEF_LEN 512
#define MAX_TAG_LEN 64
#define MAX_RECS 10
/* ---------------------------------------------------- */
/* Data structure for storing vocabulary loaded from file */
typedef struct {
    char word[MAX_WORD_LEN]; // The vocabulary word
    char def[MAX_DEF_LEN]; // Definition of the word
    char tags[MAX_TAG_LEN]; // Tags associated with the word 
} VocabSimple;

/* ---------------------------------------------------- */
/* Load vocabulary data from a file into an array */
/* File format: word|definition|tags */
/* Returns the number of vocabulary entries loaded */
static int load_vocab_simple(const char *vocab_path, VocabSimple arr[], int max) {
    FILE *f = fopen(vocab_path, "r");// Open vocabulary file in read mode
    if (!f) return 0; // If file cannot be opened, return 0
    char line[1024];// Buffer for reading each line from file
    int count = 0;  // Counter for number of vocab entries loaded
     // Read file line by line until EOF or max limit reached
    while (fgets(line, sizeof(line), f) && count < max) {
        if (line[0] == '#') continue;// Skip comment lines
        char w[MAX_WORD_LEN] = {0}, d[MAX_DEF_LEN] = {0}, t[MAX_TAG_LEN] = {0};// Temporary storage for word,Temporary storage for definition,Temporary storage for tags
                 /* Parse a line using '|' as delimiter */
        if (sscanf(line, " %127[^|]|%511[^|]|%63[^\n]", w, d, t) >= 1) {
           // Copy parsed data into vocab array
            strncpy(arr[count].word, w, MAX_WORD_LEN-1);
            strncpy(arr[count].def, d, MAX_DEF_LEN-1);
            strncpy(arr[count].tags, t, MAX_TAG_LEN-1);
            count++;// Increase vocabulary count
        }
    }
    fclose(f);// Close vocabulary file
    return count;// Return number of loaded vocabulary entries
}
/* ---------------------------------------------------- */
/* Find the index of a word in the vocabulary array */
/* Returns index if found, or -1 if not found */
static int vocab_index_by_word(VocabSimple arr[], int n, const char *word) {
   // Loop through all vocabulary entries
    for (int i = 0; i < n; ++i) {
       // Compare current vocab word with target word
        if (strcmp(arr[i].word, word) == 0) return i;// Word found, return its index
    }
    return -1;// Word not found in vocabulary
}

/* ---------------------------------------------------- */
/* Main AI function */
/* Analyzes user review history and generates recommendations */
void ai_recommend_for_user(const char *username, const char *vocab_path, const char *review_path) {
    if (!username) return;// Exit if username is NULL

    /* load vocab */
    VocabSimple vocab[MAX_WORDS];  // Array to store vocabulary data
    int vcount = load_vocab_simple(vocab_path, vocab, MAX_WORDS); // Number of loaded vocab words

    /* Track how many times each vocabulary word was answered incorrectly */
    int wrong_count[MAX_WORDS];
    for (int i=0;i<MAX_WORDS;i++) wrong_count[i]=0;// Initialize all counts to zero

    /* Store words found in review file but not in vocabulary file */
    char other_words[MAX_WORDS][MAX_WORD_LEN];// Words not in vocab
    int other_count[MAX_WORDS]; // Wrong count for those words
    int other_n = 0;// Number of such words
    // Initialize counts for non-vocabulary words
    for (int i=0;i<MAX_WORDS;i++) other_count[i]=0;

    /* Open the review history file */
    FILE *rf = fopen(review_path, "r");
    if (!rf) {
        printf("No review data found (file %s missing).\n", review_path);
        return;// Stop if review file cannot be opened
    }

    char line[512];// Buffer for reading review file line by line
    // Read each line from review file
    while (fgets(line, sizeof(line), rf)) {
        if (line[0] == '#') continue;// Ignore comment lines
        char user[128] = {0}, word[128] = {0};// Username from file,Reviewed word
        long ts = 0;// Timestamp (not used in current logic)
        int cnt = 0;// Number of wrong attempts

        /* expected format: username|word|timestamp|count */
        if (sscanf(line, " %127[^|]|%127[^|]|%ld|%d", user, word, &ts, &cnt) >= 2) {
           // Process only records that match the current user
            if (strcmp(user, username) == 0) {
               // Find the word in the vocabulary list
                int vi = vocab_index_by_word(vocab, vcount, word);
                if (vi >= 0) {
                   // If the word exists in vocabulary,
                    // increase its wrong count
                    wrong_count[vi] += (cnt>0?cnt:1);
                } else {
                    // If the word is NOT in vocabulary,
                    // store it separately
                    int found = -1;
                    // Check if the word was already recorded
                    for (int k=0;k<other_n;k++) if (strcmp(other_words[k], word)==0) { found=k; break; }
                    if (found>=0) other_count[found] += (cnt>0?cnt:1);
                       // Word already exists → increase its count
                    else {
                        if (other_n < MAX_WORDS) {
                           // New word → add to list
                            strncpy(other_words[other_n], word, MAX_WORD_LEN-1);
                            other_count[other_n] = (cnt>0?cnt:1);
                            other_n++;
                        }
                    }
                }
            }
        }
    }
    fclose(rf);// Close review file

  /* ---------------------------------------------------- */
    /* Analyze which tags the user struggles with the most */
    typedef struct { char tag[MAX_TAG_LEN]; int count; } TagCount;// Total wrong count for this tag
    TagCount tags[MAX_WORDS];// Array to store tag statistics
    int tag_n = 0; // Number of unique tags found
       // Loop through all vocabulary words
    for (int i=0;i<vcount;i++) {
        // Only analyze words with at least one mistake
        if (wrong_count[i] > 0 && vocab[i].tags[0]) {
            /* tags may be comma separated; split by comma or space */
            char tagcopy[MAX_TAG_LEN];
            strncpy(tagcopy, vocab[i].tags, MAX_TAG_LEN-1); tagcopy[MAX_TAG_LEN-1]=0;
            char *p = tagcopy;
            char *tok;
           // Split tags using comma, semicolon, or space
            while ((tok = strtok(p, ",; ")) != NULL) {
                p = NULL;
                /* trim */
                while (tok[0]==' ') tok++;
                int found = -1;
               // Check if this tag already exists
                for (int t=0;t<tag_n;t++) if (strcmp(tags[t].tag, tok)==0) { found = t; break; }
                if (found>=0) tags[found].count += wrong_count[i]; // Tag already exists → add to its score
                else {  // New tag → add to tag list
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

   
    /* ---------------------------------------------------- */
    /* Display analysis result to the user */
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
