#include "header.h"
#include "symbolTable.h"
#include "armGenerator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int DEBUG = 1;

// ARM routine
void storeLinkerAndSPRegister(int size) {
    printf("\tstp x29, x30, [sp, -%d]!\n", size);
    printf("\tadd x29, sp, 0\n");
}

void loadLinkerAndSPRegister() {
    printf("\tldp x29, x30, [sp], 16\n");
    printf("\tret\n");
}

int getOffset(char* symbolName){
    SymbolTableEntry *entry = retrieveSymbol(symbolName);

    if(DEBUG > 0){
        if(entry == NULL){
            fprintf(stderr, "entry  is null %s\n", symbolName);
            exit(1);
            return 0;
        }
        else{
            fprintf(stderr, "Get id: %s offset: %d\n", symbolName, entry->attribute->offset);
            return entry->attribute->offset;
        }
    }
    return entry->attribute->offset;
}

void armGenerator(AST_NODE *root) {
    visitProgramNode(root);
    return;
}

void visitProgramNode(AST_NODE *programNode) {
    AST_NODE *traverseDeclaration = programNode->child;
    while (traverseDeclaration) {
        if (traverseDeclaration->nodeType == VARIABLE_DECL_LIST_NODE) {
            visitGeneralNode(traverseDeclaration);
        } 
        else if(traverseDeclaration->nodeType == DECLARATION_NODE) {
            visitDeclarationNode(traverseDeclaration);
        }
        else{
            fprintf(stderr, "Node not match in visitProgramNode\n");
            exit(1);
        }

        if (traverseDeclaration->dataType == ERROR_TYPE) {
            programNode->dataType = ERROR_TYPE;
        }

        traverseDeclaration = traverseDeclaration->rightSibling;
    }
    return;
}

void visitDeclareIdList(AST_NODE *declarationNode,
                   SymbolAttributeKind isVariableOrTypeAttribute,
                   int ignoreArrayFirstDimSize) {
    AST_NODE *typeNode = declarationNode->child;
    AST_NODE *traverseIDList = typeNode->rightSibling;
    while (traverseIDList) {
        if(DEBUG > 0){
            int offset = traverseIDList->semantic_value.identifierSemanticValue.offset;
            char* name = traverseIDList->semantic_value.identifierSemanticValue.identifierName;
            getOffset(name);
        }
        switch (
            traverseIDList->semantic_value.identifierSemanticValue.kind) {
        case NORMAL_ID:
            break;
        case ARRAY_ID:
            break;
        case WITH_INIT_ID:
            // TODO assign value to sp + offset 
            break;
        default:
            break;
        }
        traverseIDList = traverseIDList->rightSibling;
    }
}

void visitDeclarationNode(AST_NODE *declarationNode) {
    AST_NODE *typeNode = declarationNode->child;

    switch (declarationNode->semantic_value.declSemanticValue.kind) {
        case VARIABLE_DECL:
            visitDeclareIdList(declarationNode, VARIABLE_ATTRIBUTE, 0);
            break;
        /*
        case TYPE_DECL:
            declareIdList(declarationNode, TYPE_ATTRIBUTE, 0);
            break;
        */
        case FUNCTION_DECL:
            visitDeclareFunction(declarationNode);
            break;
        /*
        case FUNCTION_PARAMETER_DECL:
            declareIdList(declarationNode, VARIABLE_ATTRIBUTE, 1);
            break;
        */
    }
    return;
}

void visitDeclareFunction(AST_NODE *declarationNode) {
    AST_NODE *returnTypeNode = declarationNode->child;
    AST_NODE *functionNameID = returnTypeNode->rightSibling;
    AST_NODE *parameterListNode = functionNameID->rightSibling;

    printf(".text\n");
    printf(
        "_start_%s:\n",
        functionNameID->semantic_value.identifierSemanticValue.identifierName);
    storeLinkerAndSPRegister(declarationNode->semantic_value.declSemanticValue.frameSize);

    /*TODO bonus
     * parse parameter
    AST_NODE *traverseParameter = parameterListNode->child;
    if (traverseParameter) {
        processDeclarationNode(traverseParameter);
    }
    while (traverseParameter) {
        processDeclarationNode(traverseParameter);
    }
    */

    AST_NODE *blockNode = parameterListNode->rightSibling;
    AST_NODE *traverseListNode = blockNode->child;
    while (traverseListNode) {
        visitGeneralNode(traverseListNode);
        traverseListNode = traverseListNode->rightSibling;
    }

    loadLinkerAndSPRegister();
}

void visitGeneralNode(AST_NODE *node) {
    AST_NODE *traverseChildren = node->child;
    switch (node->nodeType) {
    case VARIABLE_DECL_LIST_NODE:
        while (traverseChildren) {
            visitDeclarationNode(traverseChildren);
            traverseChildren = traverseChildren->rightSibling;
        }
        break;
    case STMT_LIST_NODE:
        while (traverseChildren) {
            visitStmtNode(traverseChildren);
            traverseChildren = traverseChildren->rightSibling;
        }
        break;
    /*
    case NONEMPTY_ASSIGN_EXPR_LIST_NODE:
        while (traverseChildren) {
            checkAssignOrExpr(traverseChildren);
            if (traverseChildren->dataType == ERROR_TYPE) {
                node->dataType = ERROR_TYPE;
            }
            traverseChildren = traverseChildren->rightSibling;
        }
        break;
    case NONEMPTY_RELOP_EXPR_LIST_NODE:
        while (traverseChildren) {
            processExprRelatedNode(traverseChildren);
            if (traverseChildren->dataType == ERROR_TYPE) {
                node->dataType = ERROR_TYPE;
            }
            traverseChildren = traverseChildren->rightSibling;
        }
        break;
    */
    case NUL_NODE:
        break;
    default:
        break;
    }
}

void visitStmtNode(AST_NODE *stmtNode) {
    if (stmtNode->nodeType == NUL_NODE) {
        return;
    } else if (stmtNode->nodeType == BLOCK_NODE) {
        // processBlockNode(stmtNode);
    } else {
        switch (stmtNode->semantic_value.stmtSemanticValue.kind) {
        /*case WHILE_STMT:
            checkWhileStmt(stmtNode);
            break;
        case FOR_STMT:
            checkForStmt(stmtNode);
            break;
        case ASSIGN_STMT:
            checkAssignmentStmt(stmtNode);
            break;
        case IF_STMT:
            checkIfStmt(stmtNode);
            break;
            */
        case FUNCTION_CALL_STMT:
            visitFunctionCall(stmtNode);
            break;
        /*case RETURN_STMT:
            checkReturnStmt(stmtNode);
            break;
            */
        default:
            printf(
                "Unhandle case in void processStmtNode(AST_NODE* stmtNode)\n");
            stmtNode->dataType = ERROR_TYPE;
            break;
        }
    }
}

void visitFunctionCall(AST_NODE *functionCallNode) {
    AST_NODE *functionIDNode = functionCallNode->child;

    // special case
    if (strcmp(functionIDNode->semantic_value.identifierSemanticValue
                   .identifierName,
               "write") == 0) {
        visitWriteFunction(functionCallNode);
        return;
    }
    /*
        SymbolTableEntry *symbolTableEntry = retrieveSymbol(
            functionIDNode->semantic_value.identifierSemanticValue.identifierName);
        functionIDNode->semantic_value.identifierSemanticValue.symbolTableEntry
       =
            symbolTableEntry;

        if (symbolTableEntry == NULL) {
            printErrorMsg(functionIDNode, SYMBOL_UNDECLARED);
            functionIDNode->dataType = ERROR_TYPE;
            functionCallNode->dataType = ERROR_TYPE;
            return;
        } else if (symbolTableEntry->attribute->attributeKind !=
                   FUNCTION_SIGNATURE) {
            printErrorMsg(functionIDNode, NOT_FUNCTION_NAME);
            functionIDNode->dataType = ERROR_TYPE;
            functionCallNode->dataType = ERROR_TYPE;
            return;
        }

        AST_NODE *actualParameterList = functionIDNode->rightSibling;
        processGeneralNode(actualParameterList);

        AST_NODE *actualParameter = actualParameterList->child;
        Parameter *formalParameter =
            symbolTableEntry->attribute->attr.functionSignature->parameterList;

        int parameterPassingError = 0;
        while (actualParameter && formalParameter) {
            if (actualParameter->dataType == ERROR_TYPE) {
                parameterPassingError = 1;
            } else {
                checkParameterPassing(formalParameter, actualParameter);
                if (actualParameter->dataType == ERROR_TYPE) {
                    parameterPassingError = 1;
                }
            }
            actualParameter = actualParameter->rightSibling;
            formalParameter = formalParameter->next;
        }

        if (parameterPassingError) {
            functionCallNode->dataType = ERROR_TYPE;
        }
        if (actualParameter != NULL) {
            printErrorMsg(functionIDNode, TOO_MANY_ARGUMENTS);
            functionCallNode->dataType = ERROR_TYPE;
        } else if (formalParameter != NULL) {
            printErrorMsg(functionIDNode, TOO_FEW_ARGUMENTS);
            functionCallNode->dataType = ERROR_TYPE;
        } else {
            functionCallNode->dataType =
                symbolTableEntry->attribute->attr.functionSignature->returnType;
        }
        */
}

void visitWriteFunction(AST_NODE *functionCallNode) {
    AST_NODE *functionIDNode = functionCallNode->child;
    AST_NODE *actualParameterList = functionIDNode->rightSibling;
    visitGeneralNode(actualParameterList);
    AST_NODE *actualParameter = actualParameterList->child;

    switch (actualParameter->dataType) {
    case INT_TYPE:
        printf("\tmov x0, %d\n",
               actualParameter->semantic_value.const1->const_u);
        printf("\tbl _write_int\n");
        break;
    case FLOAT_TYPE:
        printf("\tmov x0, %lf\n",
               actualParameter->semantic_value.const1->const_u);
        printf("\tbl _write_float");
        break;
    case CONST_STRING_TYPE:
        printf("\tmov x0, %s\n",
               actualParameter->semantic_value.const1->const_u);
        printf("\tbl _write_str");
        break;
    default:
        break;
    }
}