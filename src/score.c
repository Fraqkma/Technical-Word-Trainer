#include <stdio.h>
#include <time.h>
#include "../include/score.h"

/* Save quiz result to data/progress.txt
   Format: username|timestamp|correct|total|time_seconds
*/
void score_save(const char *username, int correct, int total, double time_sec) {
    if (!username) return;

    FILE *f = fopen("data/progress.txt", "a");
    if (!f) return;

    time_t t = time(NULL);
    /* use long for timestamp and print time as integer seconds */
    fprintf(f, "%s|%ld|%d|%d|%.0f\n", username, (long)t, correct, total, time_sec);
    fclose(f);
}
