/*
 * utils.h
 *
 *  Created on: Jul 5, 2020
 *      Author: emanuel
 */

#ifndef UTILS_H_
#define UTILS_H_

char **new_argv(int count, ...)
{
    va_list args;
    int i;
    char **argv = malloc((count+1) * sizeof(char*));
    char *temp;
    va_start(args, count);
    for (i = 0; i < count; i++) {
        temp = va_arg(args, char*);
        argv[i] = malloc(sizeof(temp));
        argv[i] = temp;
    }
    argv[i] = NULL;
    va_end(args);
    return argv;
}

#endif /* UTILS_H_ */
