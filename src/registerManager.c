#include "registerManager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int ALLOCATED_ARRAY[REGISTER_SIZE];

void allocate(AST_NODE *node, REGISTER_TYPE type, int registerNumber) {
    ALLOCATED_ARRAY[registerNumber] = 1;
    node->register_info.registerNumber = registerNumber;
    node->registerNumber = registerNumber;
    switch (type) {
    case R_32:
        node->register_info.symbol = 'w';
        break;
    case R_64:
        node->register_info.symbol = 'x';
        break;
    case S_32:
        node->register_info.symbol = 's';
        break;
    case D_64:
        node->register_info.symbol = 'd';
        break;
    }
}

void initRegister() {
    for (int i = 0; i < REGISTER_SIZE; i++)
        ALLOCATED_ARRAY[i] = 0;
}

void allocR0Register(AST_NODE *node, REGISTER_TYPE type) {
    for (int i = REGISTER0_BEGIN; i < REGISTER0_END; i++)
        if (ALLOCATED_ARRAY[i] == 0) {
            allocate(node, type, i);
            break;
        }
}

void allocR1Register(AST_NODE *node, REGISTER_TYPE type) {
    for (int i = REGISTER1_BEGIN; i < REGISTER1_END; i++)
        if (ALLOCATED_ARRAY[i] == 0) {
            allocate(node, type, i);
            break;
        }
}

void allocR2Register(AST_NODE *node, REGISTER_TYPE type) {
    for (int i = REGISTER2_BEGIN; i < REGISTER2_END; i++)
        if (ALLOCATED_ARRAY[i] == 0) {
            allocate(node, type, i);
            break;
        }
}

int allocR2() {
    for (int i = REGISTER2_BEGIN; i < REGISTER2_END; i++)
        if (ALLOCATED_ARRAY[i] == 0) {
            ALLOCATED_ARRAY[i] = 1;
            return i;
        }
}

void freeRegister(AST_NODE *node) {
    ALLOCATED_ARRAY[node->registerNumber] = 0;
    node->register_info.registerNumber = -1;
    node->register_info.symbol = '$';
}

int freeR2(int i) { ALLOCATED_ARRAY[i] = 0; }
