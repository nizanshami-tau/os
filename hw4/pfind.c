#include <threads.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>

mtx_t lock;
cnd_t cv;

// -----------------------------queue--------------------------------------

struct Node{
    char *path;
    struct Node *next;
};
// start-the next item in the queue, end- last item in the line
struct Queue{
    struct Node *start, *end;
    int size;
    char *search_term;
};

struct Queue *init_queue(char *search_term){
    struct Queue *q = malloc(sizeof(struct Queue));
    q->start = NULL;
    q->end = NULL;
    q->size = 0;
    q->search_term = search_term;
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
    q->size++;
}

char *dequeue(struct Queue *q){
    struct Node *tmp;
    char *result_path;
    if(q->size == 0){
        return NULL;
    }
    tmp = q->start;
    q->start = q->start->next;
    if(q->size == 1){
        q->end = NULL;
    }
    q->size--;
    result_path = tmp->path;
    free(tmp);
    return result_path;
}

void free_queue(struct Queue *q){
    while (q->size != 0){
        dequeue(q);
    }
    free(q);
}
//--------------------------------------------------------------------------------
int search_file(void *q){
    
}

int init_thrd(){
    return 0;
}
//--------------------------------------------------------------------------------

int main(int argc, char *argv[]){
    struct Queue *q;
    char *root, *search_term;
    int n, rc, i, t;

    if(argc != 4){
        perror("not enough arguments");
        exit(1);
    }
    if(opendir(argv[1]) != NULL){
        perror("root file can't be search");
        exit(1);
    }
    root = argv[1];
    search_term = argv[2];
    n = atoi(argv[3]);
    
    q = init_queue(search_term);
    enqueue(q, argv[1]);

    rc = mtx_init(&lock, mtx_plain);
    if (rc != thrd_success) {
    perror("ERROR in mtx_init()");
    exit(1);
    }
    rc = cnd_init(&cv);
    if(rc != thrd_success){
        perror("ERROR in cnd_init()");
        exit(1);
    }

    thrd_t threads[n];
    
    for(i = 0; i < n;i++){
        rc = thrd_create(&threads[i], init_thrd, NULL);
        if(rc != thrd_success){
            perror("ERROR in thread create");
            exit(1);
        }
    }

    //wait all the thread are created
    for(i = 0;i < n;i++){
        rc = thrd_join(threads[i], &t);
        if(rc != thrd_success){
            perror("ERROR in thrd_join");
            exit(1);
        }
    }
    

    return 0;
}