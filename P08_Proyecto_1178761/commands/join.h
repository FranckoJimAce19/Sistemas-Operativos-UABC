/**
 * @file  join.h
 * @brief Comando join — muestra o crea archivos de texto via syscalls POSIX.
 */
#ifndef JOIN_H
#define JOIN_H

/**
 * @brief Muestra en consola el contenido del archivo especificado.
 *        Usa open(), read(), write(), close() para lectura POSIX.
 * @param filePath Ruta del archivo a mostrar.
 */
void joinRead(const char *filePath);

/**
 * @brief Entra en modo texto y guarda lo escrito por el usuario en un archivo.
 *        Termina cuando el usuario escribe '$' y presiona Enter.
 *        Usa open(), write(), close() para escritura POSIX.
 * @param filePath Ruta del archivo a crear/sobreescribir.
 */
void joinWrite(const char *filePath);

#endif /* JOIN_H */
