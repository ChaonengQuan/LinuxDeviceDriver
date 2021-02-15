/* No typedef because Driver and Game have different implementations*/
struct Node
{
    char faceValue; /* store ascii value */
    struct Node *next;
};

/* malloc space and initialize fields by received value */
struct Node *nodeConstructor(char receivedValue);

/* insert to the last node */
struct Node *nodeInsert(struct Node **head, char receivedValue);

/* delete the last node */
struct Node *nodeDelete(struct Node **head);

/* Recursively free the list from tail to head*/
void freeMyLinkedList(struct Node *head);

/* Return the number of nodes in the linked list*/
int getMyLinkedListSize(struct Node *head);


/* A pile contans a myLinkedList of cards*/
typedef 
    struct _Pile
    {
        struct Node *head; /*head of myLinkedList*/
    } 
Pile;

/*constrcut a pile with NULL linked list*/
Pile *pileConstructor();

/*  calculate cards in the pile by rules then print out on console
    When blackjack, natural blackjack, or a 5-card Charlie occurs, clear that pile. 
    solution: An ace should always be counted as 11 unless it NEEDS to be 1 to prevent a bust */
int calculatePilePoints(Pile **myPile);

/* print out one pile of cards of the table on console*/
void printPile(Pile *pile);

/* A table contains five piles */
typedef 
    struct _Table
    {
        int score; /* display evenry round */
        //int cardsDealt; /* +1 each time read from dev/cards, reset to 0 when hit 52*/
        Pile *pile1; /* each pile with NULL myLinkedList */
        Pile *pile2;
        Pile *pile3;
        Pile *pile4;
        Pile *pile5;
    } 
Table;

/*construct a table with five pile, each pile with NULL linked list*/
Table *tableConstructor();

/* print out each pile of cards of the table on console */
void printTable(Table *table);