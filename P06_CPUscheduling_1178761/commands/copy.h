/**
 * @file  copy.h
 * @brief Comando copy — copia el contenido de un archivo a otro via syscalls POSIX.
 */
#ifndef COPY_H
#define COPY_H

/**
 * @brief Copia el contenido de source a destination usando open/read/write/close.
 * @param source Ruta del archivo fuente.
 * @param destination Ruta del archivo destino.
 */
void copy(const char *source, const char *destination);

#endif /* COPY_H */
