#include "tdas/extra.h"
#include "tdas/list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <math.h>

typedef struct Objeto {
  char* etiqueta;
  int puntuacion;
  int carga;
} Objeto;

typedef struct ObjetoInventario {
  char* etiqueta;
  int carga;
  int puntuacion;
  struct ObjetoInventario* siguiente;
} ObjetoInventario;

typedef struct Explorador {
  ObjetoInventario* primero;
  int carga_total;
  int puntos;
  int tiempo;
} Explorador;

typedef struct Nodo {
  int identificador;
  char* titulo;
  char* detalle;
  List* objetos;
  int enlaces_ids[4];
  struct Nodo* enlaces[4];
  int es_final;
} Nodo;

typedef struct Laberinto {
  Nodo* mapa[128];
  int total_nodos;
} Laberinto;

Laberinto* lab;

Laberinto* inicializarLaberinto();
void enlazarNodos(Laberinto* l);
void cargarEscenario(Laberinto * l);
void mostrarObjetos(List* objs);
void mostrarEstado(Nodo* n, Explorador* e);
void tomarObjeto(Nodo* n, Explorador* e);
void dejarObjeto(Explorador* e);
void iniciarJuego();

int main() {
  SetConsoleOutputCP(CP_UTF8);
  SetConsoleCP(CP_UTF8);
  int opcion;
  lab = inicializarLaberinto();

  do{
    printf("1) Cargar Escenario\n");
    printf("2) Comenzar Juego\n");
    scanf("%d", &opcion);

    if (opcion == 1){
      cargarEscenario(lab);
      enlazarNodos(lab);
    } else if(opcion == 2){
      if (lab->total_nodos > 0) {
        iniciarJuego();
      } else {
        printf("Primero carga un escenario.\n");
      }
    } else if (opcion != 2) {
      printf("Opcion invalida.\n");
    }
  } while(opcion != 2);

  printf("Fin del programa.\n");
  return 0;
}

Laberinto* inicializarLaberinto() {
  Laberinto* l = malloc(sizeof(Laberinto));
  for (int i = 0; i < 128; i++) l->mapa[i] = NULL;
  l->total_nodos = 0;
  return l;
}

void enlazarNodos(Laberinto* l) {
  for (int i = 0; i < 128; i++) {
    Nodo* n = l->mapa[i];
    if (!n) continue;
    for (int d = 0; d < 4; d++) {
      int eid = n->enlaces_ids[d];
      if (eid >= 0 && eid < 128 && l->mapa[eid] != NULL){
        n->enlaces[d] = l->mapa[eid];
      }
    }
  }
}

void cargarEscenario(Laberinto * l) {
  char archivo[256];
  printf("Archivo del escenario:\n");
  scanf("%s", archivo);
  FILE *fp = fopen(archivo, "r");
  if (fp == NULL) {
    perror("Error abriendo archivo");
    return;
  }

  char **datos;
  datos = leer_linea_csv(fp, ',');

  while ((datos = leer_linea_csv(fp, ',')) != NULL) {
    List* lista_str = split_string(datos[3], ";");
    List* listaObjs = list_create();
    Nodo* n = malloc(sizeof(Nodo));

    for(char *obj = list_first(lista_str); obj != NULL; obj = list_next(lista_str)){
      List* val = split_string(obj, ",");
      char* nombre = list_first(val);
      int valor = atoi(list_next(val));
      int peso = atoi(list_next(val));
      list_clean(val);
      free(val);
      Objeto* o = malloc(sizeof(Objeto));
      o->etiqueta = strdup(nombre);
      o->puntuacion = valor;
      o->carga = peso;
      list_pushBack(listaObjs, o);
    }
    n->objetos = listaObjs;
    list_clean(lista_str);
    free(lista_str);

    int arriba = atoi(datos[4]);
    int abajo = atoi(datos[5]);
    int izq = atoi(datos[6]);
    int der = atoi(datos[7]);

    int final = (strcmp(datos[8], "Si") == 0 || strcmp(datos[8], "SI") == 0 || strcmp(datos[8], "si") == 0);

    int identificador = atoi(datos[0]);

    n->identificador = identificador;
    n->titulo = strdup(datos[1]);
    n->detalle = strdup(datos[2]);
    n->es_final = final;
    n->enlaces_ids[0] = arriba;
    n->enlaces_ids[1] = abajo;
    n->enlaces_ids[2] = izq;
    n->enlaces_ids[3] = der;

    for (int d = 0; d < 4; d++) n->enlaces[d] = NULL;

    l->mapa[identificador] = n;
    l->total_nodos++;
  }
  fclose(fp);
}

void mostrarObjetos(List* objs) {
  if (objs == NULL) {
    printf("  Lista no inicializada\n");
    return;
  }
  if (is_empty(objs)) {
    printf("  (sin objetos)\n");
    return;
  }
  Objeto* o = (Objeto*) list_first(objs);
  while (o) {
    printf("  - %s (%d pts, %d kg)\n",o->etiqueta,o->puntuacion,o->carga);
    o = (Objeto*) list_next(objs);
  }
}

void mostrarEstado(Nodo* n, Explorador* e) {
  printf("\n=== NODO %d: %s ===\n", n->identificador, n->titulo);
  printf("%s\n\n", n->detalle);
  printf("Objetos en el lugar:\n");
  mostrarObjetos(n->objetos);

  printf("\n-- Estado del Explorador --\n");
  printf("Tiempo: %d\n", e->tiempo);
  printf("Carga total: %d kg\n", e->carga_total);
  printf("Puntos: %d pts\n", e->puntos);

  printf("Inventario:\n");
  if (!e->primero) {
    printf("  (vacio)\n");
  } else {
    for (ObjetoInventario* it = e->primero; it; it = it->siguiente) {
      printf("  - %s (%d pts, %d kg)\n", it->etiqueta, it->puntuacion, it->carga);
    }
  }

  printf("\n-- Opciones --\n");
  const char* direcciones[4] = {"Arriba","Abajo","Izquierda","Derecha"};
  for (int d = 0; d < 4; d++) {
    if (n->enlaces[d]) {
      printf("  %d) Ir %s -> nodo %d\n",d+1, direcciones[d], n->enlaces[d]->identificador);
    }
  }
  printf("  5) Tomar objeto\n");
  printf("  6) Dejar objeto\n");
  printf("  7) Reiniciar\n");
  printf("  8) Salir\n");
  printf("Elegir: ");
}

void tomarObjeto(Nodo* n, Explorador* e) {
  if (!n->objetos || list_size(n->objetos) == 0) {
    printf("No hay objetos.\n");
    return;
  }
  Objeto* obj = (Objeto*) list_first(n->objetos);
  if (!obj) return;

  ObjetoInventario* nuevo = malloc(sizeof(ObjetoInventario));
  nuevo->etiqueta = strdup(obj->etiqueta);
  nuevo->carga   = obj->carga;
  nuevo->puntuacion  = obj->puntuacion;
  nuevo->siguiente = e->primero;
  e->primero      = nuevo;

  e->carga_total += obj->carga;
  e->puntos    += obj->puntuacion;

  list_popFront(n->objetos);

  free(obj->etiqueta);
  free(obj);

  printf("Objeto obtenido: %s (%d pts, %d kg)\n", nuevo->etiqueta, nuevo->puntuacion, nuevo->carga);
}

void dejarObjeto(Explorador* e) {
  if (!e->primero) {
    printf("Inventario vacio.\n");
    return;
  }
  printf("Cual objeto deseas descartar?\n");
  ObjetoInventario* it = e->primero;
  int idx = 1;
  for (; it; it = it->siguiente, idx++) {
    printf("  %d) %s (%d pts, %d kg)\n",idx, it->etiqueta, it->puntuacion, it->carga);
  }
  printf("  0) Cancelar\nElegir: ");
  int opt;
  scanf("%d", &opt);
  if (opt <= 0) {
    printf("Accion cancelada.\n");
    return;
  }
  ObjetoInventario* ant = NULL;
  it = e->primero;
  idx = 1;
  while (it && idx < opt) {
    ant = it;
    it = it->siguiente;
    idx++;
  }
  if (!it) {
    printf("Seleccion invalida.\n");
    return;
  }
  if (ant) ant->siguiente = it->siguiente;
  else     e->primero = it->siguiente;

  e->carga_total -= it->carga;
  e->puntos      -= it->puntuacion;

  printf("Descartado: %s\n", it->etiqueta);
  free(it->etiqueta);
  free(it);
  e->tiempo--;
}

void iniciarJuego(){
  Explorador e = { .primero = NULL, .carga_total = 0, .puntos = 0, .tiempo = 10 };
  Nodo* pos = lab->mapa[1];
  int eleccion;

  while (e.tiempo > 0 && (pos && !pos->es_final) ) {
    mostrarEstado(pos, &e);
    scanf("%d", &eleccion);

    switch (eleccion) {
      case 1: case 2: case 3: case 4:
      {
        int d = eleccion - 1;
        if (pos->enlaces[d]) {
          int cost = (int)ceil((e.carga_total + 1) / 10.0);
          e.tiempo -= cost;
          pos = pos->enlaces[d];
        } else {
          printf("Direccion invalida.\n");
        }
        break;
      }
      case 5:
        tomarObjeto(pos, &e);
        break;
      case 6:
        dejarObjeto(&e);
        break;
      case 7:
        while (e.primero) {
          ObjetoInventario* tmp = e.primero;
          e.primero = e.primero->siguiente;
          free(tmp->etiqueta);
          free(tmp);
        }
        e.carga_total = 0;
        e.puntos = 0;
        e.tiempo = 10;
        pos = lab->mapa[1];
        printf("Reiniciado.\n");
        break;
      case 8:
        printf("Fin del juego.\n");
        return;
      default:
        printf("Invalido.\n");
    }
  }

  if (pos && pos->es_final) {
    printf("\nLlegaste al final.\n");
    printf("Inventario:\n");
    if (!e.primero) {
      printf("  (vacio)\n");
    } else {
      for (ObjetoInventario* it = e.primero; it; it = it->siguiente) {
        printf("  - %s (%d pts, %d kg)\n",
        it->etiqueta, it->puntuacion, it->carga);
      }
    }
    printf("Puntaje: %d pts\n", e.puntos);
  } else if (e.tiempo <= 0) {
    printf("\nTiempo agotado. Derrota.\n");
  } else {
    printf("\nJuego interrumpido.\n");
  }
}
