/*

	Exports to CSCM from eap_supplicant.dll

*/

#define WIMAX_MSK_LEN       64

#define EAP_SUPPLICANT_SUCCESS 						  0
#define EAP_SUPPLICANT_STARTED 						 -1
#define EAP_SUPPLICANT_TEST_EAPOL_FAILED			 -2
#define EAP_SUPPLICANT_SM_GET_KEY_FAILED			 -3
#define EAP_SUPPLICANT_TIMED_OUT                     -4
#define EAP_SUPPLICANT_RADIUS_ACCESS_REJECT_RECEIVED -5
#define EAP_SUPPLICANT_NUM_MPPE_MISMATCH			 -6
#define EAP_SUPPLICANT_MALLOC_FAILED				 -10

typedef	void (*PFN_EAPSUP_CALLBACK) (void *context, const u8 *msg, size_t msg_len);
typedef int  (*PFN_RSA_PRIV_ENC_CALLBACK) (void *context, 
										   const u8 *msg_in, size_t msg_in_len, 
										   u8 *msg_out, size_t msg_out_max_len);
typedef	void (*PFN_RADIUS_CALLBACK) (void *context, const u8 *msg, size_t msg_len);

struct eapol_debug_print_control {

	char *debug_filename;
	int wpa_debug_level;
	int wpa_debug_show_keys;
	int hostapd_logger_level;
	int wpa_debug_syslog;
};

// Debuggging macros
#define PRINTLINE printf("Line: %d\n", __LINE__);

#define PMK_LEN 32

struct eapol_test_data {

	struct wpa_supplicant *wpa_s;

	int eapol_test_num_reauths;
	int no_mppe_keys;
	int num_mppe_ok, num_mppe_mismatch;

	u8 radius_identifier;
	struct radius_msg *last_recv_radius;
	struct radius_client_data *radius;
	struct hostapd_radius_servers *radius_conf;

	u8 *last_eap_radius; /* last received EAP Response from Authentication
			      * Server */
	size_t last_eap_radius_len;

	u8 authenticator_pmk[PMK_LEN];
	size_t authenticator_pmk_len;
	int radius_access_accept_received;
	int radius_access_reject_received;
	int auth_timed_out;
	int authentication_timeout_sec;

	u8 *eap_identity;
	size_t eap_identity_len;

	// Added CMP 16Aug2006
	// Used in eap_radius.c
	u8 *user_identity;
	size_t user_identity_len;
	u8 *user_password;
	size_t user_password_len;

	// The following is a workaround to incompatibilities between 
	// VC++ and the MinGW environment
	void *pst_own_ip_addr; // This is initilized by the EAP supplicant only

	// Added CMP 20Jun2008
	void *hBeceemAPI; // Beceem API handle, needed by Enqueue functions

};

#ifdef BECEEM_CSCM
#ifdef CONFIG_NATIVE_WINDOWS

#ifndef EXTERN_C
#ifdef __cplusplus
#define EXTERN_C extern "C"
#endif
#endif

#else
#define EXTERN_C
#endif
#endif

// Exports from EAP Supplicant DLL

EXTERN_C int eap_supplicant_open(
	char *wpa_supplicant_conf,
	u32 eloop_polling_interval_ms,
	u32 authentication_timeout_sec,
	struct eapol_debug_print_control *opt,
	void *hBeceemAPI,
	PFN_EAPSUP_CALLBACK eapsup_callback,
	const char *beceem_so_path,
	PFN_RSA_PRIV_ENC_CALLBACK rsa_priv_enc_callback);
 
EXTERN_C void eap_supplicant_identity (char *identity, char *password);

EXTERN_C int is_ieee802_1x_rx_ready(void);

EXTERN_C int eap_supplicant_authenticate (
	u8 *MSK_buffer, 
	size_t MSK_buffer_len, 
	int wimax_mode,
	int generate_eap_request_identity);

EXTERN_C void eap_supplicant_close (void);

EXTERN_C void ieee802_1x_EAP_packet_received (int bFromRADIUS, u8 *eap, size_t len);

EXTERN_C int build_EAP_request_identity_packet (u8 *buffer, size_t buffer_len);

EXTERN_C
void eap_radius_init (
			const char *authsrv,
			int port, 
			const char *secret,
			PFN_RADIUS_CALLBACK radius_callback,
			int hostapd_logger_level);
EXTERN_C
void eap_radius_close (void);

EXTERN_C
void ieee802_1x_encapsulate_radius(
			struct eapol_test_data *e,
			const u8 *eap, size_t len);


