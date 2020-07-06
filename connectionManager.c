/*
 * connectionManager.c
 *
 *  Created on: Jul 5, 2020
 *      Author: emanuel
 */

#include "includes/connectionManager.h"

int doMQPUT(int argc, char **argv, char *msg) {
	/*  Declare file and character for sample input                   */
	//FILE *fp;
	/*   Declare MQI structures needed                                */
	MQOD od = { MQOD_DEFAULT }; /* Object Descriptor             */
	MQMD md = { MQMD_DEFAULT }; /* Message Descriptor            */
	MQPMO pmo = { MQPMO_DEFAULT }; /* put message options           */
	MQCNO cno = { MQCNO_DEFAULT }; /* connection options            */
	MQCSP csp = { MQCSP_DEFAULT }; /* security parameters           */
	/** note, sample uses defaults where it can **/

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
	char *UserId; /* UserId for authentication     */
	char Password[MQ_CSP_PASSWORD_LENGTH + 1] = { 0 }; /* For auth  */

	printf("Sample AMQSPUT0 start\n");
	printf("=====================\n");
	for (int i = 0; i < argc; i++) {
		printf("value %s\n", argv[i]);
	}

	if (argc < 2) {
		printf("Required parameter missing - queue name\n");
		exit(99);
	}

	/******************************************************************/
	/* Setup any authentication information supplied in the local     */
	/* environment. The connection options structure points to the    */
	/* security structure. If the userid is set, then the password    */
	/* is read from the terminal. Having the password entered this    */
	/* way avoids it being accessible from other programs that can    */
	/* inspect command line parameters or environment variables.      */
	/******************************************************************/
	UserId = getenv("MQSAMP_USER_ID");
	if (UserId != NULL) {
		/****************************************************************/
		/* Set the connection options to use the security structure and */
		/* set version information to ensure the structure is processed.*/
		/****************************************************************/
		cno.SecurityParmsPtr = &csp;
		cno.Version = MQCNO_VERSION_5;

		csp.AuthenticationType = MQCSP_AUTH_USER_ID_AND_PWD;
		csp.CSPUserIdPtr = UserId;
		csp.CSPUserIdLength = (MQLONG) strlen(UserId);

		/****************************************************************/
		/* Get the password. This is very simple, and does not turn off */
		/* echoing or replace characters with '*'.                      */
		/****************************************************************/
		printf("Enter password: ");

		fgets(Password, sizeof(Password) - 1, stdin);

		if (strlen(Password) > 0 && Password[strlen(Password) - 1] == '\n')
			Password[strlen(Password) - 1] = 0;
		csp.CSPPasswordPtr = Password;
		csp.CSPPasswordLength = (MQLONG) strlen(csp.CSPPasswordPtr);
	}

	/******************************************************************/
	/*                                                                */
	/*   Connect to queue manager                                     */
	/*                                                                */
	/******************************************************************/
	QMName[0] = 0; /* default */
	if (argc > 2)
		strncpy(QMName, argv[2], (size_t) MQ_Q_MGR_NAME_LENGTH);

	MQCONNX(QMName, /* queue manager                  */
	&cno, /* connection options             */
	&Hcon, /* connection handle              */
	&CompCode, /* completion code                */
	&CReason); /* reason code                    */

	/* report reason and stop if it failed     */
	if (CompCode == MQCC_FAILED) {
		printf("MQCONNX ended with reason code %d\n", CReason);
		return ((int) CReason);

	}

	/* if there was a warning report the cause and continue */
	if (CompCode == MQCC_WARNING) {
		printf("MQCONNX generated a warning with reason code %d\n", CReason);
		printf("Continuing...\n");
	}
	/******************************************************************/
	/*                                                                */
	/*   Use parameter as the name of the target queue                */
	/*                                                                */
	/******************************************************************/
	strncpy(od.ObjectName, argv[1], (size_t) MQ_Q_NAME_LENGTH);
	printf("target queue is %s\n", od.ObjectName);

	if (argc > 5) {
		strncpy(od.ObjectQMgrName, argv[5], (size_t) MQ_Q_MGR_NAME_LENGTH);
		printf("target queue manager is %s\n", od.ObjectQMgrName);
	}

	if (argc > 6) {
		strncpy(od.DynamicQName, argv[6], (size_t) MQ_Q_NAME_LENGTH);
		printf("dynamic queue name is %s\n", od.DynamicQName);
	}

	/******************************************************************/
	/*                                                                */
	/*   Open the target message queue for output                     */
	/*                                                                */
	/******************************************************************/
	if (argc > 3) {
		O_options = atoi(argv[3]);
		printf("open  options are %d\n", O_options);
	} else {
		O_options = MQOO_OUTPUT /* open queue for output     */
		| MQOO_FAIL_IF_QUIESCING /* but not if MQM stopping   */
		; /* = 0x2010 = 8208 decimal   */
	}

	MQOPEN(Hcon, /* connection handle            */
	&od, /* object descriptor for queue  */
	O_options, /* open options                 */
	&Hobj, /* object handle                */
	&OpenCode, /* MQOPEN completion code       */
	&Reason); /* reason code                  */

	/* report reason, if any; stop if failed      */
	if (Reason != MQRC_NONE) {
		printf("MQOPEN ended with reason code %d\n", Reason);
	}

	if (OpenCode == MQCC_FAILED) {
		printf("unable to open queue for output\n");
	}

	/******************************************************************/
	/*                                                                */
	/*   Read lines from the file and put them to the message queue   */
	/*   Loop until null line or end of file, or there is a failure   */
	/*                                                                */
	/******************************************************************/
	CompCode = OpenCode; /* use MQOPEN result for initial test */
	//fp = stdin;

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
		} else
			/* satisfy end condition when empty line is read */
			CompCode = MQCC_FAILED;
	//}

	/******************************************************************/
	/*                                                                */
	/*   Close the target queue (if it was opened)                    */
	/*                                                                */
	/******************************************************************/
	if (OpenCode != MQCC_FAILED) {
		if (argc > 4) {
			C_options = atoi(argv[4]);
			printf("close options are %d\n", C_options);
		} else {
			C_options = MQCO_NONE; /* no close options             */
		}

		MQCLOSE(Hcon, /* connection handle            */
		&Hobj, /* object handle                */
		C_options, &CompCode, /* completion code              */
		&Reason); /* reason code                  */

		/* report reason, if any     */
		if (Reason != MQRC_NONE) {
			printf("MQCLOSE ended with reason code %d\n", Reason);
		}
	}

	/******************************************************************/
	/*                                                                */
	/*   Disconnect from MQM if not already connected                 */
	/*                                                                */
	/******************************************************************/
	if (CReason != MQRC_ALREADY_CONNECTED) {
		MQDISC(&Hcon, /* connection handle            */
		&CompCode, /* completion code              */
		&Reason); /* reason code                  */

		/* report reason, if any     */
		if (Reason != MQRC_NONE) {
			printf("MQDISC ended with reason code %d\n", Reason);
		}
	}

	/******************************************************************/
	/*                                                                */
	/* END OF AMQSPUT0                                                */
	/*                                                                */
	/******************************************************************/
	printf("Sample AMQSPUT0 end\n");
	return (0);
}
