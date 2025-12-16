/* include/ai.h */
#ifndef AI_H
#define AI_H

/* Analyze user's review history and print recommendations to stdout.
   username: currently-logged-in user to analyze.
   vocab_path: path to vocab file (e.g., "data/vocab.txt")
   review_path: path to review file (e.g., "data/review.txt")
*/
void ai_recommend_for_user(const char *username, const char *vocab_path, const char *review_path);

#endif // AI_H
