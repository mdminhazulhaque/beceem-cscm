# ==========================================================
# 
# Beceem CSCM
#
# Copyright 2009 Beceem Communications, Inc.
#
# ==========================================================

#
# Makefile for compiling EAP supplicant library
#


ifeq ($(DEBUG),0)
CFLAGS += -O2
endif

ifeq ($(PLATFORM),mac)

CFLAGS += -Wall -g
CC = gcc
LEXT = dylib
LOPT = -single_module -dynamiclib -arch i386 -arch x86_64
CFLAGS += -arch i386 -arch x86_64
CFLAGS += -I/usr/local/ssl/include 
LIBS += -L/usr/local/ssl/lib
else
CFLAGS += -fPIC
LOPT += -fPIC
CFLAGS += -MMD -Wall -g
CFLAGS += -I.
LIBS += -L.
ifdef CROSS_TOOLCHAIN_DIR
CFLAGS += -I$(CROSS_TOOLCHAIN_DIR)/include -I$(CROSS_TOOLCHAIN_DIR)/usr/include
LIBS += -L$(CROSS_TOOLCHAIN_DIR)/lib -L$(CROSS_TOOLCHAIN_DIR)/usr/lib
endif
CC = $(CROSS_COMPILE)gcc
LEXT = so
LOPT = -shared

endif

ALL=cscm_eap_supplicant

all: verify_config $(ALL)

verify_config:
	@if [ ! -r .config ]; then \
		echo 'Building wpa_supplicant requires a configuration file'; \
		echo '(.config). See README for more instructions. You can'; \
		echo 'run "cp defconfig .config" to create an example'; \
		echo 'configuration.'; \
		exit 1; \
	fi

mkconfig:
	@if [ -e .config ]; then \
		echo '.config exists - did not replace it'; \
		exit 1; \
	fi
	echo CONFIG_DRIVER_HOSTAP=y >> .config
	echo CONFIG_DRIVER_WEXT=y >> .config
	echo CONFIG_WIRELESS_EXTENSION=y >> .config

install: all
	cp libeap_supplicant.$(LEXT) /lib

OBJS = config.o \
	eloop.o common.o md5.o \
	rc4.o sha1.o
OBJS_p = wpa_passphrase.o sha1.o md5.o
OBJS_c = wpa_cli.o wpa_ctrl.o

-include .config

ifdef CONFIG_DEBUG_SYSLOG
CFLAGS += -DCONFIG_DEBUG_SYSLOG
endif

ifdef CONFIG_BECEEM_CSCM
CFLAGS += -DBECEEM_CSCM
endif

ifdef CONFIG_EAPOL_TEST
# CFLAGS += -Werror -DEAPOL_TEST
CFLAGS += -Wall -DEAPOL_TEST
endif

ifndef CONFIG_BACKEND
CONFIG_BACKEND=file
endif

ifeq ($(CONFIG_BACKEND), file)
OBJS += config_file.o base64.o
CFLAGS += -DCONFIG_BACKEND_FILE
endif

ifdef CONFIG_DRIVER_HOSTAP
CFLAGS += -DCONFIG_DRIVER_HOSTAP
OBJS_d += driver_hostap.o
CONFIG_WIRELESS_EXTENSION=y
endif

ifdef CONFIG_DRIVER_WEXT
CFLAGS += -DCONFIG_DRIVER_WEXT
CONFIG_WIRELESS_EXTENSION=y
endif

ifdef CONFIG_DRIVER_PRISM54
CFLAGS += -DCONFIG_DRIVER_PRISM54
OBJS_d += driver_prism54.o
CONFIG_WIRELESS_EXTENSION=y
endif

ifdef CONFIG_DRIVER_HERMES
CFLAGS += -DCONFIG_DRIVER_HERMES
OBJS_d += driver_hermes.o
CONFIG_WIRELESS_EXTENSION=y
endif

ifdef CONFIG_DRIVER_MADWIFI
CFLAGS += -DCONFIG_DRIVER_MADWIFI
OBJS_d += driver_madwifi.o
CONFIG_WIRELESS_EXTENSION=y
endif

ifdef CONFIG_DRIVER_ATMEL
CFLAGS += -DCONFIG_DRIVER_ATMEL
OBJS_d += driver_atmel.o
CONFIG_WIRELESS_EXTENSION=y
endif

ifdef CONFIG_DRIVER_NDISWRAPPER
CFLAGS += -DCONFIG_DRIVER_NDISWRAPPER
OBJS_d += driver_ndiswrapper.o
CONFIG_WIRELESS_EXTENSION=y
endif

ifdef CONFIG_DRIVER_BROADCOM
CFLAGS += -DCONFIG_DRIVER_BROADCOM
OBJS_d += driver_broadcom.o
endif

ifdef CONFIG_DRIVER_IPW
CFLAGS += -DCONFIG_DRIVER_IPW
OBJS_d += driver_ipw.o
CONFIG_WIRELESS_EXTENSION=y
endif

ifdef CONFIG_DRIVER_BSD
CFLAGS += -DCONFIG_DRIVER_BSD
OBJS_d += driver_bsd.o
CONFIG_DNET_PCAP=y
CONFIG_L2_FREEBSD=y
endif

ifdef CONFIG_DRIVER_NDIS
CFLAGS += -DCONFIG_DRIVER_NDIS
OBJS_d += driver_ndis.o driver_ndis_.o
CONFIG_DNET_PCAP=y
CONFIG_WINPCAP=y
endif

ifdef CONFIG_DRIVER_WIRED
CFLAGS += -DCONFIG_DRIVER_WIRED
OBJS_d += driver_wired.o
endif

ifdef CONFIG_DRIVER_TEST
CFLAGS += -DCONFIG_DRIVER_TEST
OBJS_d += driver_test.o
endif

ifndef CONFIG_BECEEM_CSCM

ifdef CONFIG_DNET_PCAP
CFLAGS += -DUSE_DNET_PCAP
ifdef CONFIG_WINPCAP
OBJS += l2_packet_pcap.o
CFLAGS += -DCONFIG_WINPCAP
LIBS += -lwpcap -lpacket
LIBS_w += -lwpcap
else
ifdef CONFIG_L2_FREEBSD
OBJS += l2_packet_freebsd.o
LIBS += -lpcap
else
OBJS += l2_packet_pcap.o
LIBS += -ldnet -lpcap
endif
endif
else
OBJS += l2_packet_linux.o
endif

endif

ifdef CONFIG_EAP_TLS
# EAP-TLS
CFLAGS += -DEAP_TLS
OBJS += eap_tls.o
TLS_FUNCS=y
CONFIG_IEEE8021X_EAPOL=y
endif

ifdef CONFIG_EAP_PEAP
# EAP-PEAP
CFLAGS += -DEAP_PEAP
OBJS += eap_peap.o
TLS_FUNCS=y
CONFIG_EAP_MSCHAPV2=y
CONFIG_IEEE8021X_EAPOL=y
CONFIG_EAP_TLV=y
endif

ifdef CONFIG_EAP_TTLS
# EAP-TTLS
CFLAGS += -DEAP_TTLS
OBJS += eap_ttls.o
MS_FUNCS=y
TLS_FUNCS=y
CONFIG_EAP_MD5=y
CONFIG_IEEE8021X_EAPOL=y
endif

ifdef CONFIG_EAP_MD5
# EAP-MD5 (also used by EAP-TTLS)
CFLAGS += -DEAP_MD5
OBJS += eap_md5.o
CONFIG_IEEE8021X_EAPOL=y
endif

# backwards compatibility for old spelling
ifdef CONFIG_MSCHAPV2
CONFIG_EAP_MSCHAPV2=y
endif

ifdef CONFIG_EAP_MSCHAPV2
# EAP-MSCHAPv2 (also used by EAP-PEAP)
CFLAGS += -DEAP_MSCHAPv2
OBJS += eap_mschapv2.o
MS_FUNCS=y
CONFIG_IEEE8021X_EAPOL=y
endif

ifdef CONFIG_EAP_GTC
# EAP-GTC (also used by EAP-PEAP)
CFLAGS += -DEAP_GTC
OBJS += eap_gtc.o
CONFIG_IEEE8021X_EAPOL=y
endif

ifdef CONFIG_EAP_OTP
# EAP-OTP
CFLAGS += -DEAP_OTP
OBJS += eap_otp.o
CONFIG_IEEE8021X_EAPOL=y
endif

ifdef CONFIG_EAP_SIM
# EAP-SIM
CFLAGS += -DEAP_SIM
OBJS += eap_sim.o
CONFIG_IEEE8021X_EAPOL=y
CONFIG_EAP_SIM_COMMON=y
endif

ifdef CONFIG_EAP_LEAP
# EAP-LEAP
CFLAGS += -DEAP_LEAP
OBJS += eap_leap.o
MS_FUNCS=y
CONFIG_IEEE8021X_EAPOL=y
endif

ifdef CONFIG_EAP_PSK
# EAP-PSK
CFLAGS += -DEAP_PSK
OBJS += eap_psk.o eap_psk_common.o
CONFIG_IEEE8021X_EAPOL=y
NEED_AES=y
endif

ifdef CONFIG_EAP_AKA
# EAP-AKA
CFLAGS += -DEAP_AKA
OBJS += eap_aka.o
CONFIG_IEEE8021X_EAPOL=y
CONFIG_EAP_SIM_COMMON=y
endif

ifdef CONFIG_EAP_SIM_COMMON
OBJS += eap_sim_common.o
NEED_AES=y
endif

ifdef CONFIG_EAP_TLV
# EAP-TLV
CFLAGS += -DEAP_TLV
OBJS += eap_tlv.o
endif

ifdef CONFIG_EAP_FAST
# EAP-FAST
CFLAGS += -DEAP_FAST
OBJS += eap_fast.o
TLS_FUNCS=y
endif

ifdef CONFIG_EAP_PAX
# EAP-PAX
CFLAGS += -DEAP_PAX
OBJS += eap_pax.o eap_pax_common.o
CONFIG_IEEE8021X_EAPOL=y
endif

ifdef CONFIG_IEEE8021X_EAPOL
# IEEE 802.1X/EAPOL state machines (e.g., for RADIUS authentication)
CFLAGS += -DIEEE8021X_EAPOL
OBJS += eapol_sm.o eap.o
endif

ifdef CONFIG_PCSC
# PC/SC interface for smartcards (USIM, GSM SIM)
# CFLAGS += -DPCSC_FUNCS -I/usr/include/PCSC
CFLAGS += -DPCSC_FUNCS 
OBJS += pcsc_funcs.o
# -lpthread may not be needed depending on how pcsc-lite was configured
# LIBS += -lpcsclite -lpthread
LIBS += -lwinscard
endif

ifndef CONFIG_TLS
CONFIG_TLS=openssl
endif

ifdef TLS_FUNCS
# Shared TLS functions (needed for EAP_TLS, EAP_PEAP, EAP_TTLS, and EAP_FAST)
CFLAGS += -DEAP_TLS_FUNCS
OBJS += eap_tls_common.o
ifeq ($(CONFIG_TLS), openssl)
OBJS += tls_openssl.o
ifdef CONFIG_NATIVE_WINDOWS
LIBS += -lssleay32 -llibeay32
LIBS_p += -llibeay32
else
LIBS += -lssl -lcrypto
LIBS_p += -lcrypto
endif
endif
ifeq ($(CONFIG_TLS), gnutls)
OBJS += tls_gnutls.o
LIBS += -lgnutls -lgcrypt -lgpg-error
LIBS_p += -lgcrypt
endif
ifeq ($(CONFIG_TLS), schannel)
OBJS += tls_schannel.o
# Using OpenSSL for crypto at the moment; to be replaced
LIBS += -lcrypto
LIBS_p += -lcrypto
endif
ifdef CONFIG_SMARTCARD
ifndef CONFIG_NATIVE_WINDOWS
ifndef CONFIG_L2_FREEBSD
LIBS += -ldl
endif
endif
endif
NEED_CRYPTO=y
else
OBJS += tls_none.o
endif

ifdef CONFIG_PKCS12
CFLAGS += -DPKCS12_FUNCS
endif

ifdef CONFIG_SMARTCARD
CFLAGS += -DCONFIG_SMARTCARD
endif

ifdef MS_FUNCS
OBJS += ms_funcs.o
NEED_CRYPTO=y
endif

ifdef NEED_CRYPTO
ifndef TLS_FUNCS
ifeq ($(CONFIG_TLS), openssl)
LIBS += -lcrypto
LIBS_p += -lcrypto
endif
ifeq ($(CONFIG_TLS), gnutls)
LIBS += -lgcrypt
LIBS_p += -lgcrypt
endif
ifeq ($(CONFIG_TLS), schannel)
# Using OpenSSL for crypto at the moment; to be replaced
LIBS += -lcrypto
LIBS_p += -lcrypto
endif
endif
ifeq ($(CONFIG_TLS), openssl)
OBJS += crypto.o
OBJS_p += crypto.o
endif
ifeq ($(CONFIG_TLS), gnutls)
OBJS += crypto_gnutls.o
OBJS_p += crypto_gnutls.o
endif
ifeq ($(CONFIG_TLS), schannel)
# Using OpenSSL for crypto at the moment; to be replaced
OBJS += crypto.o
OBJS_p += crypto.o
endif
endif

ifdef CONFIG_WIRELESS_EXTENSION
CFLAGS += -DCONFIG_WIRELESS_EXTENSION
OBJS_d += driver_wext.o
endif

ifdef CONFIG_CTRL_IFACE
CFLAGS += -DCONFIG_CTRL_IFACE
OBJS += ctrl_iface.o
endif

ifdef CONFIG_READLINE
CFLAGS += -DCONFIG_READLINE
LIBS_c += -lncurses -lreadline
endif

ifdef CONFIG_NATIVE_WINDOWS
CFLAGS += -DCONFIG_NATIVE_WINDOWS -DCONFIG_CTRL_IFACE_UDP
LIBS += -lws2_32 -lgdi32 -lcrypt32
LIBS_c += -lws2_32
endif

ifdef CONFIG_NO_STDOUT_DEBUG
CFLAGS += -DCONFIG_NO_STDOUT_DEBUG
endif

ifdef CONFIG_IPV6
# for eapol_test only
CFLAGS += -DCONFIG_IPV6
endif

ifndef CONFIG_NO_WPA
OBJS += wpa.o preauth.o
NEED_AES=y
else
CFLAGS += -DCONFIG_NO_WPA
endif

ifdef NEED_AES
OBJS += aes_wrap.o
endif

OBJS += wpa_supplicant.o 
OBJS_t := $(OBJS) eapol_test.o radius.o radius_client.o
OBJS_t2 := $(OBJS) preauth_test.o
OBJS_eap := $(OBJS) eap_supplicant.o eap_radius.o radius_client.o radius.o
OBJS += main.o drivers.o $(OBJS_d)

cscm_eap_supplicant: .config $(OBJS_eap)
	$(CC) $(LOPT) -o ../build/libeap_supplicant.$(LEXT) \
		$(OBJS_eap) $(LIBS)
clean:
	rm -f core *~ *.o *.d *.a $(ALL) $(WINALL)

-include $(OBJS:%.o=%.d)
