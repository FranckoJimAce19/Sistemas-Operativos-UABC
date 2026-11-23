CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -O2 -MMD -MP
LDLIBS  = -pthread
BUILD   = build

TARGET  = interprete

SRCS = main.c \
       $(wildcard commands/*.c) \
       $(wildcard process/*.c) \
       $(wildcard memory/*.c) \
       $(wildcard scheduler/*.c) \
       $(wildcard knn/*.c) \
       $(wildcard utilidades/*.c)

OBJS = $(SRCS:%.c=$(BUILD)/%.o)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)
	@echo "Compilacion exitosa: ./$(TARGET)"

# Cada .c se compila a build/<ruta>.o; -MMD genera el .d con los .h que usa
$(BUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD) $(TARGET)
	@echo "Binarios eliminados."

-include $(OBJS:.o=.d)
