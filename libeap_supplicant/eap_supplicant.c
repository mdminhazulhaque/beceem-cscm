/*
 * WPA Supplicant - test code
 * Copyright (c) 2003-2006, Jouni Malinen <jkmaline@cc.hut.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 *
 * IEEE 802.1X Supplicant test code (to be used in place of wpa_supplicant.c.
 * Not used in production version.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#ifdef CSCM_ANDROID
#include <unistd.h>
#else
#include <sys/unistd.h>
#endif
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#ifndef CONFIG_NATIVE_WINDOWS
#include <netinet/in.h>
#include <arpa/inet.h>
#endif /* CONFIG_NATIVE_WINDOWS */
#include <assert.h>

#include "common.h"
#include "config.h"
#include "eapol_sm.h"
#include "eloop.h"
#include "wpa.h"
#include "eap_i.h"
#include "wpa_supplicant.h"
#include "wpa_supplicant_i.h"
#include "l2_packet.h"
#include "ctrl_iface.h"
#include "pcsc_funcs.h"
#include "radius.h"
#include "radius_client.h"

#include "eap_supplicant.h"
#ifndef CSCM_ANDROID
#include "openssl/engine.h"
#endif
#include "openssl/evp.h"

static struct in_addr own_ip_addr;

// Say own ip address is 10.10.20.151
// The radius server is configured to accept anything from 10.10.20.x
// #define OWN_IP_ADDR (10<<24) | (10<<16) | (20<<8) | 151

PFN_EAPSUP_CALLBACK pfnEAPSUPcallback;

extern int wpa_debug_level;
extern int wpa_debug_show_keys;

struct eapol_test_data eapol_test;
struct wpa_supplicant wpa_s;
static int ieee802_1x_rx_is_ready;

struct eapol_test_data *p_eapol_test;
struct wpa_supplicant *p_wpa_s;

extern FILE * g_fpWpaPrintf;

struct wpa_driver_ops *wpa_supplicant_drivers[] = { };

static char *eap_type_text(u8 type)
{
	switch (type) {
	case EAP_TYPE_IDENTITY: return "Identity";
	case EAP_TYPE_NOTIFICATION: return "Notification";
	case EAP_TYPE_NAK: return "Nak";
	case EAP_TYPE_TLS: return "TLS";
	case EAP_TYPE_TTLS: return "TTLS";
	case EAP_TYPE_PEAP: return "PEAP";
	case EAP_TYPE_SIM: return "SIM";
	case EAP_TYPE_GTC: return "GTC";
	case EAP_TYPE_MD5: return "MD5";
	case EAP_TYPE_OTP: return "OTP";
	default: return "Unknown";
	}
}

static FILE *debug_print_file;

static void eapol_sm_reauth(void *eloop_ctx, void *timeout_ctx);
static int eapol_test_compare_pmk(struct eapol_test_data *e);
static void send_eap_request_identity(void *eloop_ctx, void *timeout_ctx);

static void eapol_sm_cb(struct eapol_sm *eapol, int success, void *ctx)
{
	struct eapol_test_data *e = ctx;
	wpa_printf(MSG_INFO, "eapol_sm_cb: success=%d num_reauths=%d", 
		success, e->eapol_test_num_reauths);
	e->eapol_test_num_reauths--;
	if (e->eapol_test_num_reauths < 0)
		eloop_terminate();
	else {
		eapol_test_compare_pmk(e);
		eloop_register_timeout(0, 100000, eapol_sm_reauth, e, NULL);
	}
}

static void eapol_sm_reauth(void *eloop_ctx, void *timeout_ctx)
{
	struct eapol_test_data *e = eloop_ctx;
	wpa_printf(MSG_INFO,
			"\n\n\n\n\neapol_test: Triggering EAP reauthentication\n\n");
	e->radius_access_accept_received = 0;
	send_eap_request_identity(e->wpa_s, NULL);
}

static void test_eapol_clean(struct eapol_test_data *e,
			     struct wpa_supplicant *wpa_s)
{

// this block was moved to eap_radius.c (eap_radius_open())
#ifdef UNUSED_CODE
	radius_client_deinit(e->radius);
	free(e->last_eap_radius);
	if (e->last_recv_radius) {
		radius_msg_free(e->last_recv_radius);
		free(e->last_recv_radius);
	}
#endif
	free(e->eap_identity);
	e->eap_identity = NULL;
	free(e->user_identity);
	e->user_identity = NULL;
	free(e->user_password);
	e->user_password = NULL;
	eapol_sm_deinit(wpa_s->eapol);
	wpa_s->eapol = NULL;
	if (e->radius_conf && e->radius_conf->auth_server) {
		free(e->radius_conf->auth_server->shared_secret);
		free(e->radius_conf->auth_server);
	}
	free(e->radius_conf);
	e->radius_conf = NULL;
	//scard_deinit(wpa_s->scard);
	//wpa_supplicant_ctrl_iface_deinit(wpa_s);
	// printf("Calling wpa_config_free\n");
	wpa_config_free(wpa_s->conf);
	// printf("Out of wpa_config_free\n");
}

static void eapol_test_eapol_done_cb(void *ctx)
{
	wpa_printf(MSG_INFO, "WPA: EAPOL processing complete\n");
}

static int eapol_test_compare_pmk(struct eapol_test_data *e)
{
	u8 pmk[PMK_LEN];
	int ret = 1;

	if (eapol_sm_get_key(e->wpa_s->eapol, pmk, PMK_LEN) == 0) {
		wpa_hexdump(MSG_DEBUG, "PMK from EAPOL", pmk, PMK_LEN);
		if (memcmp(pmk, e->authenticator_pmk, PMK_LEN) != 0)
			printf("WARNING: PMK mismatch\n");
		else if (e->radius_access_accept_received)
			ret = 0;
	} else if (e->authenticator_pmk_len == 16 &&
		   eapol_sm_get_key(e->wpa_s->eapol, pmk, 16) == 0) {
		wpa_hexdump(MSG_DEBUG, "LEAP PMK from EAPOL", pmk, 16);
		if (memcmp(pmk, e->authenticator_pmk, 16) != 0)
			printf("WARNING: PMK mismatch\n");
		else if (e->radius_access_accept_received)
			ret = 0;
	} else if (e->radius_access_accept_received && e->no_mppe_keys) {
		/* No keying material expected */
		ret = 0;
	}

	if (ret)
		e->num_mppe_mismatch++;
	else if (!e->no_mppe_keys)
		e->num_mppe_ok++;

	return ret;
}

static const struct wpa_config_blob *
eapol_test_get_config_blob(void *ctx, const char *name)
{
	struct wpa_supplicant *wpa_s = ctx;
	return wpa_config_get_blob(wpa_s->conf, name);
}

static void eapol_test_set_config_blob(void *ctx,
				       struct wpa_config_blob *blob)
{
	struct wpa_supplicant *wpa_s = ctx;
	wpa_config_set_blob(wpa_s->conf, blob);
}

static int eapol_test_eapol_send(void *ctx, int type, const u8 *buf,
				 size_t len)
{
	/* struct wpa_supplicant *wpa_s = ctx; */
	if (type == IEEE802_1X_TYPE_EAP_PACKET) {
		wpa_hexdump(MSG_MSGDUMP, "TX EAP -> RADIUS", buf, len);
		pfnEAPSUPcallback (p_eapol_test, buf, len);
	}
	return 0;
}

static void send_eap_request_identity(void *eloop_ctx, void *timeout_ctx)
{
	struct wpa_supplicant *wpa_s = eloop_ctx;
	u8 buf[100], *pos;
	struct ieee802_1x_hdr *hdr;
	struct eap_hdr *eap;

	hdr = (struct ieee802_1x_hdr *) buf;
	hdr->version = EAPOL_VERSION;
	hdr->type = IEEE802_1X_TYPE_EAP_PACKET;
	hdr->length = htons(5);

	eap = (struct eap_hdr *) (hdr + 1);
	eap->code = EAP_CODE_REQUEST;
	eap->identifier = 0;
	eap->length = htons(5);
	pos = (u8 *) (eap + 1);
	*pos = EAP_TYPE_IDENTITY;

	wpa_printf(MSG_INFO, "Sending fake EAP-Request-Identity\n");
	eapol_sm_rx_eapol(wpa_s->eapol, wpa_s->bssid, buf,
			  sizeof(*hdr) + 5);
}

void eapol_test_terminate(int sig, void *eloop_ctx, void *signal_ctx)
{
	struct wpa_supplicant *wpa_s = eloop_ctx;
	wpa_msg(wpa_s, MSG_INFO, "Signal %d received - terminating", sig);
	eloop_terminate();
}

int test_eapol(struct eapol_test_data *e, struct wpa_supplicant *wpa_s,
		      struct wpa_ssid *ssid)
{
	struct eapol_config eapol_conf;
	struct eapol_ctx *ctx;

	ctx = malloc(sizeof(*ctx));
	if (ctx == NULL) {
		wpa_printf(MSG_ERROR, "Failed to allocate EAPOL context.\n");
		return -1;
	}
	memset(ctx, 0, sizeof(*ctx));
	ctx->ctx = wpa_s;
	ctx->msg_ctx = wpa_s;
	ctx->scard_ctx = wpa_s->scard;
	ctx->cb = eapol_sm_cb;
	ctx->cb_ctx = e;
	ctx->eapol_send_ctx = wpa_s;
	ctx->preauth = 0;
	ctx->eapol_done_cb = eapol_test_eapol_done_cb;
	ctx->eapol_send = eapol_test_eapol_send;
	ctx->set_config_blob = eapol_test_set_config_blob;
	ctx->get_config_blob = eapol_test_get_config_blob;
	ctx->opensc_engine_path = wpa_s->conf->opensc_engine_path;
	ctx->pkcs11_engine_path = wpa_s->conf->pkcs11_engine_path;
	ctx->pkcs11_module_path = wpa_s->conf->pkcs11_module_path;

	wpa_s->eapol = eapol_sm_init(ctx);
	if (wpa_s->eapol == NULL) {
		free(ctx);
		wpa_printf(MSG_ERROR, "Failed to initialize EAPOL state machines.\n");
		return -1;
	}

	wpa_s->current_ssid = ssid;
	memset(&eapol_conf, 0, sizeof(eapol_conf));
	eapol_conf.accept_802_1x_keys = 1;
	eapol_conf.required_keys = 0;
	eapol_conf.fast_reauth = wpa_s->conf->fast_reauth;
	eapol_conf.workaround = ssid->eap_workaround;
	eapol_sm_notify_config(wpa_s->eapol, ssid, &eapol_conf);
	eapol_sm_register_scard_ctx(wpa_s->eapol, wpa_s->scard);

	eapol_sm_notify_portValid(wpa_s->eapol, FALSE);
	/* 802.1X::portControl = Auto */
	eapol_sm_notify_portEnabled(wpa_s->eapol, TRUE);

	return 0;
}

void eapol_test_timeout(void *eloop_ctx, void *timeout_ctx)
{
	struct eapol_test_data *e = eloop_ctx;
	printf("EAPOL test timed out\n");
	e->auth_timed_out = 1;
	eloop_terminate();
}

void wpa_init_conf(struct eapol_test_data *e,
			  struct wpa_supplicant *wpa_s) 
{
	struct wpa_ssid *pssid;
	struct in_addr *pst_own_ip_addr;

	wpa_s->bssid[5] = 1;
	wpa_s->own_addr[5] = 2;
	pst_own_ip_addr = &own_ip_addr;
	e->pst_own_ip_addr = (void *) pst_own_ip_addr;
	pst_own_ip_addr->s_addr = htonl((127 << 24) | 1);
	// e->own_ip_addr.s_addr = htonl((127 << 24) | 1);
	// e->own_ip_addr.s_addr = htonl(OWN_IP_ADDR);
	strncpy(wpa_s->ifname, "test", sizeof(wpa_s->ifname));

	// Added by CMP 16Aug2006 (used in eap_radius.c)
	pssid = wpa_s->conf->ssid;
	e->user_identity_len = pssid->identity_len;
	e->user_password_len = pssid->password_len;
	
	e->user_identity = malloc(e->user_identity_len);
	assert(e->user_identity != NULL);
	strncpy((char *) e->user_identity, (char *) pssid->identity, pssid->identity_len);

	e->user_password = malloc(e->user_password_len);
	assert(e->user_password != NULL);
	strncpy((char *) e->user_password, (char *) pssid->password, pssid->password_len);
	// End CMP add

}

CSCM_EXPORT int build_EAP_request_identity_packet (u8 *buffer, size_t buffer_len)
{

	u8 *pos;
	struct eap_hdr *eap;

	if (buffer_len < sizeof(struct eap_hdr) + 1)
		return 0;

	eap = (struct eap_hdr *) buffer;
	eap->code = EAP_CODE_REQUEST;
	eap->identifier = 0;
	eap->length = htons(5);
	pos = (u8 *) (eap + 1);
	*pos = EAP_TYPE_IDENTITY;

	return sizeof(struct eap_hdr) + 1;

}

CSCM_EXPORT void ieee802_1x_EAP_packet_received (int bFromRADIUS, u8 *eap, size_t len)
{

	struct eapol_test_data *e = p_eapol_test;
	struct eap_hdr *hdr;
	int eap_type = -1;
	char buf[64];
	
	if (len < sizeof(*hdr)) {
		wpa_printf(MSG_DEBUG, "too short EAP packet "
			       "received from authentication server");
		free(eap);
		return;
	}

	if (len > sizeof(*hdr))
		eap_type = eap[sizeof(*hdr)];

	hdr = (struct eap_hdr *) eap;
	switch (hdr->code) {
	case EAP_CODE_REQUEST:
		snprintf(buf, sizeof(buf), "EAP-Request-%s (%d)",
			 eap_type >= 0 ? eap_type_text(eap_type) : "??",
			 eap_type);
		break;
	case EAP_CODE_RESPONSE:
		snprintf(buf, sizeof(buf), "EAP Response-%s (%d)",
			 eap_type >= 0 ? eap_type_text(eap_type) : "??",
			 eap_type);
		break;
	case EAP_CODE_SUCCESS:
		snprintf(buf, sizeof(buf), "EAP Success");
		/* LEAP uses EAP Success within an authentication, so must not
		 * stop here with eloop_terminate(); */
		break;
	case EAP_CODE_FAILURE:
		snprintf(buf, sizeof(buf), "EAP Failure");
		eloop_terminate();
		break;
	default:
		snprintf(buf, sizeof(buf), "unknown EAP code");
		wpa_hexdump(MSG_DEBUG, "EAP packet", eap, len);
		break;
	}
	
	wpa_printf(MSG_DEBUG, "EAP packet (code=%d "
	       "id=%d len=%d) from %s server: %s",
	      hdr->code, hdr->identifier, ntohs(hdr->length), bFromRADIUS ? "RADIUS" : "non-RADIUS", buf);
	
	if (bFromRADIUS)
		{

		/* sta->eapol_sm->be_auth.idFromServer = hdr->identifier; */

		free(e->last_eap_radius);
		e->last_eap_radius = eap;
		e->last_eap_radius_len = len;
		}

	{
		struct ieee802_1x_hdr *hdr;
		hdr = malloc(sizeof(*hdr) + len);
		assert(hdr != NULL);
		hdr->version = EAPOL_VERSION;
		hdr->type = IEEE802_1X_TYPE_EAP_PACKET;
		hdr->length = htons(len);
		memcpy((u8 *) (hdr + 1), eap, len);
		eapol_sm_rx_eapol(e->wpa_s->eapol, e->wpa_s->bssid,
				  (u8 *) hdr, sizeof(*hdr) + len);
		free(hdr);
	}
}

CSCM_EXPORT int eap_supplicant_open(
	char *wpa_supplicant_conf,
	u32 eloop_polling_interval_ms,
	u32 authentication_timeout_sec,
	struct eapol_debug_print_control *opt,
	void *hBeceemAPI,
	PFN_EAPSUP_CALLBACK eapsup_callback,
	
	const char *beceem_so_path,
	PFN_RSA_PRIV_ENC_CALLBACK rsa_priv_enc_callback)
{

	// Point to the first "line" in the array, not to a file name
	char *conf = wpa_supplicant_conf; 
	int hostapd_logger_level;
	time_t rawtime;
	struct tm * timeinfo;
	char buffer[80];
	
	// CMP SHA256 signature fix - 29 Feb 2008
	OpenSSL_add_all_digests();
	// End CMP fix

	ieee802_1x_rx_is_ready = 0;

	if(opt->wpa_debug_syslog)
		wpa_debug_open_syslog();
	else
		wpa_debug_close_syslog();

	g_fpWpaPrintf = NULL;
	if (opt->debug_filename != NULL &&
	    (opt->wpa_debug_level <= 4 || opt->hostapd_logger_level <=4))
		{	
		if ((debug_print_file = fopen(opt->debug_filename, "at")) == NULL)
		{
			wpa_printf(MSG_ERROR, "ERROR: Could not open debug print file %s for writing\n",
				opt->debug_filename);
			return -1;
		}
		g_fpWpaPrintf = debug_print_file;
		time (&rawtime);
		timeinfo = localtime(&rawtime);
		wpa_printf(MSG_INFO, "========== ========== ========= ========= ==========");
		strftime(buffer, 80, "Authentication session started at %x %X", timeinfo);
		wpa_printf(MSG_INFO, buffer);
		wpa_printf(MSG_INFO, "========== ========== ========= ========= ==========");

		wpa_debug_level      = opt->wpa_debug_level;
		wpa_debug_show_keys  = opt->wpa_debug_show_keys;
		hostapd_logger_level = opt->hostapd_logger_level;
		}
	else
		{
		debug_print_file     = NULL;
		g_fpWpaPrintf  		 = debug_print_file;
		wpa_debug_level      = 5;
		wpa_debug_show_keys  = 0;
		hostapd_logger_level = 5;
		}
		
	wpa_printf(MSG_DEBUG, "Entering eap_supplicant_open\n");

	// Beceem engine configuration
	wpa_printf(MSG_DEBUG, "hBeceemAPI            = %p", hBeceemAPI);
	wpa_printf(MSG_DEBUG, "beceem_so_path        = %s", beceem_so_path);	
	wpa_printf(MSG_DEBUG, "rsa_priv_enc_callback = %p", rsa_priv_enc_callback);

#ifdef CONFIG_NATIVE_WINDOWS
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 0), &wsaData)) {
		wpa_printf(MSG_ERROR, "Could not find a usable WinSock.dll\n");
		return -1;
	}
#endif /* CONFIG_NATIVE_WINDOWS */

	pfnEAPSUPcallback = eapsup_callback;
	p_eapol_test = &eapol_test;
	p_wpa_s		 = &wpa_s;
	memset(&eapol_test, 0, sizeof(eapol_test));
	
	// Save the BeceemAPI handle, for use in callbacks
	p_eapol_test->hBeceemAPI = hBeceemAPI;

	eapol_test.authentication_timeout_sec = authentication_timeout_sec;
	eloop_init(eloop_polling_interval_ms, &wpa_s);

	memset(&wpa_s, 0, sizeof(wpa_s));
	eapol_test.wpa_s = &wpa_s;

	wpa_s.conf = wpa_config_read(conf);
	if (wpa_s.conf == NULL) {
		wpa_printf(MSG_ERROR, "Failed to parse configuration file '%s'.\n", conf);
		return -1;
	}
	if (wpa_s.conf->ssid == NULL) {
		wpa_printf(MSG_ERROR, "No networks defined.\n");
		return -1;
	}

	wpa_init_conf(&eapol_test, &wpa_s); 
#ifdef UNUSED_CODE
	if (as_addr != NULL && as_secret != NULL)
		eap_radius_init(&eapol_test, &wpa_s, as_addr, as_port, as_secret,
				radius_callback, hostapd_logger_level);
#endif

#ifdef UNUSED_CODE
	if (wpa_supplicant_ctrl_iface_init(&wpa_s)) {
		wpa_printf(MSG_ERROR, "Failed to initialize control interface '%s'.\n"
		       "You may have another eapol_test process already "
		       "running or the file was\n"
		       "left by an unclean termination of eapol_test in "
		       "which case you will need\n"
		       "to manually remove this file before starting "
		       "eapol_test again.\n",
		       wpa_s.conf->ctrl_interface);
		return -1;
	}

	if (wpa_supplicant_scard_init(&wpa_s, wpa_s.conf->ssid))
		return -1;
#endif

	wpa_printf(MSG_DEBUG, "Leaving eap_supplicant_open\n");
	
	return 0;

}

CSCM_EXPORT void eap_supplicant_identity (char *identity, char *password)
{
	
	int flen, slen;
	
	wpa_printf(MSG_DEBUG, "Entering eap_supplicant_identity\n");

	// We must re-use the already allocated string buffers, since
	// VC++ does not allow one DLL to free heap memory allocated
	// by another

	flen = wpa_s.conf->ssid->identity_len;
	slen = strlen(identity);
	slen = slen > flen ? flen : slen;
	strncpy((char *) wpa_s.conf->ssid->identity, identity, flen);
	wpa_s.conf->ssid->identity_len = slen;
		
	flen = wpa_s.conf->ssid->password_len;
	slen = strlen(password);
	slen = slen > flen ? flen : slen;
	strncpy((char *) wpa_s.conf->ssid->password, password, flen); 
	wpa_s.conf->ssid->password_len = slen;

	wpa_printf(MSG_DEBUG, ">>>> identity=%s",wpa_s.conf->ssid->identity);
	wpa_printf(MSG_DEBUG, ">>>> password=%s",wpa_s.conf->ssid->password);

	wpa_printf(MSG_DEBUG, "Leaving eap_supplicant_identify\n");
}

CSCM_EXPORT int is_ieee802_1x_rx_ready(void)
{
	return ieee802_1x_rx_is_ready;
}

CSCM_EXPORT int eap_supplicant_authenticate (
	u8 *MSK_buffer,
	size_t MSK_buffer_len,
	int wimax_mode,
	int generate_eap_request_identity)
{

	int ret = EAP_SUPPLICANT_STARTED;

	wpa_printf(MSG_DEBUG, "Entering eap_supplicant_authenticate\n");

	// Initialize EAPOL sm and other structures	
	if (test_eapol(&eapol_test, &wpa_s, wpa_s.conf->ssid))
		return EAP_SUPPLICANT_TEST_EAPOL_FAILED;

	wpa_printf(MSG_DEBUG, "After test_eapol\n");
#ifdef UNUSED_CODE
	if (wait_for_monitor)
		wpa_supplicant_ctrl_iface_wait(&wpa_s);
#endif

	p_eapol_test->radius_access_accept_received = 0;
	
	// Set up for authentication
	eloop_clear_terminate();
	eloop_register_timeout(eapol_test.authentication_timeout_sec, 0, 
		eapol_test_timeout, &eapol_test, NULL);
	if (generate_eap_request_identity)
		eloop_register_timeout(0, 0, send_eap_request_identity, &wpa_s, NULL);
	eloop_register_signal(SIGINT, eapol_test_terminate, NULL);
	eloop_register_signal(SIGTERM, eapol_test_terminate, NULL);
#ifndef CONFIG_NATIVE_WINDOWS
	eloop_register_signal(SIGHUP, eapol_test_terminate, NULL);
#endif /* CONFIG_NATIVE_WINDOWS */

	wpa_printf(MSG_DEBUG, "After eloop setup calls\n");

	ieee802_1x_rx_is_ready = 1;
	eloop_run(wimax_mode);
	ieee802_1x_rx_is_ready = 0;

	wpa_printf(MSG_DEBUG, "After eloop_run\n");

	memset(MSK_buffer, 0, MSK_buffer_len);
	if (eapol_test.auth_timed_out)
		ret = EAP_SUPPLICANT_TIMED_OUT;
	else if (eapol_test.radius_access_reject_received)
		ret = EAP_SUPPLICANT_RADIUS_ACCESS_REJECT_RECEIVED;
	else if (wimax_mode) { 
		ret = eapol_sm_get_key(eapol_test.wpa_s->eapol, MSK_buffer, MSK_buffer_len);
		wpa_printf(MSG_DEBUG, "eapol_sm_get_key returned %d\n", ret);
		if (ret)
			ret = EAP_SUPPLICANT_SM_GET_KEY_FAILED;
		else
			ret = EAP_SUPPLICANT_SUCCESS;
		}
	else {
		if (eapol_test_compare_pmk(&eapol_test) == 0 &&
			eapol_sm_get_key(eapol_test.wpa_s->eapol, MSK_buffer, MSK_buffer_len) == 0) 
			ret = EAP_SUPPLICANT_SUCCESS;
		else
			ret = EAP_SUPPLICANT_SM_GET_KEY_FAILED;
		}
	
	if (ret == EAP_SUPPLICANT_SUCCESS)
		{	
		wpa_printf(MSG_DEBUG, "MSK before reordering");
		wpa_hexdump(MSG_DEBUG, "MSK[ 0-15] = ", MSK_buffer +  0, 16);
		wpa_hexdump(MSG_DEBUG, "MSK[16-31] = ", MSK_buffer + 16, 16);
		wpa_hexdump(MSG_DEBUG, "MSK[31-47] = ", MSK_buffer + 32, 16);
		wpa_hexdump(MSG_DEBUG, "MSK[48-63] = ", MSK_buffer + 48, 16);
		wpa_printf(MSG_DEBUG, "MSK len = %d\n", MSK_buffer_len);
		}

	if (!wimax_mode) {
		wpa_printf(MSG_ERROR, "MPPE keys OK: %d  mismatch: %d\n",
			   eapol_test.num_mppe_ok, eapol_test.num_mppe_mismatch);
		if (ret == EAP_SUPPLICANT_SUCCESS && eapol_test.num_mppe_mismatch)
			ret = EAP_SUPPLICANT_NUM_MPPE_MISMATCH;
	}

	if (ret)
		wpa_printf(MSG_ERROR, "FAILURE (ret code = %d)\n", ret);
	else
		wpa_printf(MSG_ERROR, "SUCCESS\n");

	return ret;

}

CSCM_EXPORT void eap_supplicant_close (void)
{
	
	wpa_printf(MSG_DEBUG, "Entering eap_supplicant_close\n");

	eloop_wait_terminate(); // force termination & wait for event loop to end

	wpa_printf(MSG_DEBUG, ">>>> entering test_eapol_clean\n");
	test_eapol_clean(&eapol_test, &wpa_s);

	wpa_printf(MSG_DEBUG, ">>>> calling eloop_destroy\n");	
	eloop_destroy();

#ifdef CONFIG_NATIVE_WINDOWS
	WSACleanup();
#endif /* CONFIG_NATIVE_WINDOWS */

	wpa_printf(MSG_DEBUG, "Leaving eap_supplicant_close\n");

	if (debug_print_file)
		fclose(debug_print_file);
}
