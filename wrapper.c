/*
 * wrapper.c
 *
 *  Created on: Jul 5, 2020
 *      Author: emanuel
 */

#include "includes/wrapper.h"
#include "includes/connectionManager.h"
#include "includes/utils.h"

static ERL_NIF_TERM amqs_connect(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[] ){

	mq_auth_info auth;
	auth.user="emgomez";
	//strcpy(auth.password, "");
	int result = createConnection("QM", auth);
	return enif_make_int(env, result);
}


static ERL_NIF_TERM amqs_disconnect(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[] ){
	int result = closeConnection();
	return enif_make_int(env, result);
}


static ERL_NIF_TERM amqs_put(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[] ){
	int bfz=sizeof(char)*65534;
	char* result = malloc(bfz);
	int getstr=enif_get_string(env, argv[0], result, bfz, ERL_NIF_LATIN1);
	doMQPUT("DEV.QUEUE.1", result);
	return enif_make_string(env, result, ERL_NIF_LATIN1);

/*

	{
	    int a = 0;
	    int b = 0;

	    if (!enif_get_int(env, argv[0], &a)) {
	        return enif_make_badarg(env);
	    }
	    if (!enif_get_int(env, argv[1], &b)) {
	        return enif_make_badarg(env);
	    }

	    int result = add(a, b);
	    return enif_make_int(env, result);
	}
	*/
}



