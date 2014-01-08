# This makefile can build a statically linked version of Pin.  This may be
# useful because it has a smaller memory footprint since it does not load
# shared libraries.  However, we do not recommend building Pin statically if
# you plan to redistribute it to others because a statically linked Pin is
# not as portable for the following reasons:
#
#   * Pin uses dlopen() to load the tool.  When dlopen() runs, it requires
#     access to the version of libc.so that matches the static libc that is
#     built into Pin.
#
#   * When Pin is linked statically, it hard-codes a version of libc into the
#     Pin binary.  This can be a problem if you link Pin on a system without
#     nptl threads because that version of libc will prevent Pin from analyzing
#     applications that use nptl threads.

include ../../source/tools/makefile.gnu.config
PIN_KIT = ../..
PIN_HOME = ../../source/tools

ifeq (${TARGET},ia32)
  PIN_VM_LIBS = -lpinvm -lxed -lstdc++ -ldl
  LINK_ADDRESS =  -Wl,--section-start,.interp=0x1048420
endif

ifeq (${TARGET},ia32e)
  PIN_VM_LIBS = -lpinvm -lxed -lstdc++ -ldl
  LINK_ADDRESS =  -Wl,--section-start,.interp=0x30401024
endif

ifeq (${TARGET},ipf)
  PIN_VM_LIBS = -lpinvm -lstdc++ -ldl
  LINK_ADDRESS =  -Wl,--section-start,.interp=0x0000080000002000,--section-start,.preinit_array=0x00000a0000000000
endif

test:

pin.static:
	$(CXX) $(APP_CXXFLAGS) -static $(LINK_ADDRESS) -o pin.static $(PIN_LPATHS) $(PIN_VM_LIBS)

cp-pin.exe:
	$(CXX) $(PIN_HOME)/Tests/cp-pin.cpp $(APP_CXXFLAGS) -o cp-pin.exe

pinstatic.test: cp-pin.exe pin.static pinstatic.failed pinstatic.tested
	./pin.static -- ./cp-pin.exe makefile makefile.pinstatic
	rm pinstatic.failed makefile.pinstatic


clean:
	rm -f pin.static cp-pin.exe *.tested *.failed
