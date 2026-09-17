# Práctica 2 — Intérprete de Comandos (System Calls)
> Sistemas Operativos · UABC · Clave 36299 · 2026-2

---

## Objetivo

Implementar un **intérprete de comandos en C** que realice operaciones de manejo de archivos usando exclusivamente llamadas al sistema POSIX (`unistd.h`, `fcntl.h`).

El intérprete debe:
- Seguir corriendo después de ejecutar cada comando (no salir si hay error).
- Aceptar **rutas absolutas o relativas**, no solo nombres de archivo.
- Tener un comando `help` que liste todos los comandos disponibles y su sintaxis (se irá actualizando durante todo el proyecto).
- Cada comando tendrá su propio par de archivos **`<comando>.c`** y **`<comando>.h`**.

---

## Estructura de archivos del proyecto

```
P02_interprete_1178761/
├── INSTRUCCIONES_P02.md       ← este archivo
├── main.c                     ← bucle principal del intérprete
├── main.h                     ← cabecera general (includes compartidos)
│
├── commands/                  ← un .c y .h por cada comando
│   ├── info.c / info.h        ← comando info (ayuda)
│   ├── info.txt               ← texto de ayuda (se edita para actualizar comandos)
│   ├── join.c / join.h        ← comandos join y join> (lectura y creación)
│   ├── copy.c / copy.h        ← comando copy (copiar archivos)
│   └── del.c / del.h          ← comando del (eliminar archivos)
│
└── Makefile                   ← compilación de todos los archivos
```

---

## Comandos a implementar

### 1. `join` — Mostrar contenido de un archivo

**Sintaxis:**
```
join [ruta/nombre_archivo]
```

**Comportamiento:**
- Abre el archivo con `open()` usando `O_RDONLY`.
- Lee su contenido en bloques con `read()` e imprime en `stdout` con `write()`.
- Cierra el archivo con `close()`.
- Si el archivo no existe o no se puede abrir → imprimir error **sin salir del intérprete**.

**Syscalls usadas:** `open`, `read`, `write`, `close`

---

### 2. `join>` — Crear un nuevo archivo de texto

**Sintaxis:**
```
join> [ruta/nombre_archivo]
```

**Comportamiento:**
- Tras recibir el comando, entra en **modo texto**: captura todo lo que escribe el usuario (incluidos espacios y saltos de línea).
- El modo texto termina cuando el usuario escribe el carácter **`$`** y presiona Enter.
- El texto capturado se escribe en el archivo nuevo con `write()`.
- Crea el archivo si no existe (`O_CREAT`) con permisos `S_IRUSR | S_IWUSR`.
- Cierra el archivo con `close()`.

**Syscalls usadas:** `open`, `write`, `close`

---

### 3. `copy` — Copiar archivo

**Sintaxis:**
```
copy [ruta/archivo_fuente] [ruta/archivo_destino]
```

**Comportamiento:**
- Abre el archivo fuente con `open()` en modo `O_RDONLY`.
- Abre (o crea) el archivo destino con `open()` en modo `O_WRONLY | O_CREAT | O_TRUNC`.
- Copia el contenido en bloques con `read()` + `write()`.
- Cierra ambos archivos con `close()`.
- Si el archivo fuente no existe → imprimir error **sin salir del intérprete**.

**Syscalls usadas:** `open`, `read`, `write`, `close`

---

### 4. `del` — Eliminar un archivo

**Sintaxis:**
```
del [ruta/nombre_archivo]
```

**Comportamiento:**
- Elimina el archivo especificado con `unlink()`.
- Si el archivo no existe o no se puede eliminar → imprimir error **sin salir del intérprete**.

**Syscalls usadas:** `unlink`

---

### 5. `info` — Mostrar ayuda

**Sintaxis:**
```
info
```

**Comportamiento:**
- Muestra en consola la lista de todos los comandos disponibles con su sintaxis y descripción breve.
- Este comando se actualizará cada vez que se agregue un nuevo comando durante el proyecto.

---

## Comportamiento general del intérprete

```
$ ./interprete

======= Interprete de Comandos SO =======
Escribe 'info' para ver los comandos.
>> [el usuario escribe aquí]
```

- El prompt `>>` aparece después de cada operación (exitosa o con error).
- Si el comando escrito **no existe**, mostrar:
  ```
  Comando no reconocido: 'xyz'. Escribe 'info' para ver los comandos disponibles.
  ```
  y continuar sin salir.
- El intérprete solo termina cuando el usuario escribe `exit` o cierra la terminal.
- Las **rutas** (absolutas como `/home/user/archivo.txt` o relativas como `../carpeta/archivo.txt`) deben funcionar igual que los nombres simples.

---

## Headers/Syscalls de referencia

| Header       | Funciones a usar                           |
|--------------|--------------------------------------------|
| fcntl.h      | open()                                     |
| unistd.h     | read(), write(), close(), unlink()         |
| stdio.h      | printf(), fgets() (solo para el prompt)    |
| string.h     | strcmp(), strtok(), strcpy(), etc.         |
| stdlib.h     | malloc(), free(), exit()                   |

### Flags de open() a usar

| Flag       | Descripción                              |
|------------|------------------------------------------|
| O_RDONLY   | Solo lectura                             |
| O_WRONLY   | Solo escritura                           |
| O_CREAT    | Crear archivo si no existe               |
| O_TRUNC    | Truncar archivo al abrir                 |
| O_APPEND   | Escribir al final del archivo            |
| S_IRUSR    | Permiso de lectura para el propietario   |
| S_IWUSR    | Permiso de escritura para el propietario |

---

## Pasos de implementación sugeridos

- [ ] Paso 1 — Crear la estructura de carpetas y archivos vacíos del proyecto.
- [ ] Paso 2 — Implementar main.c: bucle que lee input, hace tokenize de la línea, identifica el comando y llama a la función correspondiente.
- [ ] Paso 3 — Implementar info.c / info.h: imprime la tabla de comandos disponibles.
- [ ] Paso 4 — Implementar join.c / join.h: modo lectura (join archivo) y modo escritura (join> archivo).
- [ ] Paso 5 — Implementar copy.c / copy.h: copia de archivos.
- [ ] Paso 6 — Implementar del.c / del.h: eliminar archivo con unlink().
- [ ] Paso 7 — Crear el Makefile para compilar todo con un solo comando make.
- [ ] Paso 8 — Probar cada comando con rutas absolutas y relativas.
- [ ] Paso 9 — Verificar que el intérprete nunca se cierra por un error de comando.

---

## Compilación

```bash
# Con Makefile:
make

# Manual:
gcc -o interprete main.c commands/info.c commands/join.c commands/copy.c commands/del.c

# Ejecutar:
./interprete
```

---

## Notas importantes

> IMPORTANTE: Usar exclusivamente las syscalls de unistd.h y fcntl.h para operaciones de archivo (no usar fopen, fread, fwrite, fclose de stdio.h).

> ADVERTENCIA: El intérprete nunca debe terminar debido a un error en un comando. Solo debe salir con el comando exit.

> NOTA: El comando info se actualizará a lo largo del proyecto conforme se agreguen nuevos comandos. Documentar cada nuevo comando en commands/info.c.
