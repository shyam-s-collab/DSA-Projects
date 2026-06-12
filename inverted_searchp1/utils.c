#include "inverted_search.h"

/* Common stop-words skipped during indexing */
static const char *STOPWORDS[] = {
    "a","an","the","in","is","it","of","on","at","to",
    "for","and","or","but","not","with","from","by","as",
    "are","was","were","be","been","being","have","has",
    "had","do","does","did","will","would","could","should",
    "may","might","this","that","these","those","i","you",
    "he","she","we","they","me","him","her","us","them",
    "my","your","his","its","our","their",
    NULL
};

void to_lower(char *str) {
    for (; *str; str++)
        *str = (char)tolower((unsigned char)*str);
}

int is_stopword(const char *word) {
    for (int i = 0; STOPWORDS[i]; i++)
        if (strcmp(word, STOPWORDS[i]) == 0) return 1;
    return 0;
}

/*
 * hash_fn – djb2 hash function.
 * Fast, good distribution, widely used.
 */
unsigned int hash_fn(const char *word) {
    unsigned long hash = 5381;
    int c;
    while ((c = (unsigned char)*word++))
        hash = ((hash << 5) + hash) + c;   /* hash * 33 + c */
    return (unsigned int)(hash % HASH_SIZE);
}