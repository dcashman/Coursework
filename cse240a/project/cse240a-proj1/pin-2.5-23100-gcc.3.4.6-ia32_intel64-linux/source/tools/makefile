
#
# NB: this top level makefile is recursive
#     All directories it decends into MUST be
#     be exported as part of the pinkit
#

all: 
	-$(MAKE) -C Tests test-apps
	-$(MAKE) -C ToolUnitTests
	-$(MAKE) -C SignalTests
	-$(MAKE) -C Debugger
	-$(MAKE) -C Tests
	-$(MAKE) -C SimpleExamples
	-$(MAKE) -C ManualExamples
	-$(MAKE) -C Memory
	-$(MAKE) -C InstLibExamples
	-$(MAKE) -C DebugTrace
	-$(MAKE) -C SegTrace
	-$(MAKE) -C PinPoints
	-$(MAKE) -C CacheClient
	-$(MAKE) -C CodeCacheFootprint
	-$(MAKE) -C SuperPin
	-$(MAKE) -C Maid
	-$(MAKE) -C Probes
	-$(MAKE) -C AlignChk
	-$(MAKE) -C Mix
	-$(MAKE) -C Insmix
	-$(MAKE) -C SegmentsVirtualization
	-$(MAKE) -C I18N
	-$(MAKE) -C MemTrace
	-$(MAKE) -C ChildProcess
	-$(MAKE) -C CrossIa32Intel64

tests-sanity:
	-$(MAKE) -C Tests test-apps
	-$(MAKE) -C Tests tests-sanity
	-$(MAKE) -C ToolUnitTests tests-sanity
	-$(MAKE) -C SignalTests tests-sanity
	-$(MAKE) -C Debugger tests-sanity
	-$(MAKE) -C Probes tests-sanity
	-$(MAKE) -C SegTrace tests-sanity
	-$(MAKE) -C Mix tests-sanity
	-$(MAKE) -C SegmentsVirtualization tests-sanity
	-$(MAKE) -C I18N tests-sanity
	-$(MAKE) -C ChildProcess tests-sanity
	./testsummary


test: run-test	
	./testsummary

kit-test: run-test run-test-mypintool
	./testsummary

run-test: 
	-$(MAKE) -k -C Tests test-apps
	-$(MAKE) -k -C ToolUnitTests test
	-$(MAKE) -k -C SignalTests test
	-$(MAKE) -k -C Debugger test
	-$(MAKE) -k -C Tests test
	-$(MAKE) -k -C SimpleExamples test
	-$(MAKE) -k -C ManualExamples test
	-$(MAKE) -k -C Memory test
	-$(MAKE) -k -C InstLibExamples test
	-$(MAKE) -k -C DebugTrace test
	-$(MAKE) -k -C SegTrace test
	-$(MAKE) -k -C PinPoints test
	-$(MAKE) -k -C CacheClient test
	-$(MAKE) -k -C CodeCacheFootprint test
	-$(MAKE) -k -C Maid test
	-$(MAKE) -k -C Probes test
	-$(MAKE) -k -C AlignChk test
	-$(MAKE) -k -C Mix  test
	-$(MAKE) -k -C Insmix test
	-$(MAKE) -k -C SegmentsVirtualization test
	-$(MAKE) -k -C I18N test
	-$(MAKE) -k -C MemTrace test
	-$(MAKE) -k -C ChildProcess test
	-$(MAKE) -k -C CrossIa32Intel64 test


run-test-mypintool:
	-$(MAKE) -C MyPinTool test

clean:
	-$(MAKE) -C ToolUnitTests clean
	-$(MAKE) -C SignalTests clean
	-$(MAKE) -C Debugger clean
	-$(MAKE) -C Tests clean
	-$(MAKE) -C SimpleExamples clean
	-$(MAKE) -C ManualExamples clean
	-$(MAKE) -C Memory clean
	-$(MAKE) -C InstLibExamples clean
	-$(MAKE) -C DebugTrace clean
	-$(MAKE) -C SegTrace clean
	-$(MAKE) -C PinPoints clean
	-$(MAKE) -C CacheClient clean
	-$(MAKE) -C CodeCacheFootprint clean
	-$(MAKE) -C SuperPin clean
	-$(MAKE) -C Maid clean
	-$(MAKE) -C Probes clean
	-$(MAKE) -C AlignChk clean
	-$(MAKE) -C Mix  clean
	-$(MAKE) -C Insmix clean
	-$(MAKE) -C SegmentsVirtualization clean
	-$(MAKE) -C I18N clean
	-$(MAKE) -C MemTrace clean
	-$(MAKE) -C ChildProcess clean
	-$(MAKE) -C CrossIa32Intel64 clean
	-$(MAKE) -C MyPinTool clean

