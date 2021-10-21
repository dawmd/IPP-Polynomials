/** @file
  Implementacja dynamicznych tablic znaków (stringów)
 
  @author Dawid Mędrek
  @date 2021
*/

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "newstring.h"

/** Standardowy rozmiar niepustego stringa */
#define DEFAULT_SIZE 32
/** Maksymalny rozmiar stringa -- maksymalna wartość typu `size_t` */
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
 * Tworzy nowego stringa, ale nie przydziela żadnej pamięci na tablicę
 * typu @p char. Następnie zwraca go.
 */
string_t CreateString() {
  string_t newString = (string_t) {
    .chars  = NULL,
    .length = 0,
    .size   = 0
  };
  return newString;
}

/**
 * Sprawdza, czy wskaźnik na string nie jest równy @p NULL, a następnie
 * zwraca jego długość.
 */
size_t StringLength(string_t *string) {
  assert(string != NULL);
  return string->length;
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
 * Zmienia rozmiar tablicy znaków danego stringa. Funkcja zakłada, że
 * przekazany wskaźnik na string wskazuje na istniejącą i poprawną
 * strukturę danych.
 * @param[in] string : wskaźnik na string
 * @param[in] newSize : nowy rozmiar tablicy znaków
 */
static inline void ResizeString(string_t *string, const size_t newSize) {
  // Jeżeli ten sam rozmiar, nie wprowadza zmian
  if (newSize == string->size) {
    return;
  }

  // Nowy rozmiar jest równy zeru, a więc zwalnia zaalokowaną
  // tablicę znaków
  if (newSize == 0) {
    free(string->chars);
    string->chars = NULL;
  }
  // Nowy rozmiar jest liczbą dodatnią
  else {
    // Wskaźnik odpowiadający nowej tablicy znaków
    char *newChars;
    // Utworzenie nowej tablicy
    if (string->size == 0) {
      newChars = malloc(newSize * sizeof(char));
    }
    // Rozszerzenie tablicy
    else {
      newChars = realloc(string->chars, newSize * sizeof(char));
    }

    // Sprawdzenie, czy otrzymany wskaźnik nie jest równy `NULL`
    CHECK_PTR(newChars);

    // Przypisanie nowej tablicy
    string->chars = newChars;
  }

  // Aktualizacji długości i rozmiaru stringa
  string->length = MinSize_t(string->length, newSize);
  string->size   = newSize;
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
 * Dodaje znak na koniec stringa. Funkcja zakłada, że wskaźnik na string
 * wskazuje na istniejącą i poprawną strukturę danych.
 * @param[in] string : wskaźnik na string
 * @param[in] c : znak typu @p char
 */
static inline void AppendChar(string_t *string, const char c) {
  // Należy rozszerzyć tablicę znaków
  if (string->length >= string->size) {
    // Nie da się rozszerzyć tablicy charów -- błąd
    if (string->size == MAX_SIZE) {
      exit(1);
    }

    // Stworzenie tablicy o domyślnym rozmiarze
    if (string->size == 0) {
      ResizeString(string, DEFAULT_SIZE);
    }
    // Rozszerzenie tablicy
    else {
      ResizeString(string, ChooseNewSize(string->size));
    }
  }

  // Przypisanie nowego znaku do tablicy
  string->chars[string->length] = c;
  // Aktualizacja długości stringa
  string->length++;
}

/**
 * Sprawdza, czy wskaźnik na string nie jest równy @p NULL. Wczytuje znaki
 * tak długo, aż nie napotka znaku nowej linii lub @p EOF i dodaje je do
 * tablicy znaków. Następnie zwraca odpowiedni typ linii.
 */
LineType ScanString(string_t *string) {
  assert(string != NULL);
  // Zmienna pomocnicza służąca do wczytywania znaków z wejścia
  char newChar;
  
  while (true) {
    // Wczytuje znak
    newChar = getchar();
    
    switch (newChar) {
      // Koniec linii
      case '\n':
        return EndOfLine;
      // Koniec pliku
      case EOF:
        return EndOfFile;
      // "Zwykły" char -- dodaje do tablicy
      default:
        AppendChar(string, newChar);
        break;
    }
  }
}

/**
 * Sprawdza, czy wskaźnik na string nie jest równy @p NULL i czy @p index
 * nie wykracza poza zakres tablicy znaków. Następnie zwraca znak
 * znajdujący się na danym indeksie w tablicy znaków odpowiadającej
 * danemu stringowi.
 */
char CharAt(string_t *string, const size_t index) {
  assert(string != NULL && index < string->length);
  return string->chars[index];
}

/**
 * Sprawdza, czy wskaźniki na string i char nie są równe @p NULL.
 * Następnie porównuje kolejne znaki aż do zakończenia stringa lub @p text.
 * Jeśli nie zakończyły się one jednocześnie -- nie są one równe.
 * W przeciwnym razie są.
 */
bool CompareTo(string_t *string, const char *text) {
  assert(string != NULL && text != NULL);

  // Zmienna pomocnicza
  size_t index = 0;

  // Sprawdzenie kolejnych znaków aż do zakończenia stringa lub ciągu znaków
  while (index < string->length && text[index] != '\0') {
    // Znaki nie są te same -- nie są równe
    if (string->chars[index] != text[index]) {
      return false;
    }
    index++;
  }

  // Nie zakończyły się jednocześnie -- nie są równe
  if (index != string->length || text[index] != '\0') {
    return false;
  }
  // Są równe
  else {
    return true;
  }
}

/**
 * Sprawdza, czy wskaźnik na string nie jest równy @p NULL i czy indeks
 * nie wykracza poza długość stringa. Jeśli rozmiar tablicy znaków
 * jest większy od długości stringa, ustawia znak nowej linii na odpowiednim
 * indeksie, aby zaznaczyć koniec ciągu znaków. Następnie zwraca wskaźnik
 * na znak na indeksie równym @p index w tablicy znaków.
 */
char *GetCharArrayAt(string_t *string, const size_t index) {
  assert(string != NULL && index < string->length);
  if (string->length < string->size) {
    string->chars[string->length] = '\0';
  }
  return &string->chars[index];
}

/**
 * Sprawdza, czy wskaźnik na string nie jest równy @p NULL. Następnie ustawia
 * długość stringa na @p 0 -- sprawi to, że wczytywanie znaków do tego stringa
 * rozpocznie się ponownie od indeksu równego zeru.
 */
void ResetString(string_t *string) {
  assert(string != NULL);
  string->length = 0;
}

/**
 * Sprawdza, czy wskaźnik na string nie jest równy @p NULL. Jeśli została
 * zaalokowana tablica typu @p char, zwalnia ją, a następnie przywraca
 * stringa do domyślnych wartości -- jego długość, rozmiar są równe zeru.
 */
void DestroyString(string_t *string) {
  assert(string != NULL);

  if (string->chars != NULL) {
    free(string->chars);
  }
  string->chars = NULL;
  string->length = 0;
  string->size = 0;
}



