#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "evalexp.h"

/**
 * Store variableList and their values
 * Variable names can be a max of 7 characters
 */
struct variableList {
    char name[8];
    int value;
    struct variableList *variables;
};


/**
 * Add a variable to the variable list
 */
struct variableList *addVariable(struct variableList *variables, char *name, int value) {
    struct variableList *temp = variables;
    while (temp) {
        if (strcmp(temp->name, name) == 0) {
            temp->value = value;
            return variables;
        }
        temp = temp->variables;
    }

    struct variableList *newFront = malloc(sizeof(struct variableList));
    strcpy(newFront->name, name);
    newFront->value = value;
    newFront->variables = variables;
    return newFront;
}

/**
 * Delete a variable from the variable list
 */
struct variableList *deleteVariable(struct variableList *variables, char *variable) {
    struct variableList *temp = variables;

    if (temp) {
        if (strcmp(temp->name, variable) == 0) {
            return temp->variables;
        }
    }
    return variables;
}

/**
 * Is the variable in the list of variables?
 */
int inVariables(struct variableList *variables, char *variable) {
    if (variables) {
        return strcmp(variables->name, variable) == 0;
    }
    return 0;
}

/**
 * Return a variable's value
 */
int getVariableValue(char *name, struct variableList *variables) {
    while (variables) {
        if (strcmp(variables->name, name) == 0) {
            return variables->value;
        }
        variables = variables->variables;
    }
    return 0;
}

/**
 * Delete all variables
 */
void deleteVariables(struct variableList *variables) {
    while (variables) {
        struct variableList *temp = variables;
        variables = variables->variables;
        free(temp);
    }
}

// Duplicate method to stop the compiler complaining
int evalExpVars(struct exp *e, struct variableList *variables);

/**
 * Evaluate an expression list
 */
int evalExpList(struct explist *expressions, enum op op, struct variableList *variables) {
    int sum;
    switch (op) {
        case isplus :
            sum = 0;
            // Sum all expressions
            while (expressions) {
                // Add evaluated variable to the sum
                sum += evalExpVars(expressions->head, variables);
                expressions = expressions->tail;
            }
            return sum;
        case ismult :
            // Set the sum to 1 (not 0 because that would result in 0 every time)
            sum = 1;
            // Since the sum is set to 1, we need to have this in order to not return 1 when expressions is null
            if (!expressions) {
                return 0;
            }
            // Multiply all expressions
            while (expressions) {
                // Multiply the sum by the evaluated variable
                sum *= evalExpVars(expressions->head, variables);
                expressions = expressions->tail;
            }
            return sum;
    }
    return 0;
}

/**
 * Replace variables with their values
 */
int evalExpVars(struct exp *e, struct variableList *variables) {
    switch (e->tag) {
        case isconstant :
            return e->constant;
        case isvar :
            return getVariableValue(e->var, variables);
        case isopapp :
            return evalExpList(e->exps, e->op, variables);
        case islet :
            if (inVariables(variables, e->bvar)) {
                int oldVal = getVariableValue(e->bvar, variables);
                int evaluated = evalExpVars(e->bexp, variables);
                int toReturn = evalExpVars(e->body, addVariable(variables, e->bvar, evaluated));
                addVariable(variables, e->bvar, oldVal);
                return toReturn;
            } else {
                int evaluated = evalExpVars(e->bexp, variables);
                int toReturn = evalExpVars(e->body, addVariable(variables, e->bvar, evaluated));
                deleteVariable(variables, e->bvar);
                return toReturn;
            }
    }
    return 0;
}


/**
 * Evaluate an expression
 */
int evalexp(struct exp *e) {
    // Evaluate and calculate from the given expression
    struct variableList *vars = NULL;
    int result = evalExpVars(e, vars);

    // Cleanup
    deleteVariables(vars);

    return result;
}