# GraphQuest

Este programa implementa un laberinto interactivo donde el jugador puede explorar salas, recoger y descartar objetos, y llegar a una sala final antes de quedarse sin tiempo.

## ✅ Cómo compilar y ejecutar

1. Abre la terminal en la carpeta donde están los archivos `.c`, `.h` y el archivo `graphquest.csv`.
2. Compila el programa con el siguiente comando:

   ```bash
   gcc graphquest.c tdas/*.c -o graphquest
   ```

   Asegúrate de que los archivos `list.c`, `list.h`, `extra.c` y `extra.h` estén en la carpeta `tdas/`.

3. Ejecuta el programa:

   ```bash
   ./graphquest
   ```

   En Windows:

   ```cmd
   graphquest.exe
   ```

## ✅ Opciones funcionales

| Opción del Menú         | Estado         | Observaciones |
|-------------------------|----------------|---------------|
| Cargar escenario        | ✅ Funciona     | Carga automáticamente `graphquest.csv`. |
| Comenzar juego          | ✅ Funciona     | Inicia el recorrido desde la sala 1. |
| Moverse entre salas     | ✅ Funciona     | Usa las opciones 1 a 4. Se descuenta tiempo según el peso transportado. |
| Tomar objeto            | ✅ Funciona     | Agrega el primer objeto de la sala al inventario. |
| Dejar objeto            | ✅ Funciona     | Permite descartar un objeto del inventario. |
| Reiniciar partida       | ✅ Funciona     | Restaura el tiempo y el inventario. |
| Salir del juego         | ✅ Funciona     | Termina la ejecución. |

### ❌ Funcionalidades no implementadas o limitaciones

- Solo se toma **el primer objeto** disponible en la sala (no se puede elegir cuál).
- No hay validación si el archivo `graphquest.csv` no está presente (se lanza error).
- El laberinto debe comenzar en la sala con ID **1**, y estar bien definido en el CSV.

## 🧪 Ejemplo de interacción

```text
1) Cargar Escenario
2) Comenzar Juego
1
Escenario cargado.

1) Cargar Escenario
2) Comenzar Juego
2

=== NODO 1: Entrada Principal ===
Te encuentras en la sala de entrada...

Objetos en el lugar:
  - Llave Dorada (10 pts, 2 kg)

-- Estado del Explorador --
Tiempo: 10
Carga total: 0 kg
Puntos: 0 pts

Inventario:
  (vacío)

-- Opciones --
  2) Ir Abajo -> nodo 2
  5) Tomar objeto
  6) Dejar objeto
  7) Reiniciar
  8) Salir
Elegir: 5

Objeto obtenido: Llave Dorada (10 pts, 2 kg)
```

## 📎 Archivos incluidos

- `main.c`
- `tdas/list.c`, `tdas/list.h`
- `tdas/extra.c`, `tdas/extra.h`
- `graphquest.csv`
- `README.md`
