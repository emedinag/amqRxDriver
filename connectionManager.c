/*
 * connectionManager.c
 *
 *  Created on: Jul 5, 2020
 *      Author: emanuel
 */

#include "includes/connectionManager.h"
#include "includes/utils.h"

dict_t DICT_OHANDLER;

MQOD od = { MQOD_DEFAULT }; /* Object Descriptor             */
MQMD md = { MQMD_DEFAULT }; /* Message Descriptor            */
MQPMO pmo = { MQPMO_DEFAULT }; /* put message options           */
MQCNO cno = { MQCNO_DEFAULT }; /* connection options            */
MQCSP csp = { MQCSP_DEFAULT }; /* security parameters           */

MQHCONN Hcon; /* connection handle             */
MQHOBJ Hobj; /* object handle                 */
MQLONG O_options; /* MQOPEN options                */
MQLONG C_options; /* MQCLOSE options               */
MQLONG CompCode; /* completion code               */
MQLONG OpenCode; /* MQOPEN completion code        */
MQLONG Reason; /* reason code                   */
MQLONG CReason; /* reason code for MQCONNX       */
MQLONG messlen; /* message length                */
char buffer[65535]; /* message buffer                */
char QMName[50]; /* queue manager name            */



int createConnection(char* QMGR_Name, mq_auth_info auth){
	char *UserId; /* UserId for authentication     */
	char Password[MQ_CSP_PASSWORD_LENGTH + 1] = { 0 }; /* For auth  */

	UserId=auth.user;
	strcpy(Password, auth.password);

	if (UserId != NULL && Password[strlen(Password)]>0) {
			cno.SecurityParmsPtr = &csp;
			cno.Version = MQCNO_VERSION_5;
			csp.AuthenticationType = MQCSP_AUTH_USER_ID_AND_PWD;
			csp.CSPUserIdPtr = UserId;
			csp.CSPUserIdLength = (MQLONG) strlen(UserId);
			//if(Password['0']=='\0'){
			//if (strlen(Password) > 0 && Password[strlen(Password) - 1] == '\n')
				//Password[strlen(Password) - 1] = 0;
			//Password[strlen(Password)]=0;
			if(strlen(Password) > 0){
				csp.CSPPasswordPtr = Password;
				csp.CSPPasswordLength = (MQLONG) strlen(csp.CSPPasswordPtr);
			}
			//}
	}

	MQCONNX(QMGR_Name, /* queue manager                  */
		&cno, /* connection options             */
		&Hcon, /* connection handle              */
		&CompCode, /* completion code                */
		&CReason); /* reason code                    */

	if (CompCode == MQCC_FAILED) {
		printf("MQCONNX ended with reason code %d\n", CReason);
		return ((int) CReason);
	}
	if (CompCode == MQCC_WARNING) {
			printf("MQCONNX generated a warning with reason code %d\n", CReason);
			printf("Continuing...\n");
		}

	return 0;
}

int closeConnection(){
	if (CReason != MQRC_ALREADY_CONNECTED) {
		MQDISC(&Hcon, /* connection handle            */
		&CompCode, /* completion code              */
		&Reason); /* reason code                  */

		/* report reason, if any     */
		if (Reason != MQRC_NONE) {
			printf("MQDISC ended with reason code %d\n", Reason);
			return 1;
		}
	}
	return 0;
}

int closeObject(MQLONG Hobj){
	printf("clossing queue connection ");
	C_options = MQCO_NONE;
	MQCLOSE(Hcon, /* connection handle            */
	&Hobj, /* object handle                */
	C_options, &CompCode, /* completion code              */
	&Reason); /* reason code                  */

	/* report reason, if any     */
	if (Reason != MQRC_NONE) {
		printf("MQCLOSE ended with reason code %d\n", Reason);
	}
	return Reason;
}


int doMQPUT(char* Q_NAME, char *msg) {

	strncpy(od.ObjectName, Q_NAME, (size_t) MQ_Q_NAME_LENGTH);

/*
	if (argc > 6) {
		strncpy(od.DynamicQName, argv[6], (size_t) MQ_Q_NAME_LENGTH);
		printf("dynamic queue name is %s\n", od.DynamicQName);
	}*/
	/*if (argc > 3) {
		O_options = atoi(argv[3]);
		printf("open  options are %d\n", O_options);
	} */

	O_options = MQOO_OUTPUT /* open queue for output     */
		| MQOO_FAIL_IF_QUIESCING /* but not if MQM stopping   */
		; /* = 0x2010 = 8208 decimal   */

	MQOPEN(Hcon, /* connection handle            */
			&od, /* object descriptor for queue  */
			O_options, /* open options                 */
			&Hobj, /* object handle                */
			&OpenCode, /* MQOPEN completion code       */
			&Reason); /* reason code                  */

			/* report reason, if any; stop if failed      */
			if (Reason != MQRC_NONE) {
				printf("MQOPEN ended with reason code %d\n", Reason);
				return 1;
			}

			if (OpenCode == MQCC_FAILED) {
				printf("unable to open queue for output\n");
				return 1;
			}
	CompCode = OpenCode; /* use MQOPEN result for initial test */
	memcpy(md.Format, /* character string format            */
	MQFMT_STRING, (size_t) MQ_FORMAT_LENGTH);

	pmo.Options = MQPMO_NO_SYNCPOINT | MQPMO_FAIL_IF_QUIESCING;

	/******************************************************************/
	/* Use these options when connecting to Queue Managers that also  */
	/* support them, see the Application Programming Reference for    */
	/* details.                                                       */
	/* The MQPMO_NEW_MSG_ID option causes the MsgId to be replaced,   */
	/* so that there is no need to reset it before each MQPUT.        */
	/* The MQPMO_NEW_CORREL_ID option causes the CorrelId to be       */
	/* replaced.                                                      */
	/******************************************************************/
	/* pmo.Options |= MQPMO_NEW_MSG_ID;                               */
	/* pmo.Options |= MQPMO_NEW_CORREL_ID;                            */

	//while (CompCode != MQCC_FAILED) {
	memcpy(buffer, msg, sizeof(msg));
	messlen = (MQLONG) strlen(buffer); /* length without null      */
	if (buffer[messlen - 1] == '\n') /* last char is a new-line    */
	{
		buffer[messlen - 1] = '\0'; /* replace new-line with null */
		--messlen; /* reduce buffer length       */
	}

	//else messlen = 0;        /* treat EOF same as null line         */

	/****************************************************************/
	/*                                                              */
	/*   Put each buffer to the message queue                       */
	/*                                                              */
	/****************************************************************/
	if (messlen > 0) {
		/**************************************************************/
		/* The following statement is not required if the             */
		/* MQPMO_NEW_MSG_ID option is used.                           */
		/**************************************************************/
		memcpy(md.MsgId, /* reset MsgId to get a new one    */
		MQMI_NONE, sizeof(md.MsgId));
		MQPUT(Hcon, /* connection handle               */
		Hobj, /* object handle                   */
		&md, /* message descriptor              */
		&pmo, /* default options (datagram)      */
		messlen, /* message length                  */
		buffer, /* message buffer                  */
		&CompCode, /* completion code                 */
		&Reason); /* reason code                     */

		/* report reason, if any */
		if (Reason != MQRC_NONE) {
			printf("MQPUT ended with reason code %d\n", Reason);
		}

	} else {
		CompCode = MQCC_FAILED;
	}
	//}

	if (OpenCode != MQCC_FAILED) {

	}

	/******************************************************************/
	/*                                                                */
	/*   Disconnect from MQM if not already connected                 */
	/*                                                                */
	/******************************************************************/
	closeObject(Hobj);

	/******************************************************************/
	/*                                                                */
	/* END OF AMQSPUT0                                                */
	/*                                                                */
	/******************************************************************/

	return (0);
}
