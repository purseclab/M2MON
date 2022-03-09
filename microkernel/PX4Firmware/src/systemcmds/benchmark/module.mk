MODULE_COMMAND = benchmark
SRCS = benchmark.c core_portme.c core_state.c core_matrix.c core_list_join.c core_util.c
MAXOPTIMIZATION = -Os
MODULE_STACKSIZE = 4096

ifndef $(ITERATIONS)
ITERATIONS=0
endif

EXTRACFLAGS += -Wno-float-equal
EXTRACFLAGS += -DITERATIONS=$(ITERATIONS)
