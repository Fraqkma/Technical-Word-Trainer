\
#include <stdio.h>
#include "../include/vocab.h"

int main(void) {
    VocabEntry arr[MAX_WORDS];
    int c = vocab_load(\"data/vocab.txt\", arr, MAX_WORDS);
    printf(\"Loaded %d words\\n\", c);
    vocab_list(arr, c);
    return 0;
}
