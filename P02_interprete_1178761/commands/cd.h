/**
 * @file  cd.h
 * @brief Comando cd — cambia el directorio de trabajo del interprete.
 */
#ifndef CD_H
#define CD_H

/**
 * @brief Cambia el directorio de trabajo.
 *        Si no se proporciona path, va al HOME del usuario.
 *        Soporta ~ para el HOME del usuario.
 * @param path Directorio destino. Puede ser NULL, vacio, "~" o "directorio".
 */
void cd(const char *path);

#endif /* CD_H */
