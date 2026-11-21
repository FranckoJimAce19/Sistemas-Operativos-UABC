/**
 * @file  pipeline.h
 * @brief Ejecucion de comandos externos y tuberias (cmd1 | cmd2 | ...).
 */
#ifndef PIPELINE_H
#define PIPELINE_H

/**
 * @brief Parsea la linea y ejecuta cada comando en un proceso hijo,
 *        conectando la salida de cada uno con la entrada del siguiente.
 * @return 0 si se ejecuto, -1 si hubo error de sintaxis o de sistema.
 */
int runPipeline(const char *line);

#endif /* PIPELINE_H */
