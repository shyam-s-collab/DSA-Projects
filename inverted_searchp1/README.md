# Inverted Search Engine — C Project

## Overview
An inverted index engine that maps words to the files containing them.
Uses a **sorted singly linked list** for the word index and nested
linked lists for per-word file records.

## Project Structure
```
inverted_search/
├── Makefile
├── README.md
├── data/
│   ├── file1.txt          ← sample input files
│   ├── file2.txt
│   └── file3.txt
└── src/
    ├── inverted_search.h  ← structs & prototypes
    ├── main.c             ← menu-driven driver
    ├── utils.c            ← to_lower, stopwords, free helpers
    ├── file_list.c        ← file-list LL (validate + insert)
    ├── indexing.c         ← parser + sorted word-LL builder
    └── searching.c        ← display, search, save, load
```

## Build
```bash
make          # compile → ./inverted_search
make clean    # remove objects + binary
make run      # compile and run
```

## Usage
```bash
./inverted_search              # uses database.txt
./inverted_search mydb.txt     # uses custom DB file
```

## Menu Options
| # | Action | Description |
|---|--------|-------------|
| 1 | Create DATABASE | Re-parse all loaded files and rebuild the index |
| 2 | Display Database | Print the full word index with file counts |
| 3 | Add Files | Validate and add a file; indexes it immediately |
| 4 | Search | Case-insensitive word lookup; shows file + count |
| 5 | Save Database | Persist current index to a text file |
| 6 | Exit | Auto-saves and quits |

## Data Structures
```c
/* One entry per unique word (sorted linked list) */
typedef struct WordNode {
    char             word[256];
    int              file_count;
    FileNode        *file_list;   // files containing this word
    struct WordNode *next;
} WordNode;

/* One entry per file per word */
typedef struct FileNode {
    char            filename[256];
    int             count;        // occurrences in this file
    struct FileNode *next;
} FileNode;
```

## Database File Format (plain text)
```
#:0
end:2:data/file2.txt:1:data/file3.txt:1:#
#:1
flower:3:data/file1.txt:1:data/file2.txt:1:data/file3.txt:1:#
...
```

## Notes
* Stop-words (a, the, in, is, …) are skipped during indexing.
* Words are normalised to lowercase before storing/searching.
* The word list is kept in ascending alphabetical order; binary-search
  style early-exit is applied during search (O(n) worst case on the LL).
* On exit the database is auto-saved to avoid data loss.