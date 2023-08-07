/**
 * @file ROS_Nodes.h
 * @author Matthew Cockburn
 * @brief Contains the ROS Node Linked List
 * @version 0.1
 * @date 2023-08-07
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef INC_SYSTEM_ROS_NODES_H_
#define INC_SYSTEM_ROS_NODES_H_

/* Headers */
#include "ROS.h"
#include "ROS_Dictionary.h"
/* Macros and Structures*/
/**
 * @brief Data Structure for a node on the linked list
 * 
 */
struct Node{
    GenericPkt_t pkt;
    struct Node* next;
};

/* Public Prototypes */
/**
 * @brief Create a Node object
 * @note This function uses malloc
 * @param item 
 * @return struct Node* 
 */
struct Node* createNode(GenericPkt_t item);
/**
 * @brief append a node to the end of a the linked list
 * 
 * @param head 
 * @param item 
 */
void append_node(struct Node** head, GenericPkt_t item);
/**
 * @brief pop the head of off the linked list
 * 
 * @param head 
 */
void pop_node(struct Node** head);
/**
 * @brief Free the entire list from memory
 * 
 * @param head 
 */
void freelist(struct Node* head);

#endif /* INC_SYSTEM_ROS_NODES_H_ */
