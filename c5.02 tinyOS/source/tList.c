#include "tlib.h"

#define firstNode headNode.nextNode
#define lastNode  headNode.preNode

void tNodeInit(tNode *node)
{
    node->nextNode = node;
    node->preNode = node;
}

void tListInit(tList * list)
{
    list->firstNode = &(list->headNode);
    list->lastNode  = &(list->headNode);
    list->nodeCount = 0;
}

uint32_t tListCount (tList *list)
{
    return list->nodeCount;
}

tNode *tListFirst (tList *list)
{
    tNode *node = (tNode *)0;

    if (list->nodeCount != 0) {
        node = list->firstNode;
    }
    return node;
}

tNode *tListLast (tList *list)
{
    tNode *node = (tNode *)0;

    if (list->nodeCount != 0) {
        node = list->lastNode;
    }
    return node;
}

tNode *tListPre (tList *list, tNode *node)
{
    if (node->preNode == node) {
        return (tNode *)0;
    } else {
        return node->preNode;
    }
}

tNode *tListNext (tList *list, tNode *node)
{
    if (node->nextNode == node) {
        return (tNode *)0;
    } else {
        return node->nextNode;
    }
}

void tListRemoveAll (tList *list)
{
    uint32_t count = list->nodeCount;
    tNode *nextNode = list->firstNode;
    for (count = list->nodeCount; count != 0; count--) {
        tNode *currentNode = nextNode;
        currentNode->preNode = currentNode;
        currentNode->nextNode = currentNode;
    }
    tListInit(list);
}

void tListAddFirst (tList *list, tNode *node)
{
    node->nextNode = list->firstNode;
    node->preNode  = list->firstNode->preNode;
    
    list->firstNode->preNode = node;
    list->firstNode = node;
    list->nodeCount++;
}

void tListAddLast (tList *list, tNode *node)
{
    node->nextNode = &(list->headNode);
    node->preNode  = list->lastNode;
    
    list->lastNode->nextNode = node;
    list->lastNode = node;
    list->nodeCount++;
}

tNode *tListRemoveFirst(tList *list)
{
    tNode *node = (tNode *)0;
    if (list->nodeCount != 0) {
        node = list->firstNode;
        node->nextNode->preNode = &(list->headNode);
        list->firstNode = node->nextNode;
        list->nodeCount--;
    }
    
    return node;
}

void tListInserAfter(tList *list, tNode *nodeAfter, tNode *nodeToInsert)
{
    nodeToInsert->preNode = nodeAfter;
    nodeToInsert->nextNode = nodeAfter->nextNode;
    
    nodeAfter->nextNode->preNode = nodeToInsert;
    nodeAfter->nextNode = nodeToInsert;
    
    list->nodeCount++;
}

void tListRemove(tList *list, tNode *node)
{
    node->preNode->nextNode = node->nextNode;
    node->nextNode->preNode = node->preNode;
    
    list->nodeCount--;
}
