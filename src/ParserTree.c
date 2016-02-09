//
//  main.c
//  Parser
// Hayo Thielecke 30.12.2013

// A basic parser for expressions with +, -, * and brackets.
// Constants must be a single digit
// This code illustrates:
// nested struct and union
// trees from struct + pointer
// recursive functions
// simple memory management
// predictive parsing

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>


// expression trees from pointer+struct+union+enum

enum Etag {
    constant, plus, minus, times
};

struct E {
    enum Etag tag;
    union {
        int constant;
        struct {
            struct E *e1;
            struct E *e2;
        } plus;
        struct {
            struct E *e1;
            struct E *e2;
        } minus;
        struct {
            struct E *e1;
            struct E *e2;
        } times;
    } Eunion;
};

// allocator

#define heapsize 100 // because C99 and gcc do not like the following:

// const int heapsize = 100; // fine in Xcode

struct E myheap[heapsize];

struct E *freeptr = myheap;

struct E *myalloc() {
    if (freeptr + 1 >= myheap + heapsize) {
        fprintf(stderr, "Heap overflow.\n");
        exit(1);
    }
    return freeptr++;
}



// expression tree construction

struct E *makeconstant(int n) {
    struct E *p;
    p = myalloc();
    p->tag = constant;
    p->Eunion.constant = n;
    return p;
}

struct E *makeplus(struct E *left,
                   struct E *right) {
    struct E *p;
    p = myalloc();
    p->tag = plus;
    p->Eunion.plus.e1 = left;
    p->Eunion.plus.e2 = right;
    return p;
}

struct E *makeminus(struct E *left,
                    struct E *right) {
    struct E *p;
    p = myalloc();
    p->tag = minus;
    p->Eunion.plus.e1 = left;
    p->Eunion.plus.e2 = right;
    return p;
}

struct E *maketimes(struct E *left,
                    struct E *right) {
    struct E *p;
    p = myalloc();
    p->tag = times;
    p->Eunion.plus.e1 = left;
    p->Eunion.plus.e2 = right;
    return p;
}

// expression tree evaluation

int eval(struct E *p) {
    switch (p->tag) {
        case constant:
            return p->Eunion.constant;
        case plus:
            return eval(p->Eunion.plus.e1)
                   + eval(p->Eunion.plus.e2);
        case minus:
            return eval(p->Eunion.minus.e1)
                   - eval(p->Eunion.minus.e2);
        case times:
            return eval(p->Eunion.times.e1)
                   * eval(p->Eunion.times.e2);
        default:
            fprintf(stderr, "Invalid tag for struct E.\n\n");
            exit(1);
    }
}

// scanning input

char input[100];

char *pos = input;

char lookahead() {
    while (isspace(*pos))
        pos++;
    return *pos;
}

void syntaxerror() {
    int n = 0;
    printf("Syntax error:\n");
    printf("%s\n", input);
    while (n < pos - input) {
        n++;
        fprintf(stderr, " ");
    }
    fprintf(stderr, "^\n\n");
    exit(1);
}

void match(char c) {
    if (lookahead() == c)
        pos++;
    else
        syntaxerror();
}

void reset() {
    // re-use memory by resetting pointer to beginning of array
    // no garbage
    freeptr = myheap;
    pos = input;
}

// forward declarations of parsing functions

struct E *parseE();

struct E *parseF();

struct E *parseEprime(struct E *);

struct E *parseFprime(struct E *);

// definitions of parsing functions

struct E *parseP() // primary expression
{
    char c;
    struct E *result = 0;

    switch (c = lookahead()) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            match(c);
            result = makeconstant(c - '0');
            break;
        case '(':
            match('(');
            result = parseE();
            match(')');
            break;
        default:
            syntaxerror();
    }
    return result;
}

struct E *parseF() // factor
{
    struct E *resultOfP;
    struct E *result = NULL;

    switch (lookahead()) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '(':
            resultOfP = parseP();
            result = parseFprime(resultOfP);
            break;
        default:
            syntaxerror();
    }
    return result;
}

struct E *parseFprime(struct E *leftContext) {
    struct E *resultOfP;
    struct E *result = NULL;

    switch (lookahead()) {
        case '*':
            match('*');
            resultOfP = parseP();
            result = parseFprime(maketimes(leftContext, resultOfP));
            break;
        case ')':
        case '+':
        case '-':
        case '$':
            result = leftContext;
            break;
        default:
            syntaxerror();
    }
    return result;
}

struct E *parseE() {
    struct E *resultOfF;
    struct E *result = NULL;

    switch (lookahead()) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '(':
            resultOfF = parseF();
            result = parseEprime(resultOfF);
            break;
        default:
            syntaxerror();
    }
    return result;
}

struct E *parseEprime(struct E *leftContext) {
    struct E *resultOfF;
    struct E *result = NULL;

    switch (lookahead()) {
        case '+':
            match('+');
            resultOfF = parseF();
            result = parseEprime(makeplus(leftContext, resultOfF));
            break;
        case '-':
            match('-');
            resultOfF = parseF();
            result = parseEprime(makeminus(leftContext, resultOfF));
            break;
        case ')':
        case '$':
            result = leftContext;
            break;
        default:
            syntaxerror();
    }
    return result;
}


int main(int argc, char *argv[]) {
    printf("Expression evaluator\n\n");
    while (1) {
        printf("Expression for evaluation:\n");
        fgets(input, 80, stdin);
        if (strlen(input) < 2) break;
        input[strlen(input) - 1] = '$';
        printf("Result = %d\n\n", eval(parseE()));
        reset();
    }
    printf("Goodbye.\n\n");
}

