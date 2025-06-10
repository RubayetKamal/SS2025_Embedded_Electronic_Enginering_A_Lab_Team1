CC = gcc
CFLAGS = -ggdb3 -O2 -pthread -Wall \
    -I. \
    -I$(FREERTOS_SRC)/include \
    -I$(FREERTOS_SRC)/portable/ThirdParty/GCC/Posix \
    -I$(FREERTOS_DEMO)/Common/include

LDFLAGS = -lpthread

FREERTOS_SRC = ../../Source
FREERTOS_DEMO = ../Common

SRC = \
    $(FREERTOS_SRC)/list.c \
    $(FREERTOS_SRC)/queue.c \
    $(FREERTOS_SRC)/tasks.c \
    $(FREERTOS_SRC)/timers.c \
    $(FREERTOS_SRC)/event_groups.c \
    $(FREERTOS_SRC)/portable/MemMang/heap_4.c \
    $(FREERTOS_SRC)/portable/ThirdParty/GCC/Posix/port.c \
    $(FREERTOS_SRC)/portable/ThirdParty/GCC/Posix/utils/wait_for_event.c \
    main.c \
    trafficLights.c \
    cars.c \
    pedestrianLights.c \
    pedestrian.c

OBJ = $(SRC:.c=.o)

TARGET = main

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET) $(OBJ)
