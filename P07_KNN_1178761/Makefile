CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -O2 -MMD -MP
LDLIBS  = -pthread
BUILD   = build

TARGET  = interprete
KNN_BIN = busqueda_knn

# Modulo K-NN compartido por ./busqueda_knn y por el comando knn del interprete
KNN_SRCS = knn/dataset.c knn/knn.c knn/busqueda.c

INTERPRETE_SRCS = main.c \
                  $(wildcard commands/*.c) \
                  $(wildcard process/*.c) \
                  $(wildcard scheduler/*.c) \
                  $(KNN_SRCS)

KNN_MAIN_SRCS = knn/main_knn.c $(KNN_SRCS)

INTERPRETE_OBJS = $(INTERPRETE_SRCS:%.c=$(BUILD)/%.o)
KNN_MAIN_OBJS   = $(KNN_MAIN_SRCS:%.c=$(BUILD)/%.o)

.PHONY: all clean datos

all: $(TARGET) $(KNN_BIN)

$(TARGET): $(INTERPRETE_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)
	@echo "Compilacion exitosa: ./$(TARGET)"

$(KNN_BIN): $(KNN_MAIN_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)
	@echo "Compilacion exitosa: ./$(KNN_BIN)"

# Cada .c se compila a build/<ruta>.o; -MMD genera el .d con los .h que usa
$(BUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

# Convierte Dataset/*.npy a data/*.csv
datos:
	python3 Dataset/npy_to_csv.py

clean:
	rm -rf $(BUILD) $(TARGET) $(KNN_BIN)
	@echo "Binarios eliminados."

-include $(INTERPRETE_OBJS:.o=.d) $(KNN_MAIN_OBJS:.o=.d)
