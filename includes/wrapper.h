/*
 * wrapper.h
 *
 *  Created on: Jul 5, 2020
 *      Author: emanuel
 */

#ifndef INCLUDES_WRAPPER_H_
#define INCLUDES_WRAPPER_H_

#include <erl_nif.h>

static ERL_NIF_TERM amqsput(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[] );

static ErlNifFunc nif_focus[]= {
		{"amqsput", 1, amqsput},
};

ERL_NIF_INIT(amqRxDriver,nif_focus,NULL,NULL,NULL,NULL);

#endif /* INCLUDES_WRAPPER_H_ */
