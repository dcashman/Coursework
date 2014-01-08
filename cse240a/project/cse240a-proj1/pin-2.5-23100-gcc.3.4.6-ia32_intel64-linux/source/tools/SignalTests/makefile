##
## Tests to validate Pin's emulation of application signal handling
##


include ../makefile.gnu.config


#
# None of these tests run on Windows since signals are not supported there.
#

APPS_l        = simple nestmask syncasyncapp fpcheck maskcheck segv blockloop exithandler altstack errcheck fatal sigcont \
                sigenableapp sigprocmask_short sig64 fatalsync blockingsyscall futex
APPS_ipf_l    = faultcheck_ipf natcheck
APPS_ia32_l   = swizzleapp_ia32 xmmcheck_ia32 xmmfaultcheck_ia32 faultcheck_ia32 faultcheck_flags_ia32 movsbtest_ia32 \
                double hlt recursive setcontext sigaltstack sigsuspend sigusr2 dfcheck_ia32 fptags_ia32
APPS_ia32e_l  = swizzleapp_ia32e xmmcheck_ia32e xmmfaultcheck_ia32e faultcheck_ia32e movsbtest_ia32e dfcheck_ia32e

TOOLS_l       = syncasynctool swizzletool exittool interceptor logtool intercepttool sigflowcheck spilltool \
                fastsigchecktool sigenable interceptall faulttool 
TOOLS_ia32_l  = faultcheck_flags_tool1 faultcheck_flags_tool2 faultcheck_flags_tool3
TOOLS_ia32e_l = faultcheck_flags_tool1 faultcheck_flags_tool2 faultcheck_flags_tool3


TESTS_l       = nestmask altstack errcheck fatallog sigcont fastsigcheck sigenable interceptall sigprocmask_short sig64 \
                faulttool fatalsync blockingsyscall futex
ifneq ($(ENABLE_VS),1)
TESTS_l += maskcheck
endif

TESTS_ipf_l   = simpleasync simplesync fpcheck interceptsegv log intercept sigflowcheck1 sigflowcheck2 syncasync \
                faultcheck_ipf natcheck
TESTS_ia32_l  = simpleasync simplesync fpcheck interceptsegv swizzle_ia32 xmmcheck_ia32 xmmfaultcheck_ia32 xmmfaultcheckspill_ia32 log \
                intercept sigflowcheck1 sigflowcheck2 syncasync faultcheck_ia32 faultcheck_flags_ia32_1 faultcheck_flags_ia32_2 movsbtest_ia32 \
                hlt sigaltstack dfcheck_ia32 fptags_ia32
###                
# sigsuspend was causing a hang, so it is disabled until the problem can be investigated
# also disabled double, recursive, setcontext and sigusr2 
# because even though they passed sanity they caused failures in the nightlies
#                double hlt recursive setcontext sigaltstack sigsuspend sigusr2
###
TESTS_ia32e_l = simpleasync simplesync fpcheck interceptsegv swizzle_ia32e xmmcheck_ia32e xmmfaultcheck_ia32e log \
                intercept sigflowcheck1 sigflowcheck2 syncasync faultcheck_ia32e movsbtest_ia32e dfcheck_ia32e

apps  = $(APPS) $(APPS_$(TARGET)) $(APPS_$(TARGET_OS)) $(APPS_$(TARGET)_$(TARGET_OS))
tools = $(TOOLS) $(TOOLS_$(TARGET)) $(TOOLS_$(TARGET_OS)) $(TOOLS_$(TARGET)_$(TARGET_OS))
tests = $(TESTS) $(TESTS_$(TARGET)) $(TESTS_$(TARGET_OS)) $(TESTS_$(TARGET)_$(TARGET_OS)) dummy


all: $(apps:%=$(OBJDIR)%) $(tools:%=$(OBJDIR)%)
test: $(tests:=.test)
tests-sanity: test

$(apps:%=$(OBJDIR)%) $(tools:%=$(OBJDIR)%): $(OBJDIR)/make-directory

$(OBJDIR)/make-directory:
	mkdir -p $(OBJDIR)
	touch  $(OBJDIR)/make-directory
$(OBJDIR):
	mkdir -p $(OBJDIR)



#
# Rules to build the applications
#
$(OBJDIR)nestmask: nestmask.c
	$(CC) $(APP_CXXFLAGS) -o $@ $<

$(OBJDIR)segv: segv.c
	$(CC) $(APP_CXXFLAGS) -o $@ $<

$(OBJDIR)syncasyncapp: syncasyncapp.c
	$(CC) $(APP_CXXFLAGS) -o $@ $<

$(OBJDIR)fpcheck: fpcheck.c
	$(CC) $(APP_CXXFLAGS) -O -o $@ $< -lm

$(OBJDIR)swizzleapp_ia32: swizzleapp.c copywithxmm_ia32.s
	$(CC) $(APP_CXXFLAGS) -o $@ swizzleapp.c copywithxmm_ia32.s

$(OBJDIR)swizzleapp_ia32e: swizzleapp.c copywithxmm_ia32e.s
	$(CC) $(APP_CXXFLAGS) -o $@ swizzleapp.c copywithxmm_ia32e.s

$(OBJDIR)xmmcheck_ia32: xmmcheck.c copywithxmm_ia32.s
	$(CC) $(APP_CXXFLAGS) -o $@ xmmcheck.c copywithxmm_ia32.s

$(OBJDIR)xmmcheck_ia32e: xmmcheck.c copywithxmm_ia32e.s
	$(CC) $(APP_CXXFLAGS) -o $@ xmmcheck.c copywithxmm_ia32e.s

$(OBJDIR)xmmfaultcheck_ia32: xmmfaultcheck.c copywithxmm_ia32.s
	$(CC) $(APP_CXXFLAGS) -DTARGET_IA32 -o $@ xmmfaultcheck.c copywithxmm_ia32.s

$(OBJDIR)xmmfaultcheck_ia32e: xmmfaultcheck.c copywithxmm_ia32e.s
	$(CC) $(APP_CXXFLAGS) -DTARGET_IA32E -o $@ xmmfaultcheck.c copywithxmm_ia32e.s

$(OBJDIR)blockloop: blockloop.c
	$(CC) $(APP_CXXFLAGS) -o $@ $<

$(OBJDIR)maskcheck: maskcheck.c
	$(CC) $(APP_CXXFLAGS) -o $@ $<

$(OBJDIR)exithandler: exithandler.c
	$(CC) $(APP_CXXFLAGS) -o $@ $<

$(OBJDIR)simple: simple.c
	$(CC) $(APP_CXXFLAGS) -o $@ $<

$(OBJDIR)altstack: altstack.c
	$(CC) $(APP_CXXFLAGS) -o $@ $<

$(OBJDIR)faultcheck_ia32: faultcheck.c faultcheck_ia32.c faultcheck_ia32_asm.s
	$(CC) $(APP_CXXFLAGS) -o $@ faultcheck.c faultcheck_ia32.c faultcheck_ia32_asm.s

$(OBJDIR)faultcheck_ia32e: faultcheck.c faultcheck_ia32e.c faultcheck_ia32e_asm.s
	$(CC) $(APP_CXXFLAGS) -o $@ faultcheck.c faultcheck_ia32e.c faultcheck_ia32e_asm.s

$(OBJDIR)faultcheck_ipf: faultcheck.c faultcheck_ipf.c faultcheck_ipf_asm.s
	$(CC) $(APP_CXXFLAGS) -o $@ faultcheck.c faultcheck_ipf.c faultcheck_ipf_asm.s

$(OBJDIR)faultcheck_flags_ia32: faultcheck_flags.c  faultcheck_flags_ia32_asm.s
	$(CC) $(APP_CXXFLAGS) -o $@ faultcheck_flags.c  faultcheck_flags_ia32_asm.s
	
$(OBJDIR)errcheck: errcheck.c
	$(CC) $(APP_CXXFLAGS) -o $@ $<

$(OBJDIR)natcheck: natcheck.c natcheck_asm.s
	$(CC) $(APP_CXXFLAGS) -o $@ natcheck.c natcheck_asm.s

$(OBJDIR)movsbtest_ia32: movsbtest.c movsbtest_ia32.s
	$(CC) $(APP_CXXFLAGS) -DTARGET_IA32 -o $@ movsbtest.c movsbtest_ia32.s

$(OBJDIR)movsbtest_ia32e: movsbtest.c movsbtest_ia32e.s
	$(CC) $(APP_CXXFLAGS) -DTARGET_IA32E -o $@ movsbtest.c movsbtest_ia32e.s

$(OBJDIR)double: double.c
	$(CC) $(APP_CXXFLAGS) -O -o $@ $<

$(OBJDIR)hlt: hlt.c
	$(CC) $(APP_CXXFLAGS) -O -o $@ $<

$(OBJDIR)recursive: recursive.c
	$(CC) $(APP_CXXFLAGS) -g -O -o $@ $<

$(OBJDIR)setcontext: setcontext.c
	$(CC) $(APP_CXXFLAGS)  -g -O -o $@ $<

$(OBJDIR)sigaltstack: sigaltstack.c
	$(CC) $(APP_CXXFLAGS) -g -O -lpthread -o $@ $<

$(OBJDIR)sigsuspend: sigsuspend.c
	$(CC) $(APP_CXXFLAGS) -g -O -lpthread -o $@ $<

$(OBJDIR)sigusr2: sigusr2.c
	$(CC) $(APP_CXXFLAGS) -O -o $@ $<

$(OBJDIR)fatal: fatal.c
	$(CC) $(APP_CXXFLAGS) -O -o $@ $<

$(OBJDIR)sigcont: sigcont.c
	$(CC) $(APP_CXXFLAGS) -O -o $@ $<

$(OBJDIR)sigenableapp: sigenableapp.c sigenableapp2.c
	$(CC) $(APP_CXXFLAGS) -o $@ sigenableapp.c sigenableapp2.c

$(OBJDIR)sigprocmask_short: sigprocmask_short.cpp
	$(CXX) $(PIN_CXXFLAGS) -o $@ sigprocmask_short.cpp

$(OBJDIR)sig64: sig64.cpp
	$(CXX) $(APP_CXXFLAGS) -o $@ sig64.cpp

$(OBJDIR)dfcheck_ia32: dfcheck.c dfcheck_ia32.s
	$(CC) $(APP_CXXFLAGS) -o $@ dfcheck.c dfcheck_ia32.s

$(OBJDIR)dfcheck_ia32e: dfcheck.c dfcheck_ia32e.s
	$(CC) $(APP_CXXFLAGS) -o $@ dfcheck.c dfcheck_ia32e.s

$(OBJDIR)fatalsync: fatalsync.c
	$(CC) $(APP_CXXFLAGS) -o $@ fatalsync.c

$(OBJDIR)blockingsyscall: blockingsyscall.c
	$(CC) $(APP_CXXFLAGS) -o $@ blockingsyscall.c

$(OBJDIR)futex: futex.c
	$(CC) $(APP_CXXFLAGS) -o $@ futex.c -lpthread

$(OBJDIR)fptags_ia32: fptags.c fptags_ia32.s
	$(CC) $(APP_CXXFLAGS) -o $@ fptags.c fptags_ia32.s


#
# Rules to build the object files
#
$(OBJDIR)%.o: %.cpp
	$(CXX) $(COPT) $(CXXFLAGS) $(PIN_CXXFLAGS) -o $@ $<

#
# Rules to build the tools
#
$(OBJDIR)interceptor: $(OBJDIR)interceptor.o
	$(CXX) $(PIN_LDFLAGS) -o $@ $< $(PIN_LIBS)

$(OBJDIR)syncasynctool: $(OBJDIR)syncasynctool.o
	$(CXX) $(PIN_LDFLAGS) -o $@ $< $(PIN_LIBS)

$(OBJDIR)swizzletool: $(OBJDIR)swizzletool.o
	$(CXX) $(PIN_LDFLAGS) -o $@ $< $(PIN_LIBS)

$(OBJDIR)logtool: $(OBJDIR)logtool.o
	$(CXX) $(PIN_LDFLAGS) -o $@ $< $(PIN_LIBS)

$(OBJDIR)intercepttool: $(OBJDIR)intercepttool.o
	$(CXX) $(PIN_LDFLAGS) -o $@ $< $(PIN_LIBS)

$(OBJDIR)sigflowcheck: $(OBJDIR)sigflowcheck.o
	$(CXX) $(PIN_LDFLAGS) -o $@ $< $(PIN_LIBS)

$(OBJDIR)exittool: $(OBJDIR)exittool.o
	$(CXX) $(PIN_LDFLAGS) -o $@ $< $(PIN_LIBS)

$(OBJDIR)spilltool: $(OBJDIR)spilltool.o
	$(CXX) $(PIN_LDFLAGS) -o $@ $< $(PIN_LIBS)

$(OBJDIR)fastsigchecktool: $(OBJDIR)fastsigchecktool.o
	$(CXX) $(PIN_LDFLAGS) -o $@ $< $(PIN_LIBS)

$(OBJDIR)sigenable: $(OBJDIR)sigenable.o
	$(CXX) $(PIN_LDFLAGS) -o $@ $< $(PIN_LIBS)

$(OBJDIR)interceptall: $(OBJDIR)interceptall.o
	$(CXX) $(PIN_LDFLAGS) -o $@ $< $(PIN_LIBS)

$(OBJDIR)faulttool: $(OBJDIR)faulttool.o
	$(CXX) $(PIN_LDFLAGS) -o $@ $< $(PIN_LIBS)
	
$(OBJDIR)faultcheck_flags_tool1: $(OBJDIR)faultcheck_flags_tool1.o
	$(CXX) $(PIN_LDFLAGS) -o $@ $< $(PIN_LIBS)

$(OBJDIR)faultcheck_flags_tool2: $(OBJDIR)faultcheck_flags_tool2.o
	$(CXX) $(PIN_LDFLAGS) -o $@ $< $(PIN_LIBS)
	
$(OBJDIR)faultcheck_flags_tool3: $(OBJDIR)faultcheck_flags_tool3.o
	$(CXX) $(PIN_LDFLAGS) -o $@ $< $(PIN_LIBS)

#
# Rules to run the tests.
#
nestmask.test: $(OBJDIR)nestmask nestmask.tested nestmask.failed 
	$(PIN) -- ./$<
	rm $(@:.test=.failed)

interceptsegv.test: interceptsegv.tested interceptsegv.failed $(OBJDIR)interceptor $(OBJDIR)segv
	$(PIN) -t $(OBJDIR)interceptor -- ./$(OBJDIR)segv
	rm $(@:.test=.failed)

syncasync.test: syncasync.tested syncasync.failed $(OBJDIR)syncasynctool $(OBJDIR)syncasyncapp
	$(PIN) -t $(OBJDIR)syncasynctool -- ./$(OBJDIR)syncasyncapp
	rm $(@:.test=.failed)

fpcheck.test: $(OBJDIR)fpcheck fpcheck.tested fpcheck.failed 
	$(PIN) -- ./$<
	rm $(@:.test=.failed)

swizzle_ia32.test: swizzle_ia32.tested swizzle_ia32.failed $(OBJDIR)swizzleapp_ia32 $(OBJDIR)swizzletool
	$(PIN) -t $(OBJDIR)swizzletool -- ./$(OBJDIR)swizzleapp_ia32
	rm $(@:.test=.failed)

swizzle_ia32e.test: swizzle_ia32e.tested swizzle_ia32e.failed $(OBJDIR)swizzleapp_ia32e $(OBJDIR)swizzletool
	$(PIN) -t $(OBJDIR)swizzletool -- ./$(OBJDIR)swizzleapp_ia32e
	rm $(@:.test=.failed)

xmmcheck_ia32.test: $(OBJDIR)xmmcheck_ia32 xmmcheck_ia32.tested xmmcheck_ia32.failed 
	$(PIN) -- ./$<
	rm $(@:.test=.failed)

xmmcheck_ia32e.test: $(OBJDIR)xmmcheck_ia32e xmmcheck_ia32e.tested xmmcheck_ia32e.failed 
	$(PIN) -- ./$<
	rm $(@:.test=.failed)

xmmfaultcheck_ia32.test: xmmfaultcheck_ia32.tested xmmfaultcheck_ia32.failed $(OBJDIR)xmmfaultcheck_ia32
	$(PIN) -- ./$(OBJDIR)$(@:.test=)
	rm $(@:.test=.failed)

xmmfaultcheckspill_ia32.test: xmmfaultcheckspill_ia32.tested xmmfaultcheckspill_ia32.failed $(OBJDIR)xmmfaultcheck_ia32 $(OBJDIR)spilltool
	$(PIN) -t $(OBJDIR)spilltool -- ./$(OBJDIR)xmmfaultcheck_ia32
	rm $(@:.test=.failed)

xmmfaultcheck_ia32e.test: xmmfaultcheck_ia32e.tested xmmfaultcheck_ia32e.failed $(OBJDIR)xmmfaultcheck_ia32e
	$(PIN) -- ./$(OBJDIR)$(@:.test=)
	rm $(@:.test=.failed)

log.test: log.tested log.failed $(OBJDIR)logtool $(OBJDIR)nestmask
	$(PIN) -t $(OBJDIR)logtool -o log.out -- ./$(OBJDIR)nestmask
	$(PIN_CMP) log.out log.reference
	rm $(@:.test=.failed)

intercept.test: intercept.tested intercept.failed $(OBJDIR)intercepttool $(OBJDIR)blockloop
	rm -f intercept.out; touch intercept.out; \
        $(PIN) -pause_tool 1 -t $(OBJDIR)intercepttool -s 1 -c 2 -p -- ./$(OBJDIR)blockloop 2> intercept.tool.out | \
            tee intercept.out & \
        until grep 'Signals are blocked' intercept.out > /dev/null; \
            do sleep 5; \
            if ! kill -s 0 $$! > /dev/null 2>&1; then \
                echo "intercept.test: Aborted because Pin process terminated"; \
                exit 1; \
            fi; \
        done; \
        pid=`head -1 intercept.out | sed 's/.*to pid//'`; \
        kill -HUP $$pid; \
        until grep 'Intercepting signal' intercept.tool.out > /dev/null; \
            do sleep 1; \
            if ! kill -s 0 $$! > /dev/null 2>&1; then \
                echo "intercept.test: Aborted because Pin process terminated"; \
                exit 1; \
            fi; \
        done; \
        kill -HUP $$pid; \
        wait $$!
	$(PIN_CMP) intercept.tool.out intercept.tool.reference
	rm $(@:.test=.failed) intercept.tool.out intercept.out

maskcheck.test: maskcheck.tested maskcheck.failed $(OBJDIR)maskcheck
	./$(OBJDIR)$(@:.test=) > $(@:.test=.out)
	$(PIN) -- ./$(OBJDIR)$(@:.test=) > $(@:.test=.pin.out)
	$(PIN_CMP) $(@:.test=.out) $(@:.test=.pin.out)
	rm $(@:.test=.failed)

sigflowcheck1.test: sigflowcheck1.tested sigflowcheck1.failed $(OBJDIR)sigflowcheck $(OBJDIR)nestmask
	$(PIN) -t $(OBJDIR)sigflowcheck -- ./$(OBJDIR)nestmask > sigflowcheck1.out
	$(PIN_CMP) sigflowcheck1.out sigflowcheck1.reference
	rm $(@:.test=.failed)

sigflowcheck2.test: sigflowcheck2.tested sigflowcheck2.failed $(OBJDIR)sigflowcheck $(OBJDIR)exithandler
	$(PIN) -t $(OBJDIR)sigflowcheck -- ./$(OBJDIR)exithandler > sigflowcheck2.out
	$(PIN_CMP) sigflowcheck2.out sigflowcheck2.reference
	rm $(@:.test=.failed)

simpleasync.test: simpleasync.tested simpleasync.failed $(OBJDIR)simple $(OBJDIR)exittool
	$(PIN) -t $(OBJDIR)exittool -- ./$(OBJDIR)simple > simpleasync.out
	$(PIN_CMP) simpleasync.out simpleasync.reference
	rm $(@:.test=.failed)

simplesync.test: simplesync.tested simplesync.failed $(OBJDIR)segv $(OBJDIR)exittool
	$(PIN) -t $(OBJDIR)exittool -- ./$(OBJDIR)segv > simplesync.out
	$(PIN_CMP) simplesync.out simplesync.reference
	rm $(@:.test=.failed)

altstack.test: altstack.tested altstack.failed $(OBJDIR)altstack
	$(PIN) -- ./$(OBJDIR)altstack
	rm $(@:.test=.failed)

faultcheck_ia32.test: faultcheck_ia32.tested faultcheck_ia32.failed $(OBJDIR)faultcheck_ia32
	./$(OBJDIR)faultcheck_ia32 > $(@:.test=.out)
	$(PIN) -- ./$(OBJDIR)faultcheck_ia32 > $(@:.test=.pin.out)
	$(PIN_CMP) $(@:.test=.out) $(@:.test=.pin.out)
	rm $(@:.test=.failed)

faultcheck_ia32e.test: faultcheck_ia32e.tested faultcheck_ia32e.failed $(OBJDIR)faultcheck_ia32e
	./$(OBJDIR)faultcheck_ia32e > $(@:.test=.out)
	$(PIN) -- ./$(OBJDIR)faultcheck_ia32e > $(@:.test=.pin.out)
	$(PIN_CMP) $(@:.test=.out) $(@:.test=.pin.out)
	rm $(@:.test=.failed)

faultcheck_ipf.test: faultcheck_ipf.tested faultcheck_ipf.failed $(OBJDIR)faultcheck_ipf
	./$(OBJDIR)faultcheck_ipf > $(@:.test=.out)
	$(PIN) -- ./$(OBJDIR)faultcheck_ipf > $(@:.test=.pin.out)
	$(PIN_CMP) $(@:.test=.out) $(@:.test=.pin.out)
	rm $(@:.test=.failed)
	
faultcheck_flags_ia32_1.test: faultcheck_flags_ia32_1.tested faultcheck_flags_ia32_1.failed $(OBJDIR)faultcheck_flags_ia32 $(OBJDIR)faultcheck_flags_tool1
	./$(OBJDIR)faultcheck_flags_ia32 > $(@:.test=.out)
	$(PIN) -t ./$(OBJDIR)faultcheck_flags_tool1 -- ./$(OBJDIR)faultcheck_flags_ia32 > $(@:.test=.pin.out)
	$(PIN_CMP) $(@:.test=.out) $(@:.test=.pin.out)
	rm $(@:.test=.failed)
	
faultcheck_flags_ia32_2.test: faultcheck_flags_ia32_2.tested faultcheck_flags_ia32_2.failed $(OBJDIR)faultcheck_flags_ia32 $(OBJDIR)faultcheck_flags_tool2
	./$(OBJDIR)faultcheck_flags_ia32 > $(@:.test=.out)
	$(PIN) -t ./$(OBJDIR)faultcheck_flags_tool2 -- ./$(OBJDIR)faultcheck_flags_ia32 > $(@:.test=.pin.out)
	$(PIN_CMP) $(@:.test=.out) $(@:.test=.pin.out)
	rm $(@:.test=.failed)
	
faultcheck_flags_ia32_3.test: faultcheck_flags_ia32_3.tested faultcheck_flags_ia32_3.failed faultcheck_flags_ia32 $(OBJDIR)faultcheck_flags_ia32 $(OBJDIR)faultcheck_flags_tool3
	./$(OBJDIR)faultcheck_flags_ia32 > $(@:.test=.out)
	$(PIN) -t ./$(OBJDIR)faultcheck_flags_tool3 -- ./$(OBJDIR)faultcheck_flags_ia32 > $(@:.test=.pin.out)
	$(PIN_CMP) $(@:.test=.out) $(@:.test=.pin.out)
	rm $(@:.test=.failed)

errcheck.test: errcheck.tested errcheck.failed $(OBJDIR)errcheck
	$(PIN) -- ./$(OBJDIR)errcheck
	rm $(@:.test=.failed)

natcheck.test: natcheck.tested natcheck.failed $(OBJDIR)natcheck
	$(PIN) -- ./$(OBJDIR)natcheck
	rm $(@:.test=.failed)

movsbtest_ia32.test: movsbtest_ia32.tested movsbtest_ia32.failed $(OBJDIR)movsbtest_ia32
	$(PIN) -- ./$(OBJDIR)movsbtest_ia32
	rm $(@:.test=.failed)

movsbtest_ia32e.test: movsbtest_ia32e.tested movsbtest_ia32e.failed $(OBJDIR)movsbtest_ia32e
	$(PIN) -- ./$(OBJDIR)movsbtest_ia32e
	rm $(@:.test=.failed)

##################################################################################################
# double, hlt, recursive, setcontext, sigaltstack, sigsuspend, and sigusr2 are only for Linux-ia32
##################################################################################################
double.test: double.tested double.failed $(OBJDIR)double
	./$(OBJDIR)$(@:.test=) > $(@:.test=.out)
	$(PIN) -- ./$(OBJDIR)$(@:.test=) > $(@:.test=.pin.out)
	$(PIN_CMP) $(@:.test=.out) $(@:.test=.pin.out)
	rm $(@:.test=.failed)

hlt.test: hlt.tested hlt.failed $(OBJDIR)hlt
	./$(OBJDIR)$(@:.test=) > $(@:.test=.out)
	$(PIN) -- ./$(OBJDIR)$(@:.test=) > $(@:.test=.pin.out)
	$(PIN_CMP) $(@:.test=.out) $(@:.test=.pin.out)
	rm $(@:.test=.failed)

recursive.test: recursive.tested recursive.failed $(OBJDIR)recursive
	./$(OBJDIR)$(@:.test=) > $(@:.test=.out)
	$(PIN) -- ./$(OBJDIR)$(@:.test=) > $(@:.test=.pin.out)
	$(PIN_CMP) $(@:.test=.out) $(@:.test=.pin.out)
	rm $(@:.test=.failed)

setcontext.test: setcontext.tested setcontext.failed $(OBJDIR)setcontext
	./$(OBJDIR)$(@:.test=) > $(@:.test=.out)
	$(PIN) -- ./$(OBJDIR)$(@:.test=) > $(@:.test=.pin.out)
	$(PIN_CMP) $(@:.test=.out) $(@:.test=.pin.out)
	rm $(@:.test=.failed)

sigaltstack.test: sigaltstack.tested sigaltstack.failed $(OBJDIR)sigaltstack
	$(PIN) -- ./$(OBJDIR)$(@:.test=)
	rm $(@:.test=.failed)

sigsuspend.test: sigsuspend.tested sigsuspend.failed $(OBJDIR)sigsuspend
	$(PIN) -- ./$(OBJDIR)$(@:.test=)
	rm $(@:.test=.failed)

sigusr2.test: sigusr2.tested sigusr2.failed $(OBJDIR)sigusr2
	./$(OBJDIR)$(@:.test=) > $(@:.test=.out)
	$(PIN) -- ./$(OBJDIR)$(@:.test=) > $(@:.test=.pin.out)
	$(PIN_CMP) $(@:.test=.out) $(@:.test=.pin.out)
	rm $(@:.test=.failed)

fatallog.test: fatallog.tested fatallog.failed $(OBJDIR)fatal $(OBJDIR)logtool
	-$(PIN) -t $(OBJDIR)logtool -o fatallog.out -- ./$(OBJDIR)fatal; exit 0
	$(PIN_CMP) fatallog.out fatallog.reference
	rm $(@:.test=.failed)

sigcont.test: sigcont.tested sigcont.failed $(OBJDIR)sigcont
	$(PIN) -- ./$(OBJDIR)sigcont
	rm $(@:.test=.failed)

fastsigcheck.test: $(OBJDIR)fastsigchecktool $(OBJDIR)fpcheck fastsigcheck.tested fastsigcheck.failed
	$(PIN) -xyzzy -statistic -logfile fastsigcheck.log -t $(OBJDIR)fastsigchecktool -- ./$(OBJDIR)fpcheck
	#
	# This tests if the number of "slow" signals is less than 5%
	#
	pctSlow=`fgrep 'pin::signal::total/slow' fastsigcheck.log | sed -e 's/.*norm: *\([0-9\.]*\).*/\1/'`; \
	    test `expr $$pctSlow \< 0.05` -eq "1"
	rm $(@:.test=.failed)

sigenable.test: $(OBJDIR)sigenable $(OBJDIR)sigenableapp sigenable.tested sigenable.failed
	$(PIN) -t $(OBJDIR)sigenable -- ./$(OBJDIR)sigenableapp > $(@:.test=.out)
	$(PIN_CMP) $(@:.test=.out) $(@:.test=.reference)
	rm $(@:.test=.failed)

interceptall.test: $(OBJDIR)interceptall interceptall.tested interceptall.failed
	$(PIN) -t $(OBJDIR)interceptall -- $(TESTAPP) makefile interceptall.makefile.copy
	rm $(@:.test=.failed)

sigprocmask_short.test: $(OBJDIR)sigprocmask_short sigprocmask_short.tested sigprocmask_short.failed
	$(PIN) -- ./$(OBJDIR)sigprocmask_short
	rm $(@:.test=.failed)

sig64.test: $(OBJDIR)sig64 sig64.tested sig64.failed
	$(PIN) -- ./$(OBJDIR)sig64
	rm $(@:.test=.failed)

faulttool.test: $(OBJDIR)faulttool $(OBJDIR)simple faulttool.tested faulttool.failed
	@echo "The 'faulttool' test is expected to seg fault and dump core"
	-$(PIN) -t $(OBJDIR)faulttool -- ./$(OBJDIR)simple > $(@:.test=.out)
	grep "Tool (or Pin) caused signal" $(@:.test=.out) > /dev/null
	grep "Location may be in in-lined analysis code, try re-running with '-inline 0' to debug it" $(@:.test=.out) > /dev/null
	rm $(@:.test=.failed)

dfcheck_ia32.test: dfcheck_ia32.tested dfcheck_ia32.failed $(OBJDIR)dfcheck_ia32
	$(PIN) -- ./$(OBJDIR)dfcheck_ia32
	rm $(@:.test=.failed)

dfcheck_ia32e.test: dfcheck_ia32e.tested dfcheck_ia32e.failed $(OBJDIR)dfcheck_ia32e
	$(PIN) -- ./$(OBJDIR)dfcheck_ia32e
	rm $(@:.test=.failed)

fatalsync.test: fatalsync.tested fatalsync.failed $(OBJDIR)fatalsync
	@echo "The 'fatalsync' test is expected to seg fault and dump core"
	-$(PIN) -- ./$(OBJDIR)fatalsync > $(@:.test=.out)
	if grep "assertion failed" $(@:.test=.out); then exit 1; fi
	rm $(@:.test=.failed)

blockingsyscall.test: $(OBJDIR)blockingsyscall blockingsyscall.tested blockingsyscall.failed
	$(PIN) -- ./$(OBJDIR)blockingsyscall > $(@:.test=.out)
	$(PIN_CMP) $(@:.test=.out) $(@:.test=.reference)
	rm $(@:.test=.failed)

futex.test: $(OBJDIR)futex futex.tested futex.failed
	$(PIN) -- ./$(OBJDIR)futex
	rm $(@:.test=.failed)

fptags_ia32.test: $(OBJDIR)fptags_ia32 fptags_ia32.tested fptags_ia32.failed
	$(PIN) -- ./$(OBJDIR)fptags_ia32
	rm $(@:.test=.failed)

dummy.test:


clean:
	rm -rf $(OBJDIR) $(tests:=.tested) $(tests:=.failed) $(tests:=.out) \
        $(tests:=.tool.out) $(tests:=.pin.out) $(tests:=.log) *.makefile.copy pin.log core*
