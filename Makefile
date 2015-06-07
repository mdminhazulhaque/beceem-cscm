# Written by Muhammad Minhazul Haque <mdminhazulhaque@gmail.com>

.PHONY: all install clean
all:
	$(MAKE) -C libeap_supplicant
	$(MAKE) -C libengine_beceem
	$(MAKE) -C libxvi020
	$(MAKE) -C wimaxd
	$(MAKE) -C wimaxc

clean:
	$(MAKE) -C libeap_supplicant clean
	$(MAKE) -C libengine_beceem clean
	$(MAKE) -C libxvi020 clean
	$(MAKE) -C wimaxd clean
	$(MAKE) -C wimaxc clean
	rm -f build/*