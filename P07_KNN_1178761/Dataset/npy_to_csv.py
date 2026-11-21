#!/usr/bin/env python3
"""
Script de conversion de archivos .npy a formato .csv para la Practica 7 (K-NN).
Convierte todos los archivos .npy dentro de la carpeta Dataset/ manteniendo sus
nombres originales y guardandolos en la carpeta data/.
No requiere librerias externas (funciona con la biblioteca estandar de Python).
"""

import os
import glob
import struct
import ast
import array

def npy_to_csv(npy_path, csv_path):
    with open(npy_path, 'rb') as f:
        magic = f.read(6)
        if magic != b'\x93NUMPY':
            raise ValueError(f"'{npy_path}' no es un archivo .npy valido.")
        
        major, minor = f.read(2)
        if major == 1:
            hlen = struct.unpack('<H', f.read(2))[0]
        else:
            hlen = struct.unpack('<I', f.read(4))[0]
            
        header = ast.literal_eval(f.read(hlen).decode('ascii', errors='ignore').strip())
        descr = header['descr']
        shape = header['shape']
        
        # Determinar tipo de dato
        if descr.endswith('f8'):
            typecode = 'd'  # float64
        elif descr.endswith('f4'):
            typecode = 'f'  # float32
        elif descr.endswith('i8'):
            typecode = 'q'  # int64
        elif descr.endswith('i4'):
            typecode = 'i'  # int32
        elif descr.endswith('u1'):
            typecode = 'B'  # uint8
        else:
            typecode = 'd'
            
        total_elements = 1
        for s in shape:
            total_elements *= s
            
        data = array.array(typecode)
        data.fromfile(f, total_elements)
        
    with open(csv_path, 'w') as out:
        if len(shape) <= 1:
            out.write('\n'.join(map(str, data)) + '\n')
        else:
            rows, cols = shape
            buf = []
            for r in range(rows):
                row_slice = data[r * cols : (r + 1) * cols]
                buf.append(','.join(map(str, row_slice)))
                if len(buf) >= 2000:
                    out.write('\n'.join(buf) + '\n')
                    buf = []
            if buf:
                out.write('\n'.join(buf) + '\n')

def main():
    script_dir = os.path.dirname(os.path.abspath(__file__))
    dataset_dir = script_dir
    project_root = os.path.dirname(dataset_dir)
    data_dir = os.path.join(project_root, 'data')
    
    os.makedirs(data_dir, exist_ok=True)
    
    pattern = os.path.join(dataset_dir, '*.npy')
    npy_files = sorted(glob.glob(pattern))
    
    # Filtrar metadatos de WSL/NTFS tipo :Zone.Identifier
    npy_files = [f for f in npy_files if not f.endswith(':Zone.Identifier')]
    
    if not npy_files:
        print(f"No se encontraron archivos .npy en {dataset_dir}")
        return
        
    print(f"Iniciando conversion de {len(npy_files)} archivos a {data_dir}...")
    for npy_path in npy_files:
        base_name = os.path.splitext(os.path.basename(npy_path))[0]
        csv_name = f"{base_name}.csv"
        csv_path = os.path.join(data_dir, csv_name)
        
        print(f"  Convirtiendo '{os.path.basename(npy_path)}' -> '{csv_name}'...", end=' ', flush=True)
        npy_to_csv(npy_path, csv_path)
        print("OK")
        
    print("Conversion completada exitosamente.")

if __name__ == '__main__':
    main()
