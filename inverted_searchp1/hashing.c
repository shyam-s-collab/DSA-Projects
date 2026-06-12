/*
 * hashing.c  –  Hash table operations for the inverted index.
 *
 * Technique : Separate chaining (each bucket is a linked list of WordNodes).
 * Hash fn   : djb2  →  O(1) average insert / lookup.
 *
 * Structure diagram:
 *
 *  buckets[]
 *  [  0 ] → NULL
 *  [  7 ] → WordNode("flower") → WordNode("for") → NULL
 *  [ 42 ] → WordNode("grass")  → NULL
 *  ...
 *
 *  Each WordNode contains a FileNode list:
 *  WordNode("flower") → FileNode(file1.txt, 1) → FileNode(file2.txt, 2) → NULL
 */

#include "inverted_search.h"

/* ── Create ──────────────────────────────────────────────────── */
HashTable *hash_create(void) {
    HashTable *ht = (HashTable *)calloc(1, sizeof(HashTable));
    if (!ht) { perror("calloc"); exit(EXIT_FAILURE); }
    /* buckets are already NULL via calloc */
    ht->word_count = 0;
    return ht;
}

/* ── Internal: get/create FileNode inside a WordNode ─────────── */
static FileNode *get_or_create_fn(WordNode *wn, const char *filename) {
    for (FileNode *fn = wn->file_list; fn; fn = fn->next)
        if (strcmp(fn->filename, filename) == 0) return fn;

    FileNode *fn = (FileNode *)calloc(1, sizeof(FileNode));
    if (!fn) { perror("calloc"); exit(EXIT_FAILURE); }
    strncpy(fn->filename, filename, 255);
    fn->count = 0;
    fn->next  = wn->file_list;   /* prepend */
    wn->file_list = fn;
    wn->file_count++;
    return fn;
}

/* ── Search ──────────────────────────────────────────────────── */
WordNode *hash_search(HashTable *ht, const char *word) {
    unsigned int idx = hash_fn(word);
    for (WordNode *wn = ht->buckets[idx]; wn; wn = wn->next)
        if (strcmp(wn->word, word) == 0) return wn;
    return NULL;
}

/* ── Insert ──────────────────────────────────────────────────── */
WordNode *hash_insert(HashTable *ht, const char *word, const char *filename) {
    if (strlen(word) <= 1 || is_stopword(word)) return NULL;

    unsigned int idx = hash_fn(word);

    /* Check if word already exists in this bucket (collision chain) */
    WordNode *wn = ht->buckets[idx];
    while (wn) {
        if (strcmp(wn->word, word) == 0) {
            /* Word found → just increment file occurrence count */
            FileNode *fn = get_or_create_fn(wn, filename);
            fn->count++;
            return wn;
        }
        wn = wn->next;
    }

    /* New word → create WordNode and prepend to bucket chain */
    wn = (WordNode *)calloc(1, sizeof(WordNode));
    if (!wn) { perror("calloc"); exit(EXIT_FAILURE); }
    strncpy(wn->word, word, 255);
    wn->file_count = 0;
    wn->file_list  = NULL;
    wn->next       = ht->buckets[idx];  /* chain: new → old head */
    ht->buckets[idx] = wn;
    ht->word_count++;

    FileNode *fn = get_or_create_fn(wn, filename);
    fn->count++;
    return wn;
}

/* ── Display ─────────────────────────────────────────────────── */
/*
 * To display in sorted order we collect all WordNodes into a temp
 * array and sort them — the hash table itself is unordered by design.
 */
static int cmp_word(const void *a, const void *b) {
    return strcmp((*(WordNode **)a)->word, (*(WordNode **)b)->word);
}

void hash_display(HashTable *ht) {
    if (ht->word_count == 0) {
        printf("  (database is empty – create it first)\n");
        return;
    }

    /* Collect pointers */
    WordNode **arr = (WordNode **)malloc(ht->word_count * sizeof(WordNode *));
    if (!arr) { perror("malloc"); return; }
    int k = 0;
    for (int i = 0; i < HASH_SIZE; i++)
        for (WordNode *wn = ht->buckets[i]; wn; wn = wn->next)
            arr[k++] = wn;

    qsort(arr, k, sizeof(WordNode *), cmp_word);

    printf("\n  %-5s  %-20s  %s\n", "Index", "Word",
           "Files (name : occurrences)");
    printf("  %s\n",
           "--------------------------------------------------------------");
    for (int i = 0; i < k; i++) {
        WordNode *wn = arr[i];
        printf("  [%-3d]  [ %-18s]  %d file/s:", i, wn->word, wn->file_count);
        for (FileNode *fn = wn->file_list; fn; fn = fn->next)
            printf("  File: %s  %d", fn->filename, fn->count);
        printf("\n");
    }
    printf("  %s\n",
           "--------------------------------------------------------------");
    free(arr);
}

/* ── Free ────────────────────────────────────────────────────── */
void hash_free(HashTable *ht) {
    for (int i = 0; i < HASH_SIZE; i++) {
        WordNode *wn = ht->buckets[i];
        while (wn) {
            FileNode *fn = wn->file_list;
            while (fn) { FileNode *tmp = fn->next; free(fn); fn = tmp; }
            WordNode *tmp = wn->next; free(wn); wn = tmp;
        }
        ht->buckets[i] = NULL;
    }
    ht->word_count = 0;
    free(ht);
}

/* ── Save ────────────────────────────────────────────────────── */
/*
 * Format (same as before so existing database files still work):
 *   #:<bucket_index>
 *   <word>:<file_count>:<file1>:<cnt1>:...:#
 */
void hash_write(HashTable *ht, const char *dbfile) {
    FILE *fp = fopen(dbfile, "w");
    if (!fp) { perror("fopen"); return; }

    for (int i = 0; i < HASH_SIZE; i++) {
        for (WordNode *wn = ht->buckets[i]; wn; wn = wn->next) {
            fprintf(fp, "#:%d\n", i);
            fprintf(fp, "%s:%d:", wn->word, wn->file_count);
            for (FileNode *fn = wn->file_list; fn; fn = fn->next)
                fprintf(fp, "%s:%d:", fn->filename, fn->count);
            fprintf(fp, "#\n");
        }
    }
    fclose(fp);
    printf("  Database saved to '%s'.\n", dbfile);
}

/* ── Load ────────────────────────────────────────────────────── */
HashTable *hash_load(const char *dbfile) {
    FILE *fp = fopen(dbfile, "r");
    if (!fp) {
        printf("  [INFO] No saved database found at '%s'.\n", dbfile);
        return hash_create();
    }

    HashTable *ht = hash_create();
    char line[4096];

    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == '#') continue;          /* skip index lines */
        line[strcspn(line, "\n")] = '\0';

        char *tok = strtok(line, ":");
        if (!tok) continue;

        char word[256];
        strncpy(word, tok, 255); word[255] = '\0';

        /* Create the WordNode directly at the right bucket */
        unsigned int idx = hash_fn(word);
        WordNode *wn = (WordNode *)calloc(1, sizeof(WordNode));
        if (!wn) { perror("calloc"); exit(EXIT_FAILURE); }
        strncpy(wn->word, word, 255);
        wn->next = ht->buckets[idx];
        ht->buckets[idx] = wn;
        ht->word_count++;

        tok = strtok(NULL, ":");   /* skip stored file_count */

        while ((tok = strtok(NULL, ":")) && strcmp(tok, "#") != 0) {
            char fname[256];
            strncpy(fname, tok, 255); fname[255] = '\0';
            tok = strtok(NULL, ":");
            if (!tok || strcmp(tok, "#") == 0) break;
            int cnt = atoi(tok);

            FileNode *fn = (FileNode *)calloc(1, sizeof(FileNode));
            if (!fn) { perror("calloc"); exit(EXIT_FAILURE); }
            strncpy(fn->filename, fname, 255);
            fn->count = cnt;
            fn->next  = wn->file_list;
            wn->file_list = fn;
            wn->file_count++;
        }
    }
    fclose(fp);
    printf("  Database loaded from '%s'. (%d words)\n",
           dbfile, ht->word_count);
    return ht;
}