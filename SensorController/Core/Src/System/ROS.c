/**
 * @file ROS_Handler.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-06-10
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "System/ROS.h"
#include <assert.h>
#include <string.h>
#include "main.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "System/TaskCtrl.h"
#include "Peripherals/usart.h"

typedef enum Read_FSM {
    RFSM_waiting,
    RFSM_opcodes,
    RFSM_data,
    RFSM_csum,
    RFSM_reset,
    RFSM_error
} Read_FSM_t;

typedef enum Opcode_FSM{
    OFSM_start,
    OFSM_read,
    OFSM_end,
    OFSM_delimiter,
    OFSM_error
}Opcode_FSM_t;

typedef enum Data_FSM{
    DFSM_read,
    DFSM_start,
    DFSM_end,
    DFSM_delimiter,
    DFSM_error
}Data_FSM_t;

Read_FSM_t header_check(uint8_t * byte);
Opcode_FSM_t opcode_check(uint8_t * byte);

struct Node* createNode(GenericPkt_t item);
void append_node(struct Node** head, GenericPkt_t item);
void pop_node(struct Node** head);
void freelist(struct Node* head);

void ROS_Reader(void * arguments)
{
    Read_FSM_t state = RFSM_waiting;
    
    uint8_t csum = 0;
    uint8_t byte = 0;
    uint8_t num_opcodes = 0;
    uint8_t opcode_reads = 0;
    uint8_t opcode_flag = 0;

    char cur_opcode[MAX_OPCODE_LEN];
    char packet[MAX_PKT_LENGTH];

    while(1){

        if(xQueueReceive(ROSQueue,&byte,portMAX_DELAY)){
            state = RFSM_error;
        }

        switch(state)
        {
        RFSM_waiting:
            state = header_check(&byte); 
        break;

        RFSM_opcode:
            switch(opcode_check(&byte))
            {
            OFSM_read: /* Construct the opcode */
                if(opcode_flag){
                    state = RFSM_error;
                }else{
                    cur_opcode[opcode_reads++] = (char)(byte);
                    if(opcode_reads ==2){
                        opcode_flag = 1;
                    }
                }
            break;
            OFSM_delimiter: /* Delimiter found goto data state*/
                state = RFSM_data;
            break;
            OFSM_end: /*  */
                memset(cur_opcode,0,sizeof(char)*MAX_OPCODE_LEN);
                num_opcodes++;
                break;
            break;
            OFSM_start: /* clear opcode flag */
                opcode_flag = 0;
            break;
            default:
                state = RFSM_error;
            break;
            }
        break;
        
        RFSM_data:

        break;
        
        RFSM_csum:

        break;

        default: // Error's will go to FSM_ERROR statement
        
        break;
        }
        // FSM Loop clean up
        if(state == RFSM_error || state == RFSM_reset){
        
        }
    }   
}
/**
 * @brief 
 * 
 * @param byte 
 * @return Read_FSM_t 
 */
Read_FSM_t header_check(uint8_t * byte)
{
    assert(byte);

    if((char)(*byte) == HEADER){
        return RFSM_opcodes;
    } else {
        return RFSM_waiting;
    }
    return RFSM_error;
}
/**
 * @brief 
 * 
 * @param byte 
 * @return Opcode_FSM_t 
 */
Opcode_FSM_t opcode_check(uint8_t * byte)
{
    assert(byte);
    char chr = (char)*byte;
    if(chr == OPCODE_START){
        return OFSM_start;
    }else if(chr == OPCODE_END){
        return OFSM_end;
    }else if (chr == DELIMITER){
        return OFSM_delimiter;
    }else{
        return OFSM_read;
    }
}
/**
 * @brief Create a Node object
 * 
 * @param pkt 
 * @return struct Node* 
 */
struct Node* createNode(GenericPkt_t pkt){
    struct Node* newNode = (struct Node*)pvPortMalloc(sizeof(struct Node));
    if(newNode == NULL){
        return NULL;
    }
    newNode->pkt = pkt;
    newNode->next = NULL;
    return newNode;
}

/**
 * @brief 
 * 
 * @param head 
 */
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
/**
 * @brief 
 * 
 * @param head 
 * @param pkt 
 */
void append_node(struct Node** head, GenericPkt_t pkt)
{
    struct Node* newNode = createNode(pkt);
    if(*head == NULL){
        *head = newNode;
        return;
    }

    struct Node* lastNode = *head;
    while(lastNode->next != next)
}
