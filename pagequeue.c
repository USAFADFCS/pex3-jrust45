/** pagequeue.c
 * ===========================================================
 * Name: C1C John Rust III, 2026
 * Section: CS483 / M4
 * Project: PEX3 - Page Replacement Simulator
 * Purpose: Implementation of the PageQueue ADT — a doubly-linked
 *          list for LRU page replacement.
 *          Head = LRU (eviction end), Tail = MRU end.
 * =========================================================== */
#include <stdio.h>
#include <stdlib.h>

#include "pagequeue.h"

/**
 * @brief Create and initialize a page queue with a given capacity
 */
PageQueue *pqInit(unsigned int maxSize) {
    // TODO: malloc a PageQueue, set head and tail to NULL,
    //       size to 0, maxSize to maxSize, and return the pointer
    PageQueue* queue = (PageQueue*)malloc(sizeof(PageQueue));
    queue->head = NULL;
    queue->tail = NULL;
    queue->size = 0;
    queue->maxSize = maxSize;
    return queue;
}

/**
 * @brief Access a page in the queue (simulates a memory reference)
 */
long pqAccess(PageQueue *pq, unsigned long pageNum) {
    // TODO: Search the queue for pageNum (suggest searching tail->head
    //       so you naturally count depth from the MRU end).
    //

    // PageQueue has >= 1 page in it
    PqNode* currentPageNode = pq->tail;
    for(int i = 0; i < pq->size; i++) {
        
        // HIT PATH (page found at depth d)
        //   - Remove the node from its current position and re-insert
        //     it at the tail (most recently used).
        //   - Return d.
        if (currentPageNode->pageNum == pageNum) {
                        
            // Page is at the tail of PageQueue (MRU end or 0)
            if (i == 0) {
                return i;
            }
            // Page is at the head of PageQueue (evict end)
            else if (i == pq->size - 1) {
                currentPageNode->next->prev = NULL;
                pq->head = currentPageNode->next;
                currentPageNode->prev = pq->tail;
                pq->tail->next = currentPageNode;
                pq->tail = currentPageNode;
                currentPageNode->next = NULL;
                return i;         
            }
            // Page is in the middle of PageQueue (middle)
            else {
                currentPageNode->prev->next = currentPageNode->next;
                currentPageNode->next->prev = currentPageNode->prev;
                pq->tail->next = currentPageNode;
                currentPageNode->prev = pq->tail;
                pq->tail = currentPageNode;
                currentPageNode->next = NULL;
                return i;
            }
        }
        else {
            currentPageNode = currentPageNode->prev;
        }
    }   
    // MISS PATH (page not found)
    //   - Allocate a new node for pageNum and insert it at the tail.
    //   - If size now exceeds maxSize, evict the head node (free it).
    //   - Return -1.
    PqNode* newPage = (PqNode*)malloc(sizeof(PqNode));
    newPage->pageNum = pageNum;
    pq->size++;
    newPage->next = NULL;

    // PageQueue is Empty Case
    if (pq->head == NULL) {
        pq->head = newPage;
        pq->tail = newPage;
        newPage->prev = NULL;
    }
    // PageQueue is Not Empty Case
    else {
        newPage->prev = pq->tail;
        pq->tail->next = newPage;
        pq->tail = newPage;
    }
    
    // If the sizeMax is ever reached
    if (pq->size > pq->maxSize) {
        PqNode* temp = pq->head;
        pq->head = temp->next;
        free(temp);
        pq->head->prev = NULL;
        pq->size--;
    }

    return -1;
}

/**
 * @brief Free all nodes in the queue and reset it to empty
 */
void pqFree(PageQueue *pq) {
    // TODO: Walk from head to tail, free each node, then free
    //       the PageQueue struct itself.
    if (pq->head == NULL) {
        free(pq);
    }
    else {
        while (pq->size != 0) {
            // Last page in list
            if (pq->size == 1) {
                free(pq->head);
                free(pq);
                return;
            }
            // First element of List on MRU End
            else {
                PqNode* selection = pq->tail;
                pq->tail = pq->tail->prev;
                free(selection);
                pq->tail->next = NULL;
                pq->size -= 1;
            }

        }
    }
    return;
}

/**
 * @brief Print queue contents to stderr for debugging
 */
void pqPrint(PageQueue *pq) {
    // TODO (optional): Print each page number from head to tail,
    //                  marking which is head and which is tail.
    //                  Useful for desk-checking small traces.
    fprintf(stderr, "Queue (size=%u/%u) HEAD[LRU]->", pq->size, pq->maxSize);
    PqNode *current = pq->head;
    while (current != NULL) {
        if (current == pq->tail) {
            fprintf(stderr, "[%lu]<-TAIL[MRU]", current->pageNum);
        }
        else {
            fprintf(stderr, "%lu->", current->pageNum);
        }
        current = current->next;
    }
    fprintf(stderr, "\n");
}
