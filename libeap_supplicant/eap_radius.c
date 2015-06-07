#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#ifndef CONFIG_NATIVE_WINDOWS
#include <arpa/inet.h>
#endif

#include "common.h"
#include "config_ssid.h"
#include "eap_defs.h"
#include "eapol_sm.h"
#include "eloop.h"
#include "l2_packet.h"
#include "radius.h"
#include "radius_client.h"
#include "wpa.h"
#include "wpa_supplicant_i.h"
#include "eap_supplicant.h"
#include "hostapd.h"

static RadiusRxResult
ieee802_1x_receive_auth(struct radius_msg *msg, struct radius_msg *req,
			u8 *shared_secret, size_t shared_secret_len,
			void *data);

extern struct eapol_test_data *p_eapol_test;
extern struct wpa_supplicant *p_wpa_s;

extern int hostapd_logger_level;
PFN_RADIUS_CALLBACK pfnRADIUScallback;

CSCM_EXPORT 
void eap_radius_close(void)
{

	struct eapol_test_data *e = p_eapol_test;

	radius_client_deinit(e->radius);
	free(e->last_eap_radius);
	if (e->last_recv_radius) {
		radius_msg_free(e->last_recv_radius);
		free(e->last_recv_radius);
	}
}

CSCM_EXPORT
void eap_radius_init (
			const char *authsrv,
			int port, 
			const char *secret,
			PFN_RADIUS_CALLBACK radius_callback,
			int hostapd_logger_level_arg)
{

	struct eapol_test_data *e = p_eapol_test;
	struct wpa_supplicant *wpa_s = p_wpa_s;
	struct hostapd_radius_server *as;
	int res;

	wpa_printf(MSG_DEBUG,"Entering eap_radius_init - e=%8p", e);

	pfnRADIUScallback = radius_callback;
	hostapd_logger_level = hostapd_logger_level_arg;
	e->radius_conf = malloc(sizeof(struct hostapd_radius_servers));
	assert(e->radius_conf != NULL);
	memset(e->radius_conf, 0, sizeof(struct hostapd_radius_servers));
	e->radius_conf->num_auth_servers = 1;
	as = malloc(sizeof(struct hostapd_radius_server));
	assert(as != NULL);
	memset(as, 0, sizeof(*as));

	wpa_printf(MSG_DEBUG, "Before authsrv IP address scan");

#ifdef CONFIG_NATIVE_WINDOWS
	{
		int a[4];
		u8 *pos;
		sscanf(authsrv, "%d.%d.%d.%d", &a[0], &a[1], &a[2], &a[3]);
		pos = (u8 *) &as->addr.u.v4;
		*pos++ = a[0];
		*pos++ = a[1];
		*pos++ = a[2];
		*pos++ = a[3];
	}
#else /* CONFIG_NATIVE_WINDOWS */
	inet_aton(authsrv, &as->addr.u.v4);
#endif /* CONFIG_NATIVE_WINDOWS */
	as->addr.af = AF_INET;
	as->port = port;
	as->shared_secret = (u8 *) strdup(secret);
	as->shared_secret_len = strlen(secret);
	e->radius_conf->auth_server = as;
	e->radius_conf->auth_servers = as;
	e->radius_conf->msg_dumps = hostapd_logger_level <= HOSTAPD_LEVEL_DEBUG_VERBOSE;

	e->radius = radius_client_init(wpa_s, e->radius_conf);
	assert(e->radius != NULL);

	wpa_printf(MSG_DEBUG, "Calling radius_client_register");

	res = radius_client_register(e->radius, RADIUS_AUTH,
				     ieee802_1x_receive_auth, e);
	assert(res == 0);

	wpa_printf(MSG_DEBUG, "Leaving eap_radius_init");

} // eap_radius_init

const char * hostapd_ip_txt(const struct hostapd_ip_addr *addr, char *buf,
			    size_t buflen)
{
	if (buflen == 0 || addr == NULL)
		return NULL;

	if (addr->af == AF_INET) {
		snprintf(buf, buflen, "%s", inet_ntoa(addr->u.v4));
	} else {
		buf[0] = '\0';
	}
#ifdef CONFIG_IPV6
	if (addr->af == AF_INET6) {
		if (inet_ntop(AF_INET6, &addr->u.v6, buf, buflen) == NULL)
			buf[0] = '\0';
	}
#endif /* CONFIG_IPV6 */

	wpa_printf(MSG_DEBUG, "Leaving eap_radius_init");

	return buf;
}

static void ieee802_1x_decapsulate_radius(struct eapol_test_data *e)
{
	u8 *eap;
	size_t len;
	struct radius_msg *msg;

	if (e->last_recv_radius == NULL)
		return;

	msg = e->last_recv_radius;

	eap = radius_msg_get_eap(msg, &len);
	if (eap == NULL) {
		/* draft-aboba-radius-rfc2869bis-20.txt, Chap. 2.6.3:
		 * RADIUS server SHOULD NOT send Access-Reject/no EAP-Message
		 * attribute */
		wpa_printf(MSG_DEBUG, "could not extract "
			       "EAP-Message from RADIUS message");
		free(e->last_eap_radius);
		e->last_eap_radius = NULL;
		e->last_eap_radius_len = 0;
		return;
	}

	pfnRADIUScallback (e, eap, len);
	
}

CSCM_EXPORT
void ieee802_1x_encapsulate_radius(struct eapol_test_data *e,
					  const u8 *eap, size_t len)
{
	struct radius_msg *msg;
	char buf[128];
	const struct eap_hdr *hdr;
	const u8 *pos;

	wpa_printf(MSG_DEBUG, "Encapsulating EAP message into a RADIUS "
		   "packet");

	e->radius_identifier = radius_client_get_id(e->radius);
	msg = radius_msg_new(RADIUS_CODE_ACCESS_REQUEST,
			     e->radius_identifier);
	if (msg == NULL) {
		printf("Could not create net RADIUS packet\n");
		return;
	}

	radius_msg_make_authenticator(msg, (u8 *) e, sizeof(*e));

	hdr = (const struct eap_hdr *) eap;
	pos = (const u8 *) (hdr + 1);
	if (len > sizeof(*hdr) && hdr->code == EAP_CODE_RESPONSE &&
	    pos[0] == EAP_TYPE_IDENTITY) {
		pos++;
		free(e->eap_identity);
		e->eap_identity_len = len - sizeof(*hdr) - 1;
		e->eap_identity = malloc(e->eap_identity_len);
		if (e->eap_identity) {
			memcpy(e->eap_identity, pos, e->eap_identity_len);
			wpa_hexdump(MSG_DEBUG, "Learned identity from "
				    "EAP-Response-Identity",
				    e->eap_identity, e->eap_identity_len);
		}
	}

	if (e->eap_identity &&
	    !radius_msg_add_attr(msg, RADIUS_ATTR_USER_NAME,
				 e->eap_identity, e->eap_identity_len)) {
		printf("Could not add User-Name\n");
		goto fail;
	}

	if (e->user_password && e->user_password_len > 0 &&
		!radius_msg_add_attr_user_password(msg,
					e->user_password, e->user_password_len,
					e->radius_conf->auth_server->shared_secret,
					e->radius_conf->auth_server->shared_secret_len)) {
		printf("Could not add Password\n");
		goto fail;
		
	}


	if (!radius_msg_add_attr(msg, RADIUS_ATTR_NAS_IP_ADDRESS,
				 (u8 *) e->pst_own_ip_addr, 4)) {
//				 (u8 *) &e->own_ip_addr, 4)) {
		printf("Could not add NAS-IP-Address\n");
		goto fail;
	}

	snprintf(buf, sizeof(buf), RADIUS_802_1X_ADDR_FORMAT,
		 MAC2STR(e->wpa_s->own_addr));
	if (!radius_msg_add_attr(msg, RADIUS_ATTR_CALLING_STATION_ID,
				 (u8 *) buf, strlen(buf))) {
		printf("Could not add Calling-Station-Id\n");
		goto fail;
	}

	/* TODO: should probably check MTU from driver config; 2304 is max for
	 * IEEE 802.11, but use 1400 to avoid problems with too large packets
	 */
	if (!radius_msg_add_attr_int32(msg, RADIUS_ATTR_FRAMED_MTU, 1400)) {
		printf("Could not add Framed-MTU\n");
		goto fail;
	}

	if (!radius_msg_add_attr_int32(msg, RADIUS_ATTR_NAS_PORT_TYPE,
				       RADIUS_NAS_PORT_TYPE_IEEE_802_11)) {
		printf("Could not add NAS-Port-Type\n");
		goto fail;
	}

	snprintf(buf, sizeof(buf), "CONNECT 11Mbps 802.11b");
	if (!radius_msg_add_attr(msg, RADIUS_ATTR_CONNECT_INFO,
				 (u8 *) buf, strlen(buf))) {
		printf("Could not add Connect-Info\n");
		goto fail;
	}

	if (eap && !radius_msg_add_eap(msg, eap, len)) {
		printf("Could not add EAP-Message\n");
		goto fail;
	}

	/* State attribute must be copied if and only if this packet is
	 * Access-Request reply to the previous Access-Challenge */
	if (e->last_recv_radius && e->last_recv_radius->hdr->code ==
	    RADIUS_CODE_ACCESS_CHALLENGE) {
		int res = radius_msg_copy_attr(msg, e->last_recv_radius,
					       RADIUS_ATTR_STATE);
		if (res < 0) {
			printf("Could not copy State attribute from previous "
			       "Access-Challenge\n");
			goto fail;
		}
		if (res > 0) {
			wpa_printf(MSG_DEBUG, "  Copied RADIUS State "
				   "Attribute");
		}
	}

	radius_client_send(e->radius, msg, RADIUS_AUTH, e->wpa_s->own_addr);
	return;

 fail:
	radius_msg_free(msg);
	free(msg);
}

void ieee802_1x_get_keys(struct eapol_test_data *e,
				struct radius_msg *msg, struct radius_msg *req,
				u8 *shared_secret, size_t shared_secret_len)
{
	struct radius_ms_mppe_keys *keys;

	keys = radius_msg_get_ms_keys(msg, req, shared_secret,
				      shared_secret_len);
	if (keys && keys->send == NULL && keys->recv == NULL) {
		free(keys);
		keys = radius_msg_get_cisco_keys(msg, req, shared_secret,
						 shared_secret_len);
	}

	if (keys) {
		if (keys->send) {
			wpa_hexdump(MSG_DEBUG, "MS-MPPE-Send-Key (sign)",
				    keys->send, keys->send_len);
		}
		if (keys->recv) {
			wpa_hexdump(MSG_DEBUG, "MS-MPPE-Recv-Key (crypt)",
				    keys->recv, keys->recv_len);
			e->authenticator_pmk_len =
				keys->recv_len > PMK_LEN ? PMK_LEN :
				keys->recv_len;
			memcpy(e->authenticator_pmk, keys->recv,
			       e->authenticator_pmk_len);
		}

		free(keys->send);
		free(keys->recv);
		free(keys);
	}
}

/* Process the RADIUS frames from Authentication Server */
RadiusRxResult
ieee802_1x_receive_auth(struct radius_msg *msg, struct radius_msg *req,
			u8 *shared_secret, size_t shared_secret_len,
			void *data)
{
	struct eapol_test_data *e = data;

	/* RFC 2869, Ch. 5.13: valid Message-Authenticator attribute MUST be
	 * present when packet contains an EAP-Message attribute */
	if (msg->hdr->code == RADIUS_CODE_ACCESS_REJECT &&
	    radius_msg_get_attr(msg, RADIUS_ATTR_MESSAGE_AUTHENTICATOR, NULL,
				0) < 0 &&
	    radius_msg_get_attr(msg, RADIUS_ATTR_EAP_MESSAGE, NULL, 0) < 0) {
		wpa_printf(MSG_DEBUG, "Allowing RADIUS "
			      "Access-Reject without Message-Authenticator "
			      "since it does not include EAP-Message\n");
	} else if (radius_msg_verify(msg, shared_secret, shared_secret_len,
				     req, 1)) {
		printf("Incoming RADIUS packet did not have correct "
		       "Message-Authenticator - dropped\n");
		return RADIUS_RX_UNKNOWN;
	}

	if (msg->hdr->code != RADIUS_CODE_ACCESS_ACCEPT &&
	    msg->hdr->code != RADIUS_CODE_ACCESS_REJECT &&
	    msg->hdr->code != RADIUS_CODE_ACCESS_CHALLENGE) {
		printf("Unknown RADIUS message code\n");
		return RADIUS_RX_UNKNOWN;
	}

	e->radius_identifier = -1;
	wpa_printf(MSG_DEBUG, "RADIUS packet matching with station");

	if (e->last_recv_radius) {
		radius_msg_free(e->last_recv_radius);
		free(e->last_recv_radius);
	}

	e->last_recv_radius = msg;

	switch (msg->hdr->code) {
	case RADIUS_CODE_ACCESS_ACCEPT:
		e->radius_access_accept_received = 1;
		ieee802_1x_get_keys(e, msg, req, shared_secret,
				    shared_secret_len);
		break;
	case RADIUS_CODE_ACCESS_REJECT:
		e->radius_access_reject_received = 1;
		break;
	}

	ieee802_1x_decapsulate_radius(e);

	if ((msg->hdr->code == RADIUS_CODE_ACCESS_ACCEPT &&
	     e->eapol_test_num_reauths < 0) ||
	    msg->hdr->code == RADIUS_CODE_ACCESS_REJECT) {
		eloop_terminate();
	}

	return RADIUS_RX_QUEUED;
}

