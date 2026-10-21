/**
 * @file  del.h
 * @brief Comando del — elimina un archivo via syscall unlink().
 */
#ifndef DEL_H
#define DEL_H

/**
 * @brief Elimina el archivo especificado usando unlink().
 * @param filePath Ruta del archivo a eliminar.
 */
void del(const char *filePath);

#endif /* DEL_H */
