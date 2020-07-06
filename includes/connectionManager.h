/*
 * connectionManager.h
 *
 *  Created on: Jul 5, 2020
 *      Author: emanuel
 */

#ifndef CONNECTIONMANAGER_H_
#define CONNECTIONMANAGER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* includes for MQI */
#include <cmqc.h>

int doMQPUT(int argc, char **argv, char* msg);

#endif /* CONNECTIONMANAGER_H_ */
