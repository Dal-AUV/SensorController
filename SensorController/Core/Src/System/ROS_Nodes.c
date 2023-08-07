/**
 * @file ROS_Nodes.c
 * @author Matthew Cockburn
 * @brief Source code for the ROS Node Linked list
 * @version 0.1
 * @date 2023-08-07
 * 
 * @copyright Copyright (c) 2023
 * 
 */
/* Header */
#include "FreeRTOS.h"
#include "System/ROS_Nodes.h"

/* Public Implementation */

struct Node* createNode(GenericPkt_t pkt){
    struct Node* newNode = (struct Node*)pvPortMalloc(sizeof(struct Node));
    if(newNode == NULL){
        return NULL;
    }
    newNode->pkt = pkt;
    newNode->next = NULL;
    return newNode;
}

void freeList(struct Node* head)
{
    struct Node* current = head;
    struct Node* next;

    while(current != NULL){
        next = current->next;
        vPortFree(current);
        current = next;
    }
    return;
}

void append_node(struct Node** head, GenericPkt_t pkt)
{
    // Create the new node
    struct Node* newNode = createNode(pkt);
    if(*head == NULL){
        *head = newNode;
        return;
    }
    struct Node* lastNode = *head;
    // Loop until the end
    while(lastNode->next != NULL){
        lastNode = lastNode->next;
    }
    lastNode->next = newNode;
    return;
}
void modifyData(struct Node* current, GenericPkt_t pkt,create)
