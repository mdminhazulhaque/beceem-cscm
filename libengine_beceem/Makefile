# ==========================================================
# 
# Beceem CSCM
#
# Copyright 2009 Beceem Communications, Inc.
#
# ==========================================================

#
# Makefile for compiling Beceem engine
#


ifeq ($(DEBUG),0)
CFLAGS += -O2
endif

CFLAGS += -DENGINE_EXPORTS
CFLAGS += -DL_ENDIAN
CFLAGS += -DOPENSSL_CPUID_OBJ
CFLAGS += -DOPENSSL_IA32_SSE2
CFLAGS += -DAES_ASM
CFLAGS += -DBN_ASM
CFLAGS += -DOPENSSL_BN_ASM_PART_WORDS
CFLAGS += -DMD5_ASM
CFLAGS += -DSHA1_ASM
CFLAGS += -DRMD160_ASM
CFLAGS += -DOPENSSL_USE_APPLINK
CFLAGS += -DOPENSSL_NO_CAMELLIA
CFLAGS += -DOPENSSL_NO_SEED
CFLAGS += -DOPENSSL_NO_RC5
CFLAGS += -DOPENSSL_NO_MDC2
CFLAGS += -DOPENSSL_NO_TLSEXT
CFLAGS += -DOPENSSL_NO_KRB5
CFLAGS += -DOPENSSL_BUILD_SHLIBCRYPTO
CFLAGS += -DFLAT_INC
CFLAGS += -DENGINE_DYNAMIC_SUPPORT


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

ALL=engine_beceem

OBJS = e_beceem.o e_beceem_ciphers.o e_beceem_digests.o e_beceem_err.o e_beceem_rsa.o
LIBS += -lcrypto

engine_beceem: $(OBJS)
	$(CC) $(LOPT) -o ../build/libengine_beceem.$(LEXT) $(OBJS) $(LIBS)

clean:
	rm -f core *~ *.o *.d *.a *.so 

-include $(OBJS:%.o=%.d)

