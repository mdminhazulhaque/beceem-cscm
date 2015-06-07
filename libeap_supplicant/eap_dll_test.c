// eap_test.cpp : Defines the entry point for the console application.
//

#ifdef CONFIG_NATIVE_WINDOWS
#include <conio.h>
#endif

#include <stdio.h>

// various items extracted from common.h
//
// Note: The entire header file cannot be included because of 
// strong dependencies on various mingw-specific include files

#define EAP_DEBUG
#ifdef EAP_DEBUG
#define PRINT_DEBUG printf
#else
#define PRINT_DEBUG(...)
#endif

#include <stdint.h>
typedef uint8_t u8;
typedef uint32_t u32;

#ifndef TRUE
#define TRUE 1
#endif

#define ETH_ALEN 6
enum {
	HOSTAPD_LEVEL_DEBUG_VERBOSE, 
	HOSTAPD_LEVEL_DEBUG,
	HOSTAPD_LEVEL_INFO, 
	HOSTAPD_LEVEL_NOTICE, 
	HOSTAPD_LEVEL_WARNING};
enum { 
	MSG_MSGDUMP, 
    MSG_DEBUG, 
	MSG_INFO, 
	MSG_WARNING, 
	MSG_ERROR };
// end of common.h items

#include "config_ssid.h"
#ifdef CONFIG_NATIVE_WINDOWS
#include "winsock2.h"
#endif
#include "eap_supplicant.h"

// ================================================================
//
// EAP-TTLS Setup
//
#ifdef __cplusplus
#define EXTERN_C extern "C"
#endif

char *radius_server_ip_addr = "10.10.20.58";
int   radius_server_port    = 1812;
//char *radius_server_secret  = "RadiusLocalLan";
char *radius_server_secret  = "testing123";

//char *user_identity = "cpuig";
//char *user_password = "RadiusCarlos";
char *user_identity = "myuser@asb.com";
char *user_password = "mypass@wd";

char *wpa_supplicant_conf = 
	
	"eapol_version=1\n"
	"ap_scan=0\n"
	"fast_reauth=1\n"

	"network={\n"
	
		"ssid=\"AuthTest\"\n"
	//	"identity=\"cpuig\"\n"
	//	"password=\"RadiusCarlos\"\n"
		"identity=\"                              \"\n"
		"password=\"                              \"\n"
		"key_mgmt=IEEE8021X\n"
//		"ca_cert=\"/Software/MinGW/etc/certs/cacert.pem\"\n"
		"ca_cert=\"c:\\Beceem\\CSCM\\Certs\\cacert.pem\"\n"
		"priority=5\n"
		"eapol_flags=0\n"

		"# ---- EAP-TTLS -----\n"
		"eap=TTLS\n"
		"anonymous_identity=\"anonymous\"\n"
		"phase2=\"auth-MSCHAPV2\"\n"

	"}\n"

	"\0";  // MUST end with two NUL characters!	

// ================================================================

// --------------------------------------------------------------
// eap_from_radius_server
//
//    is called by the RADIUS server whenever it wishes to send
//    an EAP packet to the EAP supplicant.
//
static void eap_from_radius_server (void *context, const u8 *msg, size_t msg_len)
{
	printf(">>>> eap_from_radius_server called with msg_len = %d\n", msg_len);
	printf(">>>> ... calling ieee802_1x_EAP_packet_received\n");
	ieee802_1x_EAP_packet_received (TRUE, (u8*) msg, msg_len);

}

// --------------------------------------------------------------
// eap_from_eap_supplicant
//
//    is called by the EAP supplicant whenever it wishes to transmit 
//    an EAP packet to the base station or RADIUS server.
//
static void eap_from_eap_supplicant (void *context, 
	const u8 *msg, size_t msg_len)
{	

	printf(">>>> eap_from_eap_supplicant called with msg_len = %d\n", msg_len);
	printf(">>>> ... calling ieee802_1x_encapsulate_radius\n");
	ieee802_1x_encapsulate_radius((struct eapol_test_data *) context, (u8*) msg, msg_len);
}

static void decode_auth_retcode (int ret)
	{

	printf("eap_supplicant_authenticate returned: ");
	switch (ret)
		{
		case 0:
			printf("SUCCESS");
			break;
		case -2:
			printf("auth timed out");
			break;
		case -3:
			printf("RADIUS access reject received");
			break;
		case -4:
			printf("num mppe mismatch");
			break;
		default:
			printf("Unknown return code");
			break;
		}
	printf("\n");

}

int main (int argc, char * argv[]) {
	
	int ret;
	struct eapol_debug_print_control opt;
	int eloop_polling_interval_ms = 300;
	// int send_identity_request = TRUE;

	u8 MSKBuffer[WIMAX_MSK_LEN];

	opt.debug_filename = "eap_supplicant_debug.txt";
	// wpa_debug_level values are:
	//   MSG_MSGDUMP = 0
    //   MSG_DEBUG
    //	 MSG_INFO
    //	 MSG_WARNING
	//   MSG_ERROR 	 = 4
    //   >4 is OFF
	opt.wpa_debug_level      = MSG_INFO;
	opt.wpa_debug_level      = MSG_MSGDUMP;

	// wpa_debug_show_keys values are 0=no and 1=yes
	opt.wpa_debug_show_keys  = 0;      
	opt.wpa_debug_show_keys  = 1;      

	// hostapd_logger_level values are:
	//  HOSTAPD_LEVEL_DEBUG_VERBOSE = 0 (includes message dumps)
	//  HOSTAPD_LEVEL_DEBUG
	//  HOSTAPD_LEVEL_INFO
	//  HOSTAPD_LEVEL_NOTICE
	//  HOSTAPD_LEVEL_WARNING		= 4
    //  >4 is OFF
 	opt.hostapd_logger_level = HOSTAPD_LEVEL_INFO;  
 	opt.hostapd_logger_level = HOSTAPD_LEVEL_DEBUG_VERBOSE;  

	PRINT_DEBUG("Calling eap_supplicant_open\n");

	ret = eap_supplicant_open(
							wpa_supplicant_conf,
							eloop_polling_interval_ms,
							10,
							&opt,
							NULL,
							eap_from_eap_supplicant,
							NULL, NULL);
	if (ret)
		return ret;

	PRINT_DEBUG("Calling eap_radius_init\n");
	if (radius_server_ip_addr != NULL && radius_server_secret != NULL)
		eap_radius_init(
			radius_server_ip_addr, radius_server_port, radius_server_secret,
			eap_from_radius_server, opt.hostapd_logger_level);

	PRINT_DEBUG("Calling eap_supplicant_identity\n");
	eap_supplicant_identity(user_identity, user_password);

	PRINT_DEBUG("Calling eap_supplicant_authenticate\n");
	ret = eap_supplicant_authenticate(MSKBuffer, WIMAX_MSK_LEN, 0, 1);
	decode_auth_retcode(ret);

	PRINT_DEBUG("Calling eap_supplicant_authenticate\n");
	ret = eap_supplicant_authenticate(MSKBuffer, WIMAX_MSK_LEN, 0, 1);
	decode_auth_retcode(ret);

	PRINT_DEBUG("Calling eap_supplicant_authenticate\n");
	ret = eap_supplicant_authenticate(MSKBuffer, WIMAX_MSK_LEN, 0, 1);
	decode_auth_retcode(ret);

	PRINT_DEBUG("Calling eap_supplicant_authenticate\n");
	ret = eap_supplicant_authenticate(MSKBuffer, WIMAX_MSK_LEN, 0, 1);
	decode_auth_retcode(ret);

	PRINT_DEBUG("Calling eap_supplicant_authenticate\n");
	ret = eap_supplicant_authenticate(MSKBuffer, WIMAX_MSK_LEN, 0, 1);
	decode_auth_retcode(ret);

	PRINT_DEBUG("Calling eap_radius_close and eap_supplicant_close\n");

	eap_radius_close();
	eap_supplicant_close();
	
//	printf("Press any key to continue ...");
//	_getche();
	
	return ret;

}

