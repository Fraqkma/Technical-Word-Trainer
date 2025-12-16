#include <stdio.h>
#include <string.h>

/* Very simple: print progress file as-is */
void print_leaderboard(void) {
    FILE *f = fopen("data/progress.txt", "r");
    if (!f) {
        printf("No progress data.\n");
        return;
    }
    char line[512];
    printf("\n--- Leaderboard (raw) ---\n");
    while (fgets(line, sizeof(line), f)) {
        if (line[0] == '#') continue;
        /* format: user|timestamp|score|total|time */
        char user[64]; long ts; int score, total; double t;
        if (sscanf(line, " %63[^|]|%ld|%d|%d|%lf", user, &ts, &score, &total, &t) == 5) {
            printf("%s: %d/%d\n", user, score, total);
        }
    }
    fclose(f);
}
