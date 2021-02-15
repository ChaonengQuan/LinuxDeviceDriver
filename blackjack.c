/*
 * An BlackJack Solitare Implementation
 * This program will read a card from the dev/cards driver program each time
 * Then user will decide whiche one of the five piles to place the card
 * The game ends when there is no place to play the next card without exceeding 21 points.
 * @author Chaoneng Quan
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "blackjack.h"
#define ASCII_OFFSET 48 //useful for converting card label to ascii value

/* Input: number[0,52)
 * Output: ascii value of each card
 * E.g. 0->'A', 3->'3', 12->'K'
 */
char toAsciiValue(char driverResult);

/* Input: a pile struct
 * Output: 0 if no BlackJack, otherwise +21, +35 depends on the scoreing event
 */
int calculateScore(Pile **myPile);

int main()
{
    //open the dec/cards for later use
    FILE *cardsDriver = fopen("/dev/cards", "rb");
    if(cardsDriver == NULL){
	printf("Error: dev/cards is not loaded!\n");
	return (1);
    }

    //Game initialization (malloc all pointers)
    Table *table = tableConstructor();
    //require only once display the empty table
    printTable(table);
    //count how many cards have drawn
    int howManyCardsDrawn = 0;

    /*The Game Loop*/
    while (1)
    {
        //state1: draw card
        char valueFromDriver;
        if(fread(&valueFromDriver, sizeof(char), 1, cardsDriver) != 1) //write the value from driver to char [0,52)
	{
	    printf("Error: unable to peform read()!\n");
	    return (2);
        }
	char card = toAsciiValue(valueFromDriver);             //now card is in ascii value, except '10'
        if (card == 10)
            printf("Draw card: 10\n\n");
        else
            printf("Draw card: %c\n\n", card);
        //state2: user decide
        int currentScore = 0;

        int validPile = 0; //false by default, only exit this loop when user made valid choice
        while (validPile == 0)
        {
	    char c;
            printf("Which pile to place card in? ");
            int whichPile = 0;
            while (scanf("%d", &whichPile) == 0 || whichPile < 1 || whichPile > 5) //fail to read, or the number is not valid
            {
		while((c = getchar()) != '\n');
                printf("\nPlease input a valid pile number between 1 and 5: ");
            }
            printf("\n");

            //state3: place card and update coresponding pile
            switch (whichPile)
            {
            case 1:
                if (calculatePilePoints(&(table->pile1)) > 21)
                {
                    printf("This pile is busted, please choose another pile!\n\n");
                    continue;
                }
                validPile = 1;
                nodeInsert(&(table->pile1->head), card);
                currentScore = calculateScore(&(table->pile1)); //calculateScore will clear the pile if blackjack occurs
                break;
            case 2:
                if (calculatePilePoints(&(table->pile2)) > 21)
                {
                    printf("This pile is busted, please choose another pile!\n\n");
                    continue;
                }
                validPile = 1;
                nodeInsert(&(table->pile2->head), card);
                currentScore = calculateScore(&(table->pile2)); //and check for event1: blackjack and event2: natural blackjack and event3: five card charlie
                break;
            case 3:
                if (calculatePilePoints(&(table->pile3)) > 21)
                {
                    printf("This pile is busted, please choose another pile!\n\n");
                    continue;
                }
                validPile = 1;
                nodeInsert(&(table->pile3->head), card);
                currentScore = calculateScore(&(table->pile3));
                break;
            case 4:
                if (calculatePilePoints(&(table->pile4)) > 21)
                {
                    printf("This pile is busted, please choose another pile!\n\n");
                    continue;
                }
                validPile = 1;
                nodeInsert(&(table->pile4->head), card);
                currentScore = calculateScore(&(table->pile4));
                break;
            case 5:
                if (calculatePilePoints(&(table->pile5)) > 21)
                {
                    printf("This pile is busted, please choose another pile!\n\n");
                    continue;
                }
                validPile = 1;
                nodeInsert(&(table->pile5->head), card);
                currentScore = calculateScore(&(table->pile5));
                break;
            default:
                printf("Invalid pile!\n");
            }
        }
        /* Rules:
            Blackjack (pile total is 21)                                            +21 points
            Natural blackjack (pile total is 21 with 2 cards)                       +35 points
            Clearing the board (all piles are empty after the game has begun)       +45 points
            Cleared the deck (Dealt all 52 cards, repeatable)                       +60 points
            “Five Card Charlie” (a pile contains 5 cards that are less than 21)     +total points in pile
            When blackjack, natural blackjack, or a 5-card Charlie occurs, clear that pile.
        */
        //check event4: if all piles are empty
        if (table->pile1->head->faceValue == 0 && table->pile2->head->faceValue == 0 && table->pile3->head->faceValue == 0 && table->pile4->head->faceValue == 0 && table->pile5->head->faceValue == 0)
        {
            printf("Clearing the board! +45 points\n\n");
            currentScore += 45;
        }
        //check event5: if drawned 52 cards
        howManyCardsDrawn++;
        if (howManyCardsDrawn == 51)
        { //started at 0 so 51 means drawn 52 cards
            printf("Cleared the deck! +60 points\n\n");
            currentScore += 60;
        }

        //state4: update score
        table->score += currentScore;

        //state5: display table
        printTable(table);

        //game end condition
        int pile1Points = calculatePilePoints(&(table->pile1));
        int pile2Points = calculatePilePoints(&(table->pile2));
        int pile3Points = calculatePilePoints(&(table->pile3));
        int pile4Points = calculatePilePoints(&(table->pile4));
        int pile5Points = calculatePilePoints(&(table->pile5));
        if (pile1Points > 21 && pile2Points > 21 && pile3Points > 21 && pile3Points > 21 && pile4Points > 21 && pile5Points > 21)
        {
            printf("Game End! There is no place to play the next card without exceeding 21 points.\n\n");
            break; //exit game
        }
    }

    int tablescore = table->score;

    //Game finalization (free all pointers)
    fclose(cardsDriver);
    freeMyLinkedList(table->pile1->head);
    freeMyLinkedList(table->pile2->head);
    freeMyLinkedList(table->pile3->head);
    freeMyLinkedList(table->pile4->head);
    freeMyLinkedList(table->pile5->head);
    free(table->pile1);
    free(table->pile2);
    free(table->pile3);
    free(table->pile4);
    free(table->pile5);
    free(table);

    //high score tracking
    FILE *highscore;
    highscore = fopen("highscore.dat","r");
    if(highscore== NULL){	//file does not exist
	printf("Your first score is %d. Creating highscore.dat to save all-time highest score\n\n", tablescore);
	highscore = fopen("highscore.dat","w");
	fprintf(highscore, "%d", tablescore);
	fclose(highscore);
    }else{	//file exist
	int record;
	fscanf(highscore, "%d", &record);

	if(tablescore > record){
		printf("Congratulation! your new all-time highest score is %d\n\n", tablescore);
		rewind(highscore);
		highscore = fopen("highscore.dat", "w");
		fprintf(highscore, "%d", tablescore);
	}else{
		printf("Your final score is %d, the all-time highest score is %d\n\n", tablescore, record);
	}
	fclose(highscore);
    }

    return 0;
}

/*Start of myLinkedList functions */

struct Node *nodeConstructor(char receivedValue)
{
    struct Node *card = malloc(sizeof(struct Node));
    card->faceValue = receivedValue; //first char of the charArray(string)
    card->next = NULL;
    return card;
}

struct Node *nodeInsert(struct Node **head, char receivedValue)
{
    struct Node *walker = *head;
    if (walker->faceValue == 0) //first char of empty string
    {                           //case1: the list just initialized
        walker->faceValue = receivedValue;
    }
    else
    { //case2: at least 1 node in the list
        while (walker->next != NULL)
        {
            walker = walker->next;
        }
        walker->next = nodeConstructor(receivedValue);
    }
    return *head;
}

struct Node *nodeDelete(struct Node **head)
{
    struct Node *walker = *head;
    if (walker == NULL)
    { //case1: empty list
        printf("Error: cannot delete from NULL\n");
        return *head;
    }
    else if (walker->next == NULL)
    { //case2: only 1 node in the list
        free(walker);
        walker = NULL;
        *head = NULL; //change the original head to NULL
        return NULL;
    }
    else
    { //case3: more than 2 nodes in the list
        struct Node *prev = NULL;
        while (walker->next != NULL)
        {
            prev = walker;
            walker = walker->next;
        }
        free(walker); //free the last node
        prev->next = NULL;
        return *head;
    }
}

void freeMyLinkedList(struct Node *head)
{
    if (head == NULL)
    {
        return;
    }
    freeMyLinkedList(head->next);
    free(head);
}

int getMyLinkedListSize(struct Node *head)
{
    int size = 0;
    struct Node *walker = head;
    while (walker != NULL)
    {
        size++;
        walker = walker->next;
    }
    return size;
}

/*End of myLinkedList functions*/

/*Start of pile struct function*/

Pile *pileConstructor()
{
    Pile *myPile = malloc(sizeof(Pile));
    myPile->head = nodeConstructor(0);
    return myPile;
}

/* 
 * Sum the face value of all cards in the one pile
 */
int calculatePilePoints(Pile **myPile)
{
    struct Node *walker = (*myPile)->head;
    int points = 0;
    int numAces = 0;
    //step1: treat A as 11 points
    while (walker != NULL)
    {
        if (walker->faceValue == 'A')
        {
            points += 11;
            numAces++;
        }
        else if (walker->faceValue == 10 || walker->faceValue == 'J' || walker->faceValue == 'Q' || walker->faceValue == 'K')
        {
            points += 10;
        }
        else if (walker->faceValue > 49 && walker->faceValue < 58)
        { //card = from 2 to 9, Caution: ascii value
            points += (walker->faceValue) - ASCII_OFFSET;
        }
        else
        {
            points += 0;
        }
        walker = walker->next;
    }
    //step2: minus 10 for each A if bust
    if (points > 21)
    {
        int i;
        for (i = 0; i < numAces; i++)
        {
            if (points <= 21)
                break;
            points -= 10;
        }
    }
    return points;
}

void printPile(Pile *pile)
{
    //print all cards in the pile
    struct Node *walker = pile->head;
    while (walker != NULL)
    {
        if (walker->faceValue == 10) //special case: no ascii value for '10'
        {
            printf("10 ");
        }
        else
        {
            printf("%-3c", walker->faceValue);
        }
        walker = walker->next;
    }
    printf("        = ");
    printf("%d\n", calculatePilePoints(&pile));
}

/*End of pile struct functions*/

/*Start of table struct functions*/

Table *tableConstructor()
{
    Table *myTable = malloc(sizeof(Table));
    myTable->pile1 = pileConstructor();
    myTable->pile2 = pileConstructor();
    myTable->pile3 = pileConstructor();
    myTable->pile4 = pileConstructor();
    myTable->pile5 = pileConstructor();
    myTable->score = 0;
    return myTable;
}

/* Example:
    Pile (1): K 3              = 13
    Pile (2):                  = 0
    Pile (3): 6                = 6
    Pile (4): 9 2              = 11
    Pile (5): A                = 11
*/
void printTable(Table *table)
{
    //display total score
    printf("Score: %d\n\n", table->score);
    //display each pile
    printf("Pile (1): ");
    printPile(table->pile1);
    printf("Pile (2): ");
    printPile(table->pile2);
    printf("Pile (3): ");
    printPile(table->pile3);
    printf("Pile (4): ");
    printPile(table->pile4);
    printf("Pile (5): ");
    printPile(table->pile5);
    printf("\n");
}

/*End of table struct functions*/

/* Game fucntions */

/* return the ascii value of each card*/
// char means 1 byte
char toAsciiValue(char driverResult)
{
    driverResult = driverResult % 13;
    if (driverResult == 0)
    {
        return 'A'; //ascii value 65
    }
    else if (driverResult >= 1 && driverResult <= 8)
    {
        return driverResult + 1 + ASCII_OFFSET; // output is ascii in [50, 57]
    }
    else if (driverResult == 9)
    {
        return 10; //special case: no ascii value for '10'
    }
    else if (driverResult == 10)
    {
        return 'J';
    }
    else if (driverResult == 11)
    {
        return 'Q';
    }
    else
    { //driverResult == 12
        return 'K';
    }
}

/*
 * blackjack return 21
 * natural blackjack return 35
 * a 5-card Charlie return 	total points in pile
 *When blackjack, natural blackjack, or a 5-card Charlie occurs, clear that pile.
 */
int calculateScore(Pile **myPile)
{
    int points = calculatePilePoints(myPile);
    //check for black jacks
    if (points == 21)
    {
        if (getMyLinkedListSize((*myPile)->head) == 2)
        { //natural blackjack
            //clear pile
            printf("Natural Blackjack! +35 points\n\n");
            freeMyLinkedList((*myPile)->head);
            (*myPile)->head = nodeConstructor(0); //create a dummy head
            return 35;
        }
        printf("Blackjack! +21 points\n\n");
        //clear pile
        freeMyLinkedList((*myPile)->head);
        (*myPile)->head = nodeConstructor(0); //create a dummy head
        return points;                        //normal blackjack
    }

    if (points < 21 && getMyLinkedListSize((*myPile)->head) == 5)
    { //Five Card Charlie
        //clear pile
        printf("Five Card Charlie! +%d points\n\n", points);
        freeMyLinkedList((*myPile)->head);
        (*myPile)->head = nodeConstructor(0); //create a dummy head
        return points;
    }
    return 0;
}
