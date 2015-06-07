/*
 * Event loop based on select() loop
 * Copyright (c) 2002-2005, Jouni Malinen <jkmaline@cc.hut.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#ifdef CSCM_ANDROID
#include <unistd.h>
#else
#include <sys/unistd.h>
#endif
#include <errno.h>
#include <signal.h>

#ifdef BECEEM_CSCM
#include "common.h"
#endif 

#include "eap_supplicant.h"
#include "eloop.h"

#define ELOOP_TRACE

struct eloop_sock {
	int sock;
	void *eloop_data;
	void *user_data;
	void (*handler)(int sock, void *eloop_ctx, void *sock_ctx);
};

struct eloop_timeout {
	struct timeval time;
	void *eloop_data;
	void *user_data;
	void (*handler)(void *eloop_ctx, void *sock_ctx);
	struct eloop_timeout *next;
};

struct eloop_signal {
	int sig;
	void *user_data;
	void (*handler)(int sig, void *eloop_ctx, void *signal_ctx);
	int signaled;
};

struct eloop_data {
	void *user_data;

	int max_sock, reader_count;
	struct eloop_sock *readers;

	struct eloop_timeout *timeout;

	int signal_count;
	struct eloop_signal *signals;
	int signaled;
	int pending_terminate;

	int running;
	int wimax_mode;
	
	int terminate;
	int reader_table_changed;
};

static struct eloop_data eloop;
static int g_eloop_polling_interval_us;

void eloop_init(int eloop_polling_interval_ms, void *user_data)
{
#ifdef ELOOP_TRACE
	wpa_printf(MSG_DEBUG,"**** eloop_init called - interval = %d ms",
		eloop_polling_interval_ms);
#endif
	memset(&eloop, 0, sizeof(eloop));
	eloop.user_data = user_data;
	g_eloop_polling_interval_us = 1000 * eloop_polling_interval_ms;
}


int eloop_register_read_sock(int sock,
			     void (*handler)(int sock, void *eloop_ctx,
					     void *sock_ctx),
			     void *eloop_data, void *user_data)
{
	struct eloop_sock *tmp;

	tmp = (struct eloop_sock *)
		realloc(eloop.readers,
			(eloop.reader_count + 1) * sizeof(struct eloop_sock));
	if (tmp == NULL)
		return -1;

	tmp[eloop.reader_count].sock = sock;
	tmp[eloop.reader_count].eloop_data = eloop_data;
	tmp[eloop.reader_count].user_data = user_data;
	tmp[eloop.reader_count].handler = handler;
	eloop.reader_count++;
	eloop.readers = tmp;
	if (sock > eloop.max_sock)
		eloop.max_sock = sock;
	eloop.reader_table_changed = 1;

	return 0;
}


void eloop_unregister_read_sock(int sock)
{
	int i;

	if (eloop.readers == NULL || eloop.reader_count == 0)
		return;

	for (i = 0; i < eloop.reader_count; i++) {
		if (eloop.readers[i].sock == sock)
			break;
	}
	if (i == eloop.reader_count)
		return;
	if (i != eloop.reader_count - 1) {
		memmove(&eloop.readers[i], &eloop.readers[i + 1],
			(eloop.reader_count - i - 1) *
			sizeof(struct eloop_sock));
	}
	eloop.reader_count--;
	eloop.reader_table_changed = 1;
}

int eloop_register_timeout(unsigned int secs, unsigned int usecs,
			   void (*handler)(void *eloop_ctx, void *timeout_ctx),
			   void *eloop_data, void *user_data)
{
	struct eloop_timeout *timeout, *tmp, *prev;

#ifdef ELOOP_TRACE
	wpa_printf(MSG_DEBUG,
		"**** eloop_register_timeout: %3u.%06u %8p %8p %8p",
		secs, usecs, handler, eloop_data, user_data);
#endif

	timeout = (struct eloop_timeout *) malloc(sizeof(*timeout));
	if (timeout == NULL)
		return -1;
	gettimeofday(&timeout->time, NULL);
	timeout->time.tv_sec += secs;
	timeout->time.tv_usec += usecs;
	while (timeout->time.tv_usec >= 1000000) {
		timeout->time.tv_sec++;
		timeout->time.tv_usec -= 1000000;
	}
	timeout->eloop_data = eloop_data;
	timeout->user_data = user_data;
	timeout->handler = handler;
	timeout->next = NULL;

	if (eloop.timeout == NULL) {
		eloop.timeout = timeout;
		return 0;
	}

	prev = NULL;
	tmp = eloop.timeout;
	while (tmp != NULL) {
		if (timercmp(&timeout->time, &tmp->time, <))
			break;
		prev = tmp;
		tmp = tmp->next;
	}

	if (prev == NULL) {
		timeout->next = eloop.timeout;
		eloop.timeout = timeout;
	} else {
		timeout->next = prev->next;
		prev->next = timeout;
	}

	return 0;
}

int eloop_cancel_timeout(void (*handler)(void *eloop_ctx, void *sock_ctx),
			 void *eloop_data, void *user_data)
{
	struct eloop_timeout *timeout, *prev, *next;
	int removed = 0;

#ifdef ELOOP_TRACE
	wpa_printf(MSG_DEBUG, 
		"**** eloop_cancel_timeout:              %8p %8p %8p\n",
		handler, eloop_data, user_data);
#endif

	prev = NULL;
	timeout = eloop.timeout;
	while (timeout != NULL) {
		next = timeout->next;

		if (timeout->handler == handler &&
		    (timeout->eloop_data == eloop_data ||
		     eloop_data == ELOOP_ALL_CTX) &&
		    (timeout->user_data == user_data ||
		     user_data == ELOOP_ALL_CTX)) {
			if (prev == NULL)
				eloop.timeout = next;
			else
				prev->next = next;
			free(timeout);
			removed++;
		} else
			prev = timeout;

		timeout = next;
	}

	return removed;
}


#ifndef CONFIG_NATIVE_WINDOWS
static void eloop_handle_alarm(int sig)
{
	fprintf(stderr, "eloop: could not process SIGINT or SIGTERM in two "
		"seconds. Looks like there\n"
		"is a bug that ends up in a busy loop that "
		"prevents clean shutdown.\n"
		"Killing program forcefully.\n");
	exit(1);
}
#endif /* CONFIG_NATIVE_WINDOWS */


static void eloop_handle_signal(int sig)
{
	int i;

#ifndef CONFIG_NATIVE_WINDOWS
	if ((sig == SIGINT || sig == SIGTERM) && !eloop.pending_terminate) {
		/* Use SIGALRM to break out from potential busy loops that
		 * would not allow the program to be killed. */
		eloop.pending_terminate = 1;
		signal(SIGALRM, eloop_handle_alarm);
		alarm(2);
	}
#endif /* CONFIG_NATIVE_WINDOWS */

	eloop.signaled++;
	for (i = 0; i < eloop.signal_count; i++) {
		if (eloop.signals[i].sig == sig) {
			eloop.signals[i].signaled++;
			break;
		}
	}
}


static void eloop_process_pending_signals(void)
{
	int i;

	if (eloop.signaled == 0)
		return;
	eloop.signaled = 0;

	if (eloop.pending_terminate) {
#ifndef CONFIG_NATIVE_WINDOWS
		alarm(0);
#endif /* CONFIG_NATIVE_WINDOWS */
		eloop.pending_terminate = 0;
	}

	for (i = 0; i < eloop.signal_count; i++) {
		if (eloop.signals[i].signaled) {
			eloop.signals[i].signaled = 0;
			eloop.signals[i].handler(eloop.signals[i].sig,
						 eloop.user_data,
						 eloop.signals[i].user_data);
		}
	}
}

int eloop_register_signal(int sig,
			  void (*handler)(int sig, void *eloop_ctx,
					  void *signal_ctx),
			  void *user_data)
{
	struct eloop_signal *tmp;

	tmp = (struct eloop_signal *)
		realloc(eloop.signals,
			(eloop.signal_count + 1) *
			sizeof(struct eloop_signal));
	if (tmp == NULL)
		return -1;

	tmp[eloop.signal_count].sig = sig;
	tmp[eloop.signal_count].user_data = user_data;
	tmp[eloop.signal_count].handler = handler;
	tmp[eloop.signal_count].signaled = 0;
	eloop.signal_count++;
	eloop.signals = tmp;
	signal(sig, eloop_handle_signal);

	return 0;
}

// Set wimax_mode = 1 to disable "readers" processing
void eloop_run(int wimax_mode)
{
	fd_set *rfds;
	int i, res = 0;
	struct timeval tv, now;

	eloop.running = 1;
	eloop.wimax_mode = wimax_mode;
	
	rfds = malloc(sizeof(*rfds));
	if (rfds == NULL) {
		printf("eloop_run - malloc failed\n");
		eloop.running = 0;
		return;
	}

#ifdef ELOOP_TRACE
	wpa_printf(MSG_DEBUG,
		"**** eloop_run start: eloop.terminate=%d eloop.timeout=%8p eloop.reader_count=%d",
		eloop.terminate, eloop.timeout, eloop.reader_count);
#endif

	while (!eloop.terminate &&
		(eloop.timeout || eloop.reader_count > 0 || wimax_mode)) {

		if (!wimax_mode) {

			if (eloop.timeout) {
				gettimeofday(&now, NULL);
				if (timercmp(&now, &eloop.timeout->time, <))
					timersub(&eloop.timeout->time, &now, &tv);
				else
					tv.tv_sec = tv.tv_usec = 0;
	#ifdef ELOOP_TRACE
				wpa_printf(MSG_DEBUG,
						"**** next eloop timeout in   %3lu.%06lu %8p %8p %8p",
						tv.tv_sec, tv.tv_usec,
						eloop.timeout->handler,
						eloop.timeout->eloop_data,
						eloop.timeout->user_data);
	#endif
			}

			FD_ZERO(rfds);
			for (i = 0; i < eloop.reader_count; i++)
				FD_SET(eloop.readers[i].sock, rfds);
			res = select(eloop.max_sock + 1, rfds, NULL, NULL,
					 eloop.timeout ? &tv : NULL);
			if (res < 0 && errno != EINTR) {
				perror("select");
				free(rfds);
				eloop.running = 0;
				return;
			}
			eloop_process_pending_signals();
		}

		/* check if some registered timeouts have occurred */
		if (eloop.timeout) {
			struct eloop_timeout *tmp;

			gettimeofday(&now, NULL);
			if (!timercmp(&now, &eloop.timeout->time, <)) {
				tmp = eloop.timeout;
				eloop.timeout = eloop.timeout->next;
#ifdef ELOOP_TRACE
	wpa_printf(MSG_DEBUG,
		"**** eloop_run calls timeout->handler: %8p %8p %8p",
		tmp->handler, tmp->eloop_data, tmp->user_data);
#endif
				tmp->handler(tmp->eloop_data,
					     tmp->user_data);
				free(tmp);
			}

		}
		
		if (!wimax_mode) {
			if (res <= 0)
				continue;

			eloop.reader_table_changed = 0;
			for (i = 0; i < eloop.reader_count; i++) {
				if (FD_ISSET(eloop.readers[i].sock, rfds)) {
					eloop.readers[i].handler(
						eloop.readers[i].sock,
						eloop.readers[i].eloop_data,
						eloop.readers[i].user_data);
					if (eloop.reader_table_changed)
						break;
				}
			}
		} else {
			usleep(g_eloop_polling_interval_us);
		}

	}

	free(rfds);

#ifdef ELOOP_TRACE
	wpa_printf(MSG_DEBUG,
		"**** eloop_run end: eloop.terminate=%d eloop.timeout=%8p eloop.reader_count=%d", 
		eloop.terminate, eloop.timeout, eloop.reader_count);
#endif
	eloop.running = 0;
	wpa_printf(MSG_DEBUG, "**** eloop_run is returning");
}

void eloop_terminate(void)
{
#ifdef ELOOP_TRACE
	wpa_printf(MSG_DEBUG, "**** eloop_terminate");
#endif
	eloop.terminate = 1;
}

void eloop_clear_terminate(void)
{	
#ifdef ELOOP_TRACE
	wpa_printf(MSG_DEBUG, "**** eloop_clear_terminate");
#endif
	eloop.terminate = 0;
}

void eloop_wait_terminate(void)
{

	eloop_terminate();
	
#ifdef ELOOP_TRACE
	if (!eloop.running) {
		wpa_printf(MSG_DEBUG, "**** eloop is not running");
		return;
		}
#endif

	if (eloop.wimax_mode)
		while (eloop.running)
			usleep(2000);
		
#ifdef ELOOP_TRACE
	wpa_printf(MSG_DEBUG, "**** eloop terminated");
#endif

}

void eloop_destroy(void)
{
	struct eloop_timeout *timeout, *prev;

	timeout = eloop.timeout;
	while (timeout != NULL) {
		prev = timeout;
		timeout = timeout->next;
		free(prev);
	}
	free(eloop.readers);
	free(eloop.signals);
}

int eloop_terminated(void)
{
	return eloop.terminate;
}
