#include "inverted_search.h"

void search_word(const char *raw_word, HashTable *ht) {
    char word[256];
    strncpy(word, raw_word, 255); word[255] = '\0';
    to_lower(word);

    WordNode *wn = hash_search(ht, word);   /* O(1) average */
    if (wn) {
        printf("\n  Word '%s' is present in %d file/s:\n",
               raw_word, wn->file_count);
        for (FileNode *fn = wn->file_list; fn; fn = fn->next)
            printf("    In file: %-20s  %d time/s\n",
                   fn->filename, fn->count);
    } else {
        printf("\n  Word '%s' not found in the database.\n", raw_word);
    }
}