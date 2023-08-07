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
#include "System/ROS_Nodes.h"
#include "System/ROS.h"
#include "System/ROS_Dictionary.h"
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
Read_FSM_t data_FSM(Data_FSM_t * state, uint8_t * byte,struct Node* head, 
    uint8_t * read_cnt, uint8_t * buffer);
int opcode_add_to_list(struct Node* head, char* opcode, uint16_t size);

ePKT_ID_t GetOpcodeId(char* code, uint16_t size){
    for(int i = 0; i < NUM_OF_OPCODES; i++){
        if(strncmp(code,opcode_dictionary[i],OPCODE_SIZE)){
            return i;
        }
    }
    return BAD_PKT;
}

void ROS_Reader(void * arguments)
{
    // struct NodeList {
    //     struct Node* head;
    //     struct Node* tail;
    // }list;
    
    struct Node* list_head = NULL;

    Read_FSM_t state = RFSM_waiting;
    Data_FSM_t data_state = DFSM_start;
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
        csum ^= byte;
        switch(state)
        {
        RFSM_waiting:
            switch(header_check(&byte))
            {
                RFSM_opcodes:
                    state = RFSM_opcodes;
                    break;
                RFSM_error:
                    state = RFSM_error;
                    break;
                default:
                    break;
            } 
        break;

        RFSM_opcode:
            switch(opcode_check(&byte))
            {
            OFSM_read: /* Construct the opcode */
                if(opcode_flag){
                    state = RFSM_error;
                }else{
                    cur_opcode[opcode_reads++] = (char)(byte);
                    if(opcode_reads == 2){
                        opcode_flag = 1;
                    }
                }
            break;
            OFSM_delimiter: /* Delimiter found goto data state*/
                state = RFSM_data;
                break;
            OFSM_end: /* New OPCODE Found, add it to the list */
                if(opcode_add_to_list(list_head, cur_opcode, OPCODE_SIZE)){
                    state = RFSM_error;
                }else{
                    num_opcodes++;
                }
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
            freelist(list_head);
        }
    }   
}
Read_FSM_t data_FSM(Data_FSM_t * state, uint8_t * byte,struct Node* head, 
    uint8_t * read_cnt, uint8_t * buffer, uint16_t size, uint8_t item_num);
{
    assert(state);
    assert(byte);
    assert(head);
    assert(read_cnt);
    assert(buffer);
    
    DFSM_end,
    DFSM_delimiter,
    DFSM_error

    switch (*state)
    {
    case DFSM_read:
        *buffer[read_cnt++] = *byte;
        break;
    DFSM_end:

        break;
    default:
        break;
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
 * @brief 
 * 
 * @param tail 
 * @param code 
 * @param size 
 * @return int 
 */
int opcode_add_to_list(struct Node* head, char* opcode, uint16_t size){
    assert(head);
    assert(opcode);

    GenericPkt_t pkt; 
    pkt.id = GetOpcodeId(opcode,size);
    if(pkt.id == BAD_PKT) {
        return -1;
    }else{
        memset(opcode,"00",size);
        append_node(head,pkt);
        return 0;
    }
    return -1;
}