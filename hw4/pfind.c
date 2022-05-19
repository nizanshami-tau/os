#include <threads.h>
#include <stdio.h>
#include <stdlib.h>

// -----------------------------queue--------------------------------------

struct Node{
    char *path;
    struct node *next;
};
// start-the next item in the queue, end- last item in the line
struct Queue{
    struct Node *start, *end;
    int size;
};

struct Queue *init_queue(){
    struct Queue *q = malloc(sizeof(struct Queue));
    q->start = NULL;
    q->end = NULL;
    q->size = 0;
    return q;
}

struct Node *init_node(char *path){
    struct Node *node = malloc(sizeof(struct Node));
    node->path = path;
    node->next = NULL;
    return node;
}

void enqueue(struct Queue *q, char *path){
    struct Node *new = init_node(path);
    if(q->end == NULL){
        q->end = new;
        q->start = new;
        return;
    }
    q->end->next = new;
    q->end = new;
}

char *dequeue(struct Queue *q){
    if(q->start == NULL){
        return NUll;
    }
    

}


