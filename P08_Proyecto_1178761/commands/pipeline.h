/**
 * @file  pipeline.h
 * @brief Ejecucion de comandos externos y tuberias (cmd1 | cmd2 | ...).
 */
#ifndef PIPELINE_H
#define PIPELINE_H

/**
 * @brief Parsea la linea y ejecuta cada comando en un proceso hijo,
 *        conectando la salida de cada uno con la entrada del siguiente.
 * @return Estado del ultimo comando (0 = exito, 127 = no encontrado,
 *         128 + n = terminado por la senal n); -1 si hubo error de sintaxis.
 */
int runPipeline(const char *line);

#endif /* PIPELINE_H */
