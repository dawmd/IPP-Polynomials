/** @file
  Implementacja dynamicznej tablicy jednomianów
 
  @author Dawid Mędrek
  @date 2021
*/

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "monovector.h"


/** Standardowy rozmiar niepustej tablicy jednomianów */
#define DEFAULT_SIZE 4
/** 
 * Maksymalny rozmiar tablicy jednomianów
 * -- maksymalna wartość typu `size_t`
 */
#define MAX_SIZE SIZE_MAX


/** Funkcja sprawdzająca, czy wskaźnik @p p jest równy @p NULL.
 * Jeśli jest -- awaryjnie kończy działanie programu kodem @p 1.
 * W przeciwnym wypadku nie robi nic.
 * @param[in] p : wskaźnik
 */
#define CHECK_PTR(p)  \
  do {                \
    if (p == NULL) {  \
      exit(1);        \
    }                 \
  } while (0)


/**
 * Tworzy nowy wektor, nie alokując pamięci na tablicę jednomianów,
 * a następnie zwraca go.
 */
MonoVector CreateMonoVector() {
  MonoVector newVector = (MonoVector) {
    .monos  = NULL,
    .length = 0,
    .size   = 0
  };
  return newVector;
}

/**
 * Sprawdza, czy przekazany wskaźnik na wektor nie jest równy @p NULL,
 * a następnie zwraca liczbę jednomianów w tablicy
 */
size_t VectorLength(MonoVector *vector) {
  assert(vector != NULL);
  return vector->length;
}

/**
 * Zwraca minimum z dwóch liczb typu @p size_t.
 * @param[in] num1 : liczba typu @p size_t
 * @param[in] num2 : liczba typu @p size_t
 * @return @p min(num1,num2)
 */
static inline size_t MinSize_t(const size_t num1, const size_t num2) {
  if (num1 < num2) { return num1; }
  else             { return num2; }
}

/**
 * Zmienia rozmiar tablicy typu @p Mono składającą się na przekazany wektor.
 * Funkcja zakłada, że przekazany wskaźnik @p vector wskazuje na istniejącą
 * i poprawną strukturę danych.
 * @param[in] vector : wskaźnik na wektor jednomianów
 * @param[in] newSize : nowy rozmiar tablicy jednomianów
 * 
 * @details
 * Jeśli rozmiar wektora zmniejszy się na tyle, że nie wszystkie
 * znajdujące się w nim jednomiany zmieszczą się w nowej tablicy,
 * funkcja nie usunie ich z pamięci.
 */
static inline void ResizeVector(MonoVector *vector, const size_t newSize) {
  // Jeśli ten sam rozmiar, nie wprowadza zmian
  if (newSize == vector->size) {
    return;
  }

  // Nowy rozmiar jest równy zeru, a więc zwalnia
  // zaalokowaną tablicę typu `Mono`
  if (newSize == 0) {
    free(vector->monos);
    vector->monos = NULL;
  }
  // Nowy rozmiar jest liczbą dodatnią
  else {
    // Wskaźnik odpowiadający nowej tablicy jednomianów
    Mono *newMonos;
    // Oryginalna tablica ma rozmiar równy zeru -- alokacja
    if (vector->size == 0) {
      newMonos = malloc(newSize * sizeof(Mono));
    }
    // Zmiana rozmiaru istniejącej tablicy jednomianów
    else {
      newMonos = realloc(vector->monos, newSize * sizeof(Mono));
    }

    // Sprawdzenie, czy otrzymany wskaźnik nie jest równy `NULL`
    CHECK_PTR(newMonos);

    // Przypisanie wektorowi nowej tablicy
    vector->monos = newMonos;
  }

  // Aktualizacja pól wektora
  vector->length = MinSize_t(vector->length, newSize);
  vector->size   = newSize;
}

/**
 * Zwraca minimum z dwóch liczb: @p 2*consideredSize, @p MAX_SIZE.
 * @param[in] consideredSize : liczba typu @p size_t
 * @return @p min(consideredSize,MAX_SIZE)
 * 
 * @details
 * Jako, że liczba @p 2*consideredSize może być bardzo duża, funkcja
 * nie mnoży przekazanej liczby, ale dzieli @p MAX_SIZE przez @p 2.
 * W ten sposób unika zagrożenia związanego z tzw. "overflow".
 */
static inline size_t ChooseNewSize(const size_t consideredSize) {
  if (consideredSize < MAX_SIZE / 2) { return 2 * consideredSize; }
  else                               { return MAX_SIZE;           }
}

/**
 * Sprawdza, czy przekazany wskaźnik na wektor nie jest równy @p NULL.
 * Jeśli tablica jednomianów jest wypełniona w całości, funkcja sprawdza,
 * czy może ją rozszerzyć. Jeśli nie -- kończy awaryjnie działanie programu
 * z kodem @p 1. W przeciwnym wypadku rozszerza tablicę. Następnie dodaje
 * przekazany jednomian do tablicy i aktualizuje pola wektora odnośnie jego
 * długości.
 */
void AppendMono(MonoVector *vector, Mono mono) {
  assert(vector != NULL);

  // Należy rozszerzyć tablicę jednomianów
  if (vector->length >= vector->size) {
    // Nie jest możliwe rozszerzenie -- błąd
    if (vector->size == MAX_SIZE) {
      exit(1);
    }

    // Stworzenie tablicy o domyślnym rozmiarze
    if (vector->size == 0) {
      ResizeVector(vector, DEFAULT_SIZE);
    }
    // Rozszerzenie tablicy
    else {
      ResizeVector(vector, ChooseNewSize(vector->size));
    }
  }

  // Przypisanie nowego jednomianu
  vector->monos[vector->length] = mono;
  // Aktualizacja liczby jednomianów w wektorze
  vector->length++;
}

/**
 * Sprawdza, czy przekazany wskaźnik na wektor nie jest równy @p NULL
 * i czy @p index mieści się w zakresie tablicy jednomianów. Następnie
 * zwraca jednomian znajdujący się w wektorze pod danym indeksem.
 */
Mono GetMonoAt(MonoVector *vector, const size_t index) {
  assert(vector != NULL && index < vector->length);
  return vector->monos[index];
}

/**
 * Sprawdza, czy przekazany wskaźnik na wektor nie jest równy @p NULL
 * i czy w wektorze znajdują się jakiekolwiek jednomiany. Następnie
 * zwraca wskaźnik na pierwszy element tablicy jednomianów, z której
 * złożony jest wektor.
 */
Mono *ConvertToArr(MonoVector *vector) {
  assert(vector != NULL && vector->length > 0);
  return vector->monos;
}

/**
 * Sprawdza, czy przekazany wskaźnik na wektor nie jest równy @p NULL.
 * Następnie usuwa z pamięci wszystkie znajdujące się w nim jednomiany.
 */
void DestroyMonos(MonoVector *vector) {
  assert(vector != NULL);
  if (vector->length > 0) {
    for (size_t i = 0; i < vector->length; i++) {
      MonoDestroy(&vector->monos[i]);
    }
  }
}

/**
 * Sprawdza, czy przekazany wskaźnik na wektor nie jest równy @p NULL.
 * Następnie, jeśli tablica jednomianów jest różna od @p NULL, zwalnia
 * przydzieloną na nią pamięć i aktualizuje pola wektora.
 */
void DestroyVector(MonoVector *vector) {
  if (vector != NULL) {
    if (vector->monos != NULL) {
      free(vector->monos);
      vector->monos = NULL;
    }
    // Aktualizacja pól wektora
    vector->size = 0;
    vector->length = 0;
  }
}


