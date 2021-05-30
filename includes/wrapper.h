/*
 * wrapper.h
 *
 *  Created on: Jul 5, 2020
 *      Author: emanuel
 */

#ifndef INCLUDES_WRAPPER_H_
#define INCLUDES_WRAPPER_H_

#include <erl_nif.h>

static ERL_NIF_TERM amqs_put(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[] );
static ERL_NIF_TERM amqs_connect(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[] );
static ERL_NIF_TERM amqs_disconnect(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[] );


static ErlNifFunc nif_focus[]= {
		{"amqs_put", 1, amqs_put},
		{"amqs_connect", 1 , amqs_connect},
		{"amqs_disconnect", 1, amqs_disconnect}
};

ERL_NIF_INIT(amqRxDriver,nif_focus,NULL,NULL,NULL,NULL);

#endif /* INCLUDES_WRAPPER_H_ */
