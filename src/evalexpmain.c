// main function for evaluation expression exercise

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "evalexp.h"

// tree construction functions

struct exp *mkvar(char *cp) {
    struct exp *ep = malloc(sizeof(struct exp));
    ep->tag = isvar;
    strncpy(ep->var, cp, 8);
    return ep;
};

struct exp *mkconstant(int n) {
    struct exp *ep = malloc(sizeof(struct exp));
    ep->tag = isconstant;
    ep->constant = n;
    return ep;
};

struct exp *mkopapp(enum op op, struct explist *el) {
    struct exp *ep = malloc(sizeof(struct exp));
    ep->tag = isopapp;
    ep->op = op;
    ep->exps = el;
    return ep;
};

struct exp *mklet(char *cp, struct exp *e1, struct exp *e2) {
    struct exp *ep = malloc(sizeof(struct exp));
    ep->tag = islet;
    strncpy(ep->bvar, cp, 8);
    ep->bexp = e1;
    ep->body = e2;
    return ep;
};

struct explist *cons(struct exp *e, struct explist *l) {
    struct explist *l2 = malloc(sizeof(struct explist));
    l2->head = e;
    l2->tail = l;
    return l2;
}

// main for testing

int main(int argc, const char *argv[]) {
    /**
     * Test 1
     */
    struct exp *e1, *e2;
    struct explist *l;

    l = NULL;
    l = cons(mkconstant(5), l);
    l = cons(mkconstant(3), l);
    l = cons(mkconstant(2), l);
    e1 = mkopapp(isplus, l);

    l = NULL;
    l = cons(mkvar("x"), l);
    l = cons(mkvar("x"), l);
    l = cons(mkvar("x"), l);
    e2 = mkopapp(ismult, l);

    e1 = mklet("x", e1, e2);

    printf("%d\n", evalexp(e1)); // should print  1000

    /**
     * Test 2
     */
    struct exp *e3, *e4, *e5;

    l = NULL;
    l = cons(mkconstant(5), l);
    l = cons(mkconstant(3), l);
    l = cons(mkconstant(2), l);
    e1 = mkopapp(isplus, l);

    l = NULL;
    l = cons(mkvar("y"), l);
    l = cons(mkvar("x"), l);
    l = cons(mkvar("z"), l);
    e2 = mkopapp(ismult, l);

    e3 = mklet("x", e1, e2);

    e4 = mklet("y", mkconstant(5), e3);

    e5 = mklet("z", mkconstant(40), e4);

    printf("%d\n", evalexp(e5)); // should print 2000

    /**
     * Test 3
     */
    l = NULL;
    l = cons(mkconstant(3), l);
    l = cons(mkvar("x"), l);
    e2 = mkopapp(isplus, l);

    l = NULL;
    l = cons(mkconstant(9), l);
    l = cons(mkvar("x"), l);
    e5 = mkopapp(isplus, l);

    e4 = mklet("x", mkconstant(4), e5);

    l = NULL;
    l = cons(e4, l);
    l = cons(mkvar("x"), l);
    e3 = mkopapp(isplus, l);

    e1 = mklet("x", mklet("x", mkconstant(2), e2), e3);

    printf("%d\n", evalexp(e1)); // should print 18

    return 0;
}