CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11
TARGET  = interprete

SRCS    = main.c \
          commands/info.c \
          commands/join.c \
          commands/copy.c \
          commands/del.c \
          commands/cd.c \
          commands/sdir.c \
          commands/clear.c \
          commands/exit_cmd.c

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)
	@echo "Compilacion exitosa: ./$(TARGET)"

clean:
	rm -f $(TARGET)
	@echo "Binarios eliminados."
