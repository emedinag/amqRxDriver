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


struct _mq_auth_info {
	   char*  user;
	   char  password[MQ_CSP_PASSWORD_LENGTH + 1];
	};

typedef struct _mq_auth_info mq_auth_info;

int createConnection(char* QMGR_Name, mq_auth_info auth);

int doMQPUT(char* Q_NAME, char *msg);

#endif /* CONNECTIONMANAGER_H_ */
