/*
 * utils.h
 *
 *  Created on: Jul 5, 2020
 *      Author: emanuel
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <cmqc.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct dict_t_struct {
    char *key;
    void *value;
    struct dict_t_struct *next;
} dict_t;

dict_t **dictAlloc(void);
void dictDealloc(dict_t **dict);

void *getItem(dict_t *dict, char *key);
void delItem(dict_t **dict, char *key);
void addItem(dict_t **dict, char *key, void *value);

#endif /* UTILS_H_ */
