#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX_LINE 80
#define used 1
#define free 0
typedef struct block
{
    char name[3];
    int start;
    int length;
    int state;
} Block;
struct node
{
    Block block;
    struct node *next;
};
struct node *head;
int main(int argc, char *argv[])
{

    char *args[MAX_LINE / 2 + 1]; // command line (of 80) has max of 40 arguments
    char arg[MAX_LINE];
    int i;
    // i is the number of valid tokens in the command
    head = NULL;
    int init_length;
    init_length = atoi(argv[1]);
    /*initialize the whole memory */
    head = malloc(sizeof(struct node));
    head->block.start = 0;
    head->block.length = init_length;
    head->block.state = 0;
    head->next = NULL;

    while (1)
    {
        printf("allocator>");
        //fflush(stdout);
        fgets(arg, MAX_LINE, stdin);
        if (arg[0] == '\n')
            continue; //no commands
        if (arg[strlen(arg) - 1] == '\n')
            arg[strlen(arg) - 1] = '\0'; //fgets can take in the last '\n'
        if (strcmp("X", arg) == 0)
            break; // exit
        /* divide the command into tokens */
        i = 0;
        args[0] = strtok(arg, " ");
        while (args[i] != NULL)
        {
            i++;
            args[i] = strtok(NULL, " ");
        }
        /* carry out the command */
        if (strcmp("RQ", args[0]) == 0)
        {
            /* First fit */
            if (strcmp("F", args[3]) == 0)
            {
                struct node *move = head;
                while (move != NULL)
                {
                    if (move->block.state == 0 && move->block.length >= atoi(args[2]))
                    { //find the free and big-enough block
                        int tmp = move->block.length;
                        //update the node that move points to
                        move->block.state = 1;
                        strcpy(move->block.name, args[1]);
                        move->block.length = atoi(args[2]);
                        if (tmp != atoi(args[2]))
                        { // when this block is not fully used, should allocate one more node
                            struct node *newNode = malloc(sizeof(struct node));
                            newNode->block.start = move->block.start + atoi(args[2]);
                            newNode->block.length = tmp - atoi(args[2]);
                            newNode->block.state = 0;
                            newNode->next = move->next;
                            move->next = newNode;
                        }
                        break; // get the first fit, stop the search
                    }
                    move = move->next;
                }
                if (move != NULL)
                    printf("RQ successfully\n");
                else
                    printf("RQ failed\n");
            }
            /* Best fit */
            else if (strcmp("B", args[3]) == 0)
            {
                struct node *candidate = NULL;
                struct node *move = head;
                while (move != NULL)
                { // find the first candidate
                    if (move->block.state == 0 && move->block.length >= atoi(args[2]))
                    { //find the free and big-enough block
                        candidate = move;
                        break;
                    }
                    move = move->next;
                }
                if (move != NULL)
                { //find the best candidate
                    move = move->next;
                    while (move != NULL)
                    {
                        if (move->block.state == 0 && move->block.length >= atoi(args[2]))
                        {
                            if (move->block.length < candidate->block.length)
                                candidate = move;
                        }
                        move = move->next;
                    }
                    int tmp = candidate->block.length;
                    //update the node that move points to
                    candidate->block.state = 1;
                    strcpy(candidate->block.name, args[1]);
                    candidate->block.length = atoi(args[2]);
                    if (tmp != atoi(args[2]))
                    { // when this block is not fully used
                        struct node *newNode = malloc(sizeof(struct node));
                        newNode->block.start = candidate->block.start + atoi(args[2]);
                        newNode->block.length = tmp - atoi(args[2]);
                        newNode->block.state = 0;
                        newNode->next = candidate->next;
                        candidate->next = newNode;
                    }
                    printf("RQ successfully\n");
                }
                else
                    printf("RQ failed\n");
            }
            /* Worst fit */
            else if (strcmp("W", args[3]) == 0)
            {
                struct node *candidate = NULL;
                struct node *move = head;
                while (move != NULL)
                { // find the first candidate
                    if (move->block.state == 0 && move->block.length >= atoi(args[2]))
                    { //find the free and big-enough block
                        candidate = move;
                        break;
                    }
                    move = move->next;
                }
                if (move != NULL)
                { //find the worst candidate
                    move = move->next;
                    while (move != NULL)
                    {
                        if (move->block.state == 0 && move->block.length >= atoi(args[2]))
                        {
                            if (move->block.length > candidate->block.length)
                                candidate = move;
                        }
                        move = move->next;
                    }
                    int tmp = candidate->block.length;
                    //update the node that move points to
                    candidate->block.state = 1;
                    strcpy(candidate->block.name, args[1]);
                    candidate->block.length = atoi(args[2]);
                    if (tmp != atoi(args[2]))
                    { // when this block is not fully used
                        struct node *newNode = malloc(sizeof(struct node));
                        newNode->block.start = candidate->block.start + atoi(args[2]);
                        newNode->block.length = tmp - atoi(args[2]);
                        newNode->block.state = 0;
                        newNode->next = candidate->next;
                        candidate->next = newNode;
                    }
                    printf("RQ successfully\n");
                }
                else
                    printf("RQ failed\n");
            }
            continue;
        }
        if (strcmp("RL", args[0]) == 0)
        {
            struct node *move;
            move = head;
            while (move != NULL)
            {
                if (strcmp(move->block.name, args[1]) == 0 && move->block.state == 1)
                { // release this block
                    move->block.state = 0;
                    break;
                }
                move = move->next;
            }
            if (move != NULL)
                printf("RL successfully\n");
            else
                printf("RL failed\n");
            continue;
        }
        if (strcmp("C", args[0]) == 0)
        {
            struct node *move;
            struct node *prev;
            /* delete all the free holes */
            while (head != NULL && head->block.state == 0) // special case: when head is free
                head = head->next;
            if (head != NULL)
            { //if there is at least one used hole, head won't be NULL, otherwise head is NULL
                // interior or last element in the list
                move = head->next;
                prev = head;
                while (move != NULL)
                {
                    if (move->block.state == 0)
                    {
                        prev->next = move->next;
                        move = prev->next; //update move
                        continue;
                    }
                    prev = move;
                    move = move->next;
                }
                //update the used holes
                move = head;
                move->block.start = 0;
                prev = move;
                move = move->next;
                while (move != NULL)
                {
                    move->block.start = prev->block.start + prev->block.length;
                    prev = move;
                    move = move->next;
                }
                // add the last big free hole
                struct node *newNode = malloc(sizeof(struct node));
                newNode->block.start = prev->block.start + prev->block.length;
                newNode->block.length = init_length - newNode->block.start;
                newNode->block.state = 0;
                newNode->next = NULL;
                prev->next = newNode;
            }
            // when there is no used holes,i.e., head is NULL
            else
            {
                head = malloc(sizeof(struct node));
                head->block.start = 0;
                head->block.length = init_length;
                head->block.state = 0;
                head->next = NULL;
            }

            continue;
        }
        if (strcmp("STAT", args[0]) == 0)
        {
            struct node *move;
            move = head;
            while (move != NULL)
            {
                if (move->block.state == 1)
                    printf("Addresses [%d:%d] Process %s\n", move->block.start, move->block.length + move->block.start - 1, move->block.name);
                else
                    printf("Addresses [%d:%d] Unused\n", move->block.start, move->block.length + move->block.start - 1);
                move = move->next;
            }
            continue;
        }
    }

    return 0;
}














