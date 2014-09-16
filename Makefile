
#
# The base directory.
#
ROOT=./cc3200-sdk

#
# Include the common make definitions.
#
include ${ROOT}/tools/gcc_scripts/makedefs

CFLAGS+=-DUSE_FREERTOS

#
# Where to find source files that do not live in this directory.
#
VPATH=..
VPATH+=${ROOT}/example/drivers
VPATH+=${ROOT}/example/common

#
# Where to find header files that do not live in the source directory.
#
IPATH=..
IPATH+=${ROOT}/example
IPATH+=${ROOT}/example/common
IPATH+=${ROOT}/inc
IPATH+=${ROOT}/oslib
IPATH+=${ROOT}/driverlib
IPATH+=${ROOT}/third_party/FreeRTOS
IPATH+=${ROOT}/third_party/FreeRTOS/source
IPATH+=${ROOT}/third_party/FreeRTOS/source/portable/GCC/ARM_CM4
IPATH+=${ROOT}/third_party/FreeRTOS/source/include
#
# The default rule, which causes the driver library to be built.
#
all: ${OBJDIR} ${BINDIR}
all: ${BINDIR}/wiswi.axf

#
# The rule to clean out all the build products.
#
clean:
	@rm -rf ${OBJDIR} ${wildcard *~}
	@rm -rf ${BINDIR} ${wildcard *~}


#
# The rule to create the target directories.
#
${OBJDIR}:
	@mkdir -p ${OBJDIR}

${BINDIR}:
	@mkdir -p ${BINDIR}

${BINDIR}/wiswi.axf: ${OBJDIR}/main.o
${BINDIR}/wiswi.axf: ${OBJDIR}/pinmux.o
${BINDIR}/wiswi.axf: ${OBJDIR}/gpio_if.o
${BINDIR}/wiswi.axf: ${OBJDIR}/uart_if.o
${BINDIR}/wiswi.axf: ${OBJDIR}/freertos_hooks.o
${BINDIR}/wiswi.axf: ${OBJDIR}/startup_${COMPILER}.o
${BINDIR}/wiswi.axf: ${ROOT}/driverlib/${COMPILER}/${BINDIR}/libdriver.a
${BINDIR}/wiswi.axf: ${ROOT}/oslib/${COMPILER}/${BINDIR}/FreeRTOS.a	
SCATTERgcc_wiswi=wiswi.ld
ENTRY_wiswi=ResetISR

#
# Include the automatically generated dependency files.
#
ifneq (${MAKECMDGOALS},clean)
-include ${wildcard ${COMPILER}/*.d} __dummy__
endif
