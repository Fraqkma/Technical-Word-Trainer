#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/quiz.h"
#include "../include/store.h"
#include "../include/score.h"

// shuffle array (Fisher–Yates) (Shuffle all each multiple choices in each questions)
static void shuffle(int arr[], int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1); //use random method combine with swapping fucntion
        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}

// pick 4 choices, one is correct (use the implemented shuffle meth)
static void generate_choices(int correct_index, int count, int choices[4]) {
    int used[4];
    int c = 0;

    choices[c++] = correct_index;  // correct answer first

    while (c < 4) {
        int rand_index = rand() % count; //use rand to choose random vocab index
        int duplicate = 0;

        for (int i = 0; i < c; i++) { //check duplicates
            if (choices[i] == rand_index) {
                duplicate = 1;
                break;
            }
        }

        if (!duplicate) //if different add to array
            choices[c++] = rand_index;
    }

    shuffle(choices, 4); //then shuffle each choices
}

int quiz_run(VocabEntry list[], int count, const char *username, int num_q) {
    if (count == 0) {
        printf("No vocabulary available.\n"); //prevent crashing if vocab list = 0
        return 0;
    }

    if (num_q > count) //prevent exceeding question
        num_q = count;

    srand(time(NULL)); //Make the sequence of questions truly random 
    //by using seed the use the input of time of the world since 1970
    int correct = 0;

    for (int q = 0; q < num_q; q++) { //loop for selected question
    
        // pick a random vocab index for this question
        int correct_index = rand() % count;

        // generate 4 choices
        int choices[4];
        generate_choices(correct_index, count, choices); //use generate method

        printf("\nQ%d: What is the meaning of '%s'?\n", //show question
               q + 1, list[correct_index].word);

        // print choices
        for (int i = 0; i < 4; i++) {
            printf("%d) %s\n", i + 1, list[choices[i]].def); 
        }

        // get answer 
        int ans;
        printf("Choose (1-4): ");
        scanf("%d", &ans);

        // clear leftover newline
        getchar();

        if (ans >= 1 && ans <= 4 && choices[ans - 1] == correct_index) { //if answered correctly
            printf("Correct!\n");
            correct++;
        } else {
            printf("Wrong! Correct answer: %s\n",
                   list[correct_index].def); //if incorrect show real answer

            review_record(username, list[correct_index].word); //use review method to add those words to the review session
        }
    }

    printf("\nFinal score: %d/%d\n", correct, num_q); //show final score
    score_save(username, correct, num_q, 0); //save score for this user

    return correct; //return correct points
}
