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
          commands/exit_cmd.c \
          commands/parser.c \
          commands/pipeline.c \
          process/lista_procesos.c \
          scheduler/sched_util.c \
          scheduler/fcfs.c \
          scheduler/sjf.c \
          scheduler/rr.c \
          shell_cmd/cmd_procesos.c

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)
	@echo "Compilacion exitosa: ./$(TARGET)"

clean:
	rm -f $(TARGET)
	@echo "Binarios eliminados."
