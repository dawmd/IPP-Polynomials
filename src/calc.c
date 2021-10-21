/** @file
  Kalkulator wielomianów wielu zmiennych działający w czasie rzeczywistym
  w oparciu o polecenia wydawane przez użytkownika. Linia przedstawiająca
  wielomian -- w postaci wielomianu stałego lub ciągu jednomianów -- dodaje
  wielomian do stosu, na których może następnie wywoływać kolejne operacje.
  Linia może odpowiadać także jednemu z poleceń (nazwa polecenia -- funkcja):
  1) "ZERO" -- dodanie wielomianu zerowego do stosu,
  2) "IS_COEFF" -- sprawdzenie, czy wielomian na wierzchołku stosu jest stały,
  3) "IS_ZERO" -- sprawdzenie, czy wielomian na wierzchołku stosu jest zerowy,
  4) "CLONE" -- dodanie kopii wielomianu z wierzchołku stosu do stosu,
  5) "ADD" -- zastąpienie dwóch wielomianów z wierzchołku stosu ich sumą,
  6) "MUL" -- zastąpienie dwóch wielomianów z wierzchołku stosu ich iloczynem,
  7) "NEG" -- zastępienie wielomianu z wierzchołku stosu wielomianem do niego
  przeciwnym,
  8) "SUB" -- zastąpienie dwóch wielomianów z wierzchołku stosu ich różnicą,
  9) "IS_EQ" -- sprawdzenie, czy dwa wielomiany z wierzchołku stosu są równe,
  10) "DEG" -- wypisanie stopnia wielomianu z wierzchołka stosu,
  11) "PRINT" -- wypisanie wielomianu z wierzchołka stosu,
  12) "POP" -- usunięcie wielomianu z wierzchołka stosu,
  13) "DEG_BY" -- wypisanie stopnia wielomianu z wierzchołka stosu w zależności
  od danej zmiennej,
  14) "AT" -- zastąpienie wielomianu z wierzchołku stosu jego wartością
  w danym punkcie,
  15) COMPOSE @p k -- usuwa ze stosu wielomianów @f$k+1@f$ wielomianów
  i pierwszy z nich składa z pozostałymi, ułożonymi w odwrotnej kolejności
  do tej, z jaką są ściągane ze stosu.
  
  @author Dawid Mędrek
  @date 2021
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <limits.h>
#include <errno.h>
#include <ctype.h>

#include "poly.h"
#include "polystack.h"
#include "newstring.h"
#include "monovector.h"


/**
 * To jest typ enumeracyjny reprezentujący błędy związane
 * z wykonaniem poleceń.
 */
typedef enum {
  InvalidCommandName, ///< nieprawidłowa nazwa polecenia
  NoParam, ///< brak parametru dla polecenia
  NoDegByParam, ///< brak parametru dla polecenia @p DEG_BY
  NoAtParam, ///< brak parametru dla polecenia @p AT
  NoComposeParam, ///< brak parametru lub jego brak dla polecenia @p COMPOSE
  StackUnderflow, ///< brak wystarczającej liczby wielomianów na stosie
  ParsingErr, ///< błąd podczas parsowania wielomianu
  NoError ///< brak błędu
} InputErr;


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


//////////////////////////////////////////
//                                      //
//        Operacje odpowiadające        //
//       poszczególnym poleceniem       //
//                                      //
//////////////////////////////////////////


/**
 * Dodaje wielomian zerowy do stosu wielomianów i zwraca brak błędu. Funkcja
 * zakłada, że przekazany wskaźnik wskazuje na istniejący i poprawny stos.
 * @param[in] stack : stos wielomianów
 * @return @p NoError
 */
static inline InputErr ExecuteZero(stack_t *stack) {
  PushPoly(stack, PolyZero());
  return NoError;
}

/**
 * Sprawdza, czy wielomian na wierzchołku przekazanego stosu jest wielomianem
 * stałym. Jeśli jest -- wyświetla @p 1; jeśli nie -- wyświetla @p 0.
 * Jeżeli stos jest jednak pusty, funkcja nie robi nic i zwraca błąd
 * @p StackUnderflow.
 * Funkcja zakłada, że przekazany wskaźnik wskazuje na istniejący
 * i poprawny stos.
 * @param[in] stack : stos wielomianów
 * @return @p StackUnderflow w przypadku, gdy stos jest pusty; @p NoError
 * w przeciwnym przypadku
 */
static inline InputErr ExecuteIsCoeff(stack_t *stack) {
  if (StackIsEmpty(stack)) {
    return StackUnderflow;
  }

  Poly topPoly = ShowTop(stack);
  if (PolyIsCoeff(&topPoly)) {
    printf("1\n");
  }
  else {
    printf("0\n");
  }

  return NoError;
}

/**
 * Sprawdza, czy wielomian na wierzchołku przekazanego stosu jest wielomianem
 * zerowym. Jeśli jest -- wyświetla @p 1; jeśli nie -- wyświetla @p 0.
 * Jeżeli stos jest jednak pusty, funkcja nie robi nic i zwraca błąd
 * @p StackUnderflow.
 * Funkcja zakłada, że przekazany wskaźnik wskazuje na istniejący
 * i poprawny stos.
 * @param[in] stack : stos wielomianów
 * @return @p StackUnderflow w przypadku, gdy stos jest pusty; @p NoError
 * w przeciwnym przypadku
 */
static inline InputErr ExecuteIsZero(stack_t *stack) {
  if (StackIsEmpty(stack)) {
    return StackUnderflow;
  }

  Poly topPoly = ShowTop(stack);
  if (PolyIsZero(&topPoly)) {
    printf("1\n");
  }
  else {
    printf("0\n");
  }

  return NoError;
}

/**
 * Dodaje kopię wielomianu na wierzchołku stosu do tego stosu. Jeśli jednak
 * stos jest pusty, zwraca @p StackUnderflow i nie robi nic. W przeciwnym
 * wypadku zwraca @p NoError. Funkcja zakłada, że przekazany wskaźnik
 * na stos wskazuje na istniejący i poprawny stos.
 * @param[in] stack : stos wielomianów
 * @return @p StackUnderflow w przypadku, gdy stos jest pusty; @p NoError
 * w przeciwnym przypadku
 */
static inline InputErr ExecuteClone(stack_t *stack) {
  if (StackIsEmpty(stack)) {
    return StackUnderflow;
  }

  Poly topPoly = ShowTop(stack);
  PushPoly(stack, PolyClone(&topPoly));
  return NoError;
}

/**
 * Ściąga z przekazanego stosu wielomianów dwa wielomiany i dodaje ich sumę
 * do tego stosu. Wykorzystane w tym celu wielomiany są następnie usuwane
 * z pamięci. Jeżeli jednak na stosie nie ma wymaganej do tej operacji liczby
 * wielomianów, funkcja zwraca @p StackUnderflow i nie robi nic. W przeciwnym
 * wypadku zwraca @p NoError. Funkcja zakłada także, że przekazany wskaźnik
 * na stos wskazuje na istniejący i poprawny stos.
 * @param[in] stack : stos wielomianów
 * @return @p StackUnderflow w przypadku, gdy stos nie zawiera co najmniej
 * dwóch wielomianów; @p NoError w przeciwnym przypadku
 */
static inline InputErr ExecuteAdd(stack_t *stack) {
  if (StackSize(stack) < 2) {
    return StackUnderflow;
  }
  else {
    Poly p1 = TakePoly(stack);
    Poly p2 = TakePoly(stack);
    PushPoly(stack, PolyAdd(&p1, &p2));
    PolyDestroy(&p1);
    PolyDestroy(&p2);
    return NoError;
  }
}

/**
 * Ściąga z przekazanego stosu wielomianów dwa wielomiany i dodaje ich iloczyn
 * do tego stosu. Wykorzystane w tym celu wielomiany są następnie usuwane
 * z pamięci. Jeżeli jednak na stosie nie ma wymaganej do tej operacji liczby
 * wielomianów, funkcja zwraca @p StackUnderflow i nie robi nic. W przeciwnym
 * wypadku zwraca @p NoError. Funkcja zakłada także, że przekazany wskaźnik
 * na stos wskazuje na istniejący i poprawny stos.
 * @param[in] stack : stos wielomianów
 * @return @p StackUnderflow w przypadku, gdy stos nie zawiera co najmniej
 * dwóch wielomianów; @p NoError w przeciwnym przypadku
 */
static inline InputErr ExecuteMul(stack_t *stack) {
  if (StackSize(stack) < 2) {
    return StackUnderflow;
  }
  else {
    Poly p1 = TakePoly(stack);
    Poly p2 = TakePoly(stack);
    PushPoly(stack, PolyMul(&p1, &p2));
    PolyDestroy(&p1);
    PolyDestroy(&p2);
    return NoError;
  }
}

/**
 * Ściąga z przekazanego stosu jeden wielomian, oblicza wielomian do niego
 * przeciwny i dodaje do tego stosu. Oryginalny wielomian jest następnie
 * usuwany z pamięci. Jeśli jednak stos jest pusty, funkcja zwraca
 * @p StackUnderflow i nie robi nic. W przeciwnym wypadku zwraca
 * @p NoError. Funkcja zakłada także, że przekazany wskaźnik
 * na stos wskazuje na istniejący i poprawny stos.
 * @param[in] stack : stos wielomianów
 * @return @p StackUnderflow w przypadku, gdy stos jest pusty; @p NoError
 * w przeciwnym przypadku
 */
static inline InputErr ExecuteNeg(stack_t *stack) {
  if (StackIsEmpty(stack)) {
    return StackUnderflow;
  }
  else {
    Poly p = TakePoly(stack);
    PushPoly(stack, PolyNeg(&p));
    PolyDestroy(&p);
    return NoError;
  }
}

/**
 * Ściąga z przekazanego stosu wielomianów dwa wielomiany i dodaje ich różnicę
 * do tego stosu. Wykorzystane w tym celu wielomiany są następnie usuwane
 * z pamięci. Jeżeli jednak na stosie nie ma wymaganej do tej operacji liczby
 * wielomianów, funkcja zwraca @p StackUnderflow i nie robi nic. W przeciwnym
 * wypadku zwraca @p NoError. Funkcja zakłada także, że przekazany wskaźnik
 * na stos wskazuje na istniejący i poprawny stos.
 * @param[in] stack : stos wielomianów
 * @return @p StackUnderflow w przypadku, gdy stos nie zawiera co najmniej
 * dwóch wielomianów; @p NoError w przeciwnym przypadku
 */
static inline InputErr ExecuteSub(stack_t *stack) {
  if (StackSize(stack) < 2) {
    return StackUnderflow;
  }
  else {
    Poly p1 = TakePoly(stack);
    Poly p2 = TakePoly(stack);
    PushPoly(stack, PolySub(&p1, &p2));
    PolyDestroy(&p1);
    PolyDestroy(&p2);
    return NoError;
  }
}

/**
 * Sprawdza, czy dwa wielomiany na wierzchołku przekazanego stosu są równe.
 * Jeśli tak -- wyświetla @p 1; w przeciwnym razie wyświetla @p 0. Jeśli jednak
 * stos nie zawiera co najmniej dwóch wielomianów, funkcja nie robi nic
 * i zwraca @p StackUnderflow. Inaczej zwracaną wartością jest @p NoError.
 * Funkcja zakłada, że przekazany wskaźnik na stos wskazuje na istniejący
 * i poprawny stos wielomianów.
 * @param[in] stack : stos wielomianów
 * @return @p StackUnderflow, jeśli przekazany stos nie zawiera co najmniej
 * dwóch wielomianów; @p NoError w przeciwnym razie
 */
static inline InputErr ExecuteIsEq(stack_t *stack) {
  if (StackSize(stack) < 2) {
    return StackUnderflow;
  }
  else {
    Poly p1 = TakePoly(stack);
    Poly p2 = ShowTop(stack);
    PushPoly(stack, p1);

    if (PolyIsEq(&p1, &p2)) {
      printf("%d\n", 1);
    }
    else {
      printf("%d\n", 0);
    }

    return NoError;
  }
}

/**
 * Funkcja wyświetla stopień wielomianu z wierzchołku przekazanego stosu
 * wielomianów i zwraca @p NoError. Jeśli jednak stos jest pusty, funkcja
 * nie robi nic i zwraca @p StackUnderflow. Funkcja zakłada, że przekazany
 * wskaźnik na stos wskazuje na istniejący i poprawny stos wielomianów.
 * @param[in] stack : stos wielomianów
 * @return @p StackUnderflow, jeśli przekazany stos jest pusty;
 * w przeciwnym razie @p NoError
 */
static inline InputErr ExecuteDeg(stack_t *stack) {
  if (StackIsEmpty(stack)) {
    return StackUnderflow;
  }
  else {
    Poly p = ShowTop(stack);
    poly_exp_t polyDeg = PolyDeg(&p);
    printf("%d\n", (int) polyDeg);
    return NoError;
  }
}

/**
 * Funkcja wyświetla stopień wielomianu z wierzchołku przekazanego stosu
 * wielomianów ze względu na daną zmienną i zwraca @p NoError. Jeśli jednak
 * stos jest pusty, funkcja nie robi nic i zwraca @p StackUnderflow.
 * Funkcja zakłada, że przekazany wskaźnik na stos wskazuje na istniejący
 * i poprawny stos wielomianów.
 * @param[in] stack : stos wielomianów
 * @param[in] idx : indeks zmiennej, względem której funkcja rozpatruje stopień
 * wielomianu
 * @return @p StackUnderflow, jeśli przekazany stos jest pusty;
 * w przeciwnym razie @p NoError
 */
static inline InputErr ExecuteDegBy(stack_t *stack, size_t idx) {
  if (StackIsEmpty(stack)) {
    return StackUnderflow;
  }
  else {
    Poly p = ShowTop(stack);
    poly_exp_t polyDeg = PolyDegBy(&p, idx);
    printf("%d\n", (int) polyDeg);
    return NoError;
  }
}

/**
 * Ściąga wielomian z wierzchołka przekazanego stosu wielomianów, oblicza
 * jego wartość w danym punkcie i wynik wstawia na stos. Zwraca następnie
 * @p NoError. Oryginalny wielomian jest usuwany z pamięci. Jeśli jednak
 * przekazany stos jest pusty, funkcja nie robi nic i zwraca @p StackUnderflow.
 * Funkcja zakłada, że wskaźnik na stos wielomianów wskazuje na istniejący
 * i poprawny stos.
 * @param[in] stack : stos wielomianów
 * @param[in] x : punkt, w którym zostanie obliczona wartość wielomianu
 * @return @p StackUnderflow, jeśli przekazany stos jest pusty;
 * w przeciwnym razie @p NoError
 */
static inline InputErr ExecuteAt(stack_t *stack, poly_coeff_t x) {
  if (StackIsEmpty(stack)) {
    return StackUnderflow;
  }
  else {
    Poly p = TakePoly(stack);
    Poly newPoly = PolyAt(&p, x);
    PolyDestroy(&p);
    PushPoly(stack, newPoly);
    return NoError;
  }
}

/**
 * Wyświeta przekazany wielomian. Funkcja zakłada, że przekazany wskaźnik
 * wskazuje na istniejący i poprawny wielomian.
 * @param[in] p : wielomian do wyświetlenia
 */
static inline void AuxPrintPoly(Poly *p) {
  // Jeśli wielomian jest stały, wyświetla jego wartość
  if (PolyIsCoeff(p)) {
    printf("%ld", p->coeff);
  }
  else {
    // Jeśli wielomian nie jest stały, wypisuje go w postaci jednomianów
    for (size_t i = 0; i < p->size; i++) {
      printf("(");
      // Wyświetla wielomian w obecnie rozważanym jednomianie
      AuxPrintPoly(&p->arr[i].p);
      // Wyświetla wykładnik jednomianu
      printf(",%d)", MonoGetExp(&p->arr[i]));

      if (i + 1 < p->size) {
        printf("+");
      }
    }
  }
}

/**
 * Wyświetla w linii przekazany wielomian i zwraca @p NoError. Funkcja
 * zakłada, że przekazany wskaźnik wskazuje na istniejący i poprawny wielomian.
 * @param[in] p : wielomian do wyświetlenia
 * @return @p NoError
 */
static inline InputErr PrintPoly(Poly *p) {
  // Wyświetla wielomian
  AuxPrintPoly(p);
  // Przechodzi do nowej linii
  printf("\n");
  return NoError;
}

/**
 * Wyświetla wielomian z wierzchołka przekazanego stosu wielomianów i zwraca
 * @p NoError. Jeśli jednak stos ten jest pusty, funkcja nie robi nic
 * i zwraca @p StackUnderflow. Funkcja zakłada, że przekazany wskaźnik na stos
 * wskazuje na istniejący i poprawny stos wielomianów
 * @param[in] stack : stos wielomianów
 * @return @p StackUnderflow, jeśli stos jest pusty; @p NoError w przeciwnym
 * razie
 */
static inline InputErr ExecutePrint(stack_t *stack) {
  if (StackIsEmpty(stack)) {
    return StackUnderflow;
  }
  else {
    Poly p = ShowTop(stack);
    PrintPoly(&p);
    return NoError;
  }
}

/**
 * Usuwa wielomian z wierzchołka przekazanego stosu wielomianów i zwraca
 * @p NoError. Jeśli jednak stos ten jest pusty, funkcja nie robi nic
 * i zwraca @p StackUnderflow. Funkcja zakłada, że przekazany wskaźnik na stos
 * wskazuje na istniejący i poprawny stos wielomianów.
 * @param[in] stack : stos wielomianów
 * @return @p StackUnderflow, jeśli stos jest pusty; @p NoError w przeciwnym
 * razie
 */
static inline InputErr ExecutePop(stack_t *stack) {
  if (StackIsEmpty(stack)) {
    return StackUnderflow;
  }
  else {
    Poly p = TakePoly(stack);
    PolyDestroy(&p);
    return NoError;
  }
}

/**
 * Ściąga ze stosu @p polysNum+1 wielomianów ze stosu, a nastepnie pierwszy
 * z nich składa z pozostałymi za pomocą funkcji @p PolyCompose. Wynik dodaje
 * na stos i zwraca @p NoError. Jeśli jednak nie ma na stosie wymaganej liczby
 * wielomianów, potrzebnej do wykonania operacji, funkcja nie robi nic
 * i zwraca @p StackUnderflow. Funkcja zakłada, że przekazane wskaźniki
 * wskazują na istniejące i poprawne struktury danych.
 * @param[in] stack : stos wielomianów
 * @param[in] polysNum : liczba wielomianów, z którymi będzie składany
 * wielomian z wierzchołka stosu
 */
static inline InputErr ExecuteCompose(stack_t *stack, size_t polysNum) {
  // Muszą być oba warunki, aby uniknąć overflow
  if (StackIsEmpty(stack) || StackSize(stack) - 1 < polysNum) {
    return StackUnderflow;
  }
  else {
    Poly p = TakePoly(stack);
    // Wynik składania wielomianów
    Poly result;

    if (polysNum > 0) {
      // Tablica wielomianów, z którymi zostanie złożony wielomian `p`
      Poly *polys = malloc(polysNum * sizeof(Poly));

      CHECK_PTR(polys);

      for (size_t i = 0; i < polysNum; i++) {
        polys[polysNum - 1 - i] = TakePoly(stack);
      }

      result = PolyCompose(&p, polysNum, polys);

      for (size_t i = 0; i < polysNum; i++) {
        PolyDestroy(&polys[i]);
      }

      free(polys);
    }
    else {
      result = PolyCompose(&p, 0, NULL);
    }

    PolyDestroy(&p);
    PushPoly(stack, result);
    return NoError;
  }
}


//////////////////////////////////////////
//                                      //
//       Charakteryzacja poleceń        //
//                                      //
//////////////////////////////////////////


/** To jest typ enumeracyjny reprezentujący typy poleceń
    oferowanych przez kalkulator */
typedef enum {
  ZERO,
  IS_COEFF,
  IS_ZERO,
  CLONE,
  ADD,
  MUL,
  NEG,
  SUB,
  IS_EQ,
  DEG,
  PRINT,
  POP,
  DEG_BY,
  AT,
  COMPOSE,
  INVALID_COMMAND
} CommandType;

/** To jest struktura reprezentująca polecenie nie przyjmujące argumentu */
typedef struct {
  const CommandType type; ///< typ polecenia
  const char *name; ///< ciąg znaków odpowiadający poleceniu
} NonParamCommand;

/** Liczba poleceń nie przyjmujących żadnych argumentów */
#define NUM_OF_NON_PARAM_COMMANDS 12

/** To jest tablica zawierająca charakteryzacje poleceń, które
    nie przyjmują argumentów */
static const NonParamCommand NonParamCommands[NUM_OF_NON_PARAM_COMMANDS] = {
  { .type = ZERO,      .name = "ZERO"     },
  { .type = IS_COEFF,  .name = "IS_COEFF" },
  { .type = IS_ZERO,   .name = "IS_ZERO"  },
  { .type = CLONE,     .name = "CLONE"    },
  { .type = ADD,       .name = "ADD"      },
  { .type = MUL,       .name = "MUL"      },
  { .type = NEG,       .name = "NEG"      },
  { .type = SUB,       .name = "SUB"      },
  { .type = IS_EQ,     .name = "IS_EQ"    },
  { .type = DEG,       .name = "DEG"      },
  { .type = PRINT,     .name = "PRINT"    },
  { .type = POP,       .name = "POP"      }
};

/** To jest struktura reprezentująca polecenie przyjmujące
    co najmniej jeden argument */
typedef struct {
  const CommandType type; ///< typ polecenia
  const char *name; ///< ciąg znaków odpowiadający poleceniu
  const int nameLength; ///< długość nazwy polecenia
} ParamCommand;

/** Liczba poleceń przyjmujących co najmniej jeden parametr */
#define NUM_OF_PARAM_COMMANDS 3

/** To jest tablica zawierająca charakteryzacje poleceń, które
    przyjmują co najmniej jeden argument */
static const ParamCommand ParamCommands[NUM_OF_PARAM_COMMANDS] = {
  { .type = DEG_BY,  .name = "DEG_BY",  .nameLength = 6 },
  { .type = AT,      .name = "AT",      .nameLength = 2 },
  { .type = COMPOSE, .name = "COMPOSE", .nameLength = 7 }
};


//////////////////////////////////////////
//                                      //
//        Definiowanie polecenia        //
//                                      //
//////////////////////////////////////////


/**
 * Porównuje string z poleceniem o indeksie @p commandNumber w tablicy
 * poleceń przyjmujących argument. Jeśli string nie pasuje do porównywanego
 * polecenia, zwraca @p false. W przeciwnym razie zwraca @p true.
 * @param[in] line : string reprezentujący polecenie
 * @param[in] commandNumber : indeks porównywanego polecenia z tablicy
 * poleceń przyjmujących argument
 * @return Jeśli string nie odpowiada danemu poleceniu, zwraca @p false;
 * W przeciwnym razie zwraca @p true.
 */
static inline bool IsParamCommand(string_t *line, const int commandNumber) {
  const size_t commandLength = ParamCommands[commandNumber].nameLength;
  // Jeśli string jest krótszy od nazwy polecenia, nie może go reprezentować
  if (StringLength(line) < commandLength) {
    return false;
  }

  // Porównanie znaków, z których składają się string i dane polecenie
  for (size_t i = 0; i < commandLength; i++) {
    if (CharAt(line, i) != ParamCommands[commandNumber].name[i]) {
      return false;
    }
  }

  // Niepoprawna nazwa polecenia (aby uniknąć pomyłki, gdyby występowałby
  // dwa polecenia z parametrem, wśród których nazwa jednego jest początkiem
  // drugiego, np. "POLECENIE" oraz "POLECENIEE")
  if (StringLength(line) == commandLength + 1
      && isalpha(CharAt(line, commandLength))) {
      return false;
  }

  // String może przedstawiać dane polecenie
  return true;
}

/**
 * Sprawdza, czy polecenie jest operacją przyjmującą argument
 * i zwraca jego typ.
 * @param[in] line : polecenie
 * @return Typ polecenia
 */
static inline CommandType DefineParamCommand(string_t *line) {
  // Porównanie polecenia z operacjami przyjmującymi argument
  for (int i = 0; i < NUM_OF_PARAM_COMMANDS; i++) {
    if (IsParamCommand(line, i)) {
      return ParamCommands[i].type;
    }
  }

  // Polecenie nie jest żadną z operacji przyjmujących argument
  // -- jest niepoprawnym poleceniem
  return INVALID_COMMAND;
}

/**
 * Określa typ polecenia i zwraca go.
 * @param[in] line : polecenie
 * @return Typ polecenia
 */
static inline CommandType DefineCommand(string_t *line) {
  // Sprawdzenie, czy polecenie jest operacją bezargumentową
  for (int i = 0; i < NUM_OF_NON_PARAM_COMMANDS; i++) {
    if (CompareTo(line, NonParamCommands[i].name)) {
      return NonParamCommands[i].type;
    }
  }

  // Sprawdzenie, czy polecenie jest operacją przyjmującą argument
  return DefineParamCommand(line);
}


//////////////////////////////////////////
//                                      //
//        Polecenia przyjmujące         //
//              parametr                //
//                                      //
//////////////////////////////////////////


/**
 * Liczba niedozwolonych białych znaków pomiędzy
 * poleceniem a jego argumentem.
 */
#define NUM_OF_INVALID_WHITESPACE_CHARS 5

/**
 * Tablica niedozwolonych białych znaków pomiędy
 * poleceniem a jego argumentem -- są to te znaki, których
 * obecność wywoła błąd związany z brakiem lub niepoprawnym parametrem
 * polecenia, a nie niepoprawnym poleceniem.
 */
static const char INVALID_WHITESPACE_CHARS[NUM_OF_INVALID_WHITESPACE_CHARS] = {
  '\t',
  '\n',
  '\v',
  '\f',
  '\r'
};


/**
 * Przeprowadza wstępną analizę polecenia przyjmującego parametr. W oparciu
 * o typ polecenia porównuje kolejne znaki odpowiadające nazwie polecenia,
 * a następnie, czy argument został oddzielony poprawnym białym znakiem.
 * Jeśli funkcja nie napotka żadnych błędów, ustawia wartość odpowiadającą
 * wskaźnikowi @p arrayPtr na tablicę, składającą się ze wszystkich
 * pozostałych -- jeszcze nieprzeanalizowanych znaków, będących częścią
 * oryginalnego stringa. Zwraca także @p NoError. Jeśli jednak funkcja napotka
 * jakikolwiek błąd, przerywa działanie i zwraca odpowiedni komunikat.
 * W przypadku błędnej nazwy polecenia -- @p InvalidCommandName; w przypadku
 * braku argumentu (np. gdy string nagle się zakończył) lub argument
 * został oddzielony niedozwolonym białym znakiem -- @p NoParam.
 * @param[in] line : string zawierający polecenie
 * @param[in] arrayPtr : wskaźnik ustawiany przez funkcję na pierwszy
 * nieprzejrzany znak w ciągu charów
 * @param[in] commType : typ polecenia
 * @return W przypadku braku błędów -- @p NoError; w przypadku niepoprawnego
 * polecenia -- @p InvalidCommandName; w przypadku problemu z argumentem
 * -- @p NoParam
 */
static inline InputErr InitialParamCommCheck(string_t *line, char **arrayPtr,
                                             const CommandType commType) {
  // Indeks polecenia w tablicy sparametryzowanych poleceń
  // odpowiadający `commType`
  int commandID;
  for (int i = 0; i < NUM_OF_PARAM_COMMANDS; i++) {
    if (commType == ParamCommands[i].type) {
      commandID = i;
      break;
    }
  }

  // Długość nazwy polecenia
  const size_t commandLen = ParamCommands[commandID].nameLength;

  // Jeśli polecenie jest tej samej długości co jego nazwa,
  // to nie ma parametru -- błąd
  if (StringLength(line) == commandLen) {
    return NoParam;
  }

  // Sprawdzanie, czy argument nie jest oddzielony od nazwy polecenia
  // niedozwolonym białym znakiem; jeśli tak -- błąd
  for (int i = 0; i < NUM_OF_INVALID_WHITESPACE_CHARS; i++) {
    if (CharAt(line, commandLen) == INVALID_WHITESPACE_CHARS[i]) {
      return NoParam;
    }
  }

  // Niedozwolony znak pomiędzy nazwą polecenia a argumentem -- błąd
  if (CharAt(line, commandLen) != ' ') {
    return InvalidCommandName;
  }

  // Brak argumentu
  if (StringLength(line) == commandLen + 1) {
    return NoParam;
  }

  // Poprawne polecenie -- ustawienie wskaźnika na pierwszy znak argumentu
  *arrayPtr = GetCharArrayAt(line, commandLen + 1);
  return NoError;
}

/**
 * Wykonuje polecenie @p DEG_BY -- wypisuje stopień wielomianu z wierzchołku
 * przekazanego stosu ze względu na daną zmienną i zwraca @p NoError.
 * W przypadku napotkania błędu funkcja nie robi nic i zwraca komunikat
 * o błędzie: @p NoDegByParam -- w przypadku błędu związanego z parametrem
 * operacji, @p InvalidCommandName -- w przypadku błędu związanego z nazwą
 * polecenia. Funkcja zakłada, że przekazane wskaźniki na stos wielomianów
 * i string wskazują na istniejące i poprawne struktury danych.
 * @param[in] stack : stos wielomianów
 * @param[in] line : polecenie
 * @return W przypadku sukcesu -- @p NoError; w przypadku błędu parametru
 * polecenia -- @p NoDegByParam; w przypadku nieprawidłowej nazwy polecenia
 * -- @p InvalidCommandName
 */
static inline InputErr RunDegBy(stack_t *stack, string_t *line) {
  // Wskaźnik na pierwszy znak odpowiadający argumentowi polecenia
  char *arg = NULL;
  // Wstępnie sprawdzenie poprawności polecenia
  switch (InitialParamCommCheck(line, &arg, DEG_BY)) {
    // Błąd związany z argumentem polecenia
    case NoParam:
      return NoDegByParam;
    // Błąd związany z poleceniem
    case InvalidCommandName:
      return InvalidCommandName;
    // Sukces -- funkcja przechodzi do sprawdzenia argumentu
    case NoError:
      break;
    // Błąd funkcji `InitialParamCommCheck`
    default:
      assert(false);
  }

  // Argument musi być liczbą nieujemną
  if (!isdigit(arg[0])) {
    return NoDegByParam;
  }

  // Pomocniczy wskaźnik
  char *ptr = NULL;

  // Konwertowanie argumentu polecenia na liczbę typu size_t
  size_t num = strtoul(arg, &ptr, 10);
  // Argument poza akceptowalnym zakresem lub niedozwolone znaki w argumencie
  // -- błąd
  if (errno == ERANGE || *ptr != '\0') {
    return NoDegByParam;
  }
  // Poprawny argument. Wykonanie operacji
  else {
    // Wskaźnik na początek polecenia
    char *lineStart = GetCharArrayAt(line, 0);
    // W argumencie znajdują się nieprzejrzane znaki -- a więc
    // argument zawiera niedozwolone znaki
    if ((ptr - lineStart) / sizeof(char) < StringLength(line)) {
      return NoDegByParam;
    }
    // Poprawny argument. Wykonanie operacji
    else {
      return ExecuteDegBy(stack, num);
    }
  }
}

/**
 * Wykonuje polecenie @p AT -- zastępuje wielomian z wierzchołka przekazanego
 * stosu jego wartością w danym punkcie i zwraca @p NoError.
 * W przypadku napotkania błędu funkcja nie robi nic i zwraca komunikat
 * o błędzie: @p NoAtParam -- w przypadku błędu związanego z parametrem
 * operacji, @p InvalidCommandName -- w przypadku błędu związanego z nazwą
 * polecenia. Funkcja zakłada, że przekazane wskaźniki na stos wielomianów
 * i string wskazują na istniejące i poprawne struktury danych.
 * @param[in] stack : stos wielomianów
 * @param[in] line : polecenie
 * @return W przypadku sukcesu -- @p NoError; w przypadku błędu parametru
 * polecenia -- @p NoAtParam; w przypadku nieprawidłowej nazwy polecenia
 * -- @p InvalidCommandName
 */
static inline InputErr RunAt(stack_t *stack, string_t *line) {
  // Wskaźnik na pierwszy znak argumentu
  char *arg = NULL;
  // Wstępne sprawdzenie poprawności polecenia
  switch (InitialParamCommCheck(line, &arg, AT)) {
    // Błąd związany z argumentem polecenia
    case NoParam:
      return NoAtParam;
    // Błąd związany z poleceniem
    case InvalidCommandName:
      return InvalidCommandName;
    // Sukces -- funkcja przechodzi do sprawdzenia argumentu
    case NoError:
      break;
    // Błąd funkcji pomocniczej
    default:
      assert(false);
  }

  // Argument musi być liczbą
  if (!isdigit(arg[0]) && arg[0] != '-') {
    return NoAtParam;
  }

  // Pomocniczy wskaźnik
  char *ptr = NULL;

  // Konwertowanie argumentu na liczbę typu long (poly_coeff_t)
  long num = strtol(arg, &ptr, 10);
  // Argument poza akceptowalnym zakresem lub niedozwolone znaki w argumencie
  // -- błąd
  if (errno == ERANGE || *ptr != '\0') {
    return NoAtParam;
  }
  else {
    // Wskaźnik na początek polecenia
    char *lineStart = GetCharArrayAt(line, 0);
    // W argumencie znajdują się nieprzejrzane znaki -- a więc
    // argument zawiera niedozwolone znaki
    if ((ptr - lineStart) / sizeof(char) < StringLength(line)) {
      return NoAtParam;
    }
    // Poprawny argument. Wykonanie operacji
    else {
      return ExecuteAt(stack, (poly_coeff_t) num);
    }
  }
}

/**
 * Wykonuje polecenie @p COMPOSE -- zastępuje wielomian z wierzchołka
 * przekazanego stosu jego złożeniem z innymi ze stosu i zwraca @p NoError.
 * W przypadku napotkania błędu funkcja nie robi nic i zwraca komunikat
 * o błędzie: @p NoComposeParam -- w przypadku błędu związanego z parametrem
 * operacji, @p InvalidCommandName -- w przypadku błędu związanego z nazwą
 * polecenia. Funkcja zakłada, że przekazane wskaźniki na stos wielomianów
 * i string wskazują na istniejące i poprawne struktury danych.
 * @param[in] stack : stos wielomianów
 * @param[in] line : polecenie
 * @return W przypadku sukcesu -- @p NoError; w przypadku błędu parametru
 * polecenia -- @p NoComposeParam; w przypadku nieprawidłowej nazwy polecenia
 * -- @p InvalidCommandName
 */
static inline InputErr RunCompose(stack_t *stack, string_t *line) {
  // Wskaźnik na pierwszy znak odpowiadający argumentowi polecenia
  char *arg = NULL;
  // Wstępnie sprawdzenie poprawności polecenia
  switch (InitialParamCommCheck(line, &arg, COMPOSE)) {
    // Błąd związany z argumentem polecenia
    case NoParam:
      return NoComposeParam;
    // Błąd związany z poleceniem
    case InvalidCommandName:
      return InvalidCommandName;
    // Sukces -- funkcja przechodzi do sprawdzenia argumentu
    case NoError:
      break;
    // Błąd funkcji `InitialParamCommCheck`
    default:
      assert(false);
  }

  // Argument musi być liczbą nieujemną
  if (!isdigit(arg[0])) {
    return NoComposeParam;
  }

  // Pomocniczy wskaźnik
  char *ptr = NULL;

  // Konwertowanie argumentu polecenia na liczbę typu size_t
  size_t num = strtoul(arg, &ptr, 10);
  // Argument poza akceptowalnym zakresem lub niedozwolone znaki w argumencie
  // -- błąd
  if (errno == ERANGE || *ptr != '\0') {
    return NoComposeParam;
  }
  // Poprawny argument. Wykonanie operacji
  else {
    // Wskaźnik na początek polecenia
    char *lineStart = GetCharArrayAt(line, 0);
    // W argumencie znajdują się nieprzejrzane znaki -- a więc
    // argument zawiera niedozwolone znaki
    if ((ptr - lineStart) / sizeof(char) < StringLength(line)) {
      return NoComposeParam;
    }
    // Poprawny argument. Wykonanie operacji
    else {
      return ExecuteCompose(stack, num);
    }
  }
}


//////////////////////////////////////////
//                                      //
//        Określenie i wykonanie        //
//              polecenia               //
//                                      //
//////////////////////////////////////////


/**
 * Określa typ polecenia na wielomianach wprowadzonego przez użytkownika
 * i wykonuje je. Zwraca odpowiedni komunikat o napotkach błędach.
 * Funkcja zakłada, że wskaźniki na stos wielomianów @p stack
 * i stringa @p line wskazują na istniejące i poprawne struktury danych.
 * @param[in] stack : stos wielomianów
 * @param[in] line : wprowadzone polecenie
 * @return komunikat o napotkanym błędzie lub jego braku
 */
static inline InputErr ExecuteCommand(stack_t *stack, string_t *line) {
  // Określa typ polecenia
  switch (DefineCommand(line)) {
    // Dodaje wielomian zerowy do stosu wielomianów
    case ZERO:
      return ExecuteZero(stack);
    // Sprawdza, czy wielomian na wierzchołku stosu jest stały
    case IS_COEFF:
      return ExecuteIsCoeff(stack);
    // Sprawdza, czy wielomian na wierzchołku stosu jest zerowy
    case IS_ZERO:
      return ExecuteIsZero(stack);
    // Wstawia na stos kopię wielomianu z jego wierzchołka
    case CLONE:
      return ExecuteClone(stack);
    // Zastępuje dwa wielomiany z wierzchołka stosu ich sumą
    case ADD:
      return ExecuteAdd(stack);
    // Zastępuje dwa wielomiany z wierzchołka stosu ich iloczynem
    case MUL:
      return ExecuteMul(stack);
    // Zastępuje wielomian z wierzchołka stosu wielomianem do niego przeciwnym
    case NEG:
      return ExecuteNeg(stack);
    // Zastępuje dwa wielomiany z wierzchołka stosu ich różnicą
    case SUB:
      return ExecuteSub(stack);
    // Sprawdza, czy dwa wielomiany z wierzchołka stosu są równe
    case IS_EQ:
      return ExecuteIsEq(stack);
    // Wypisuje stopień wielomianu z wierzchołka stosu
    case DEG:
      return ExecuteDeg(stack);
    // Wypisuje wielomian z wierzchołka stosu
    case PRINT:
      return ExecutePrint(stack);
    // Usuwa wielomian z wierzchołka stosu
    case POP:
      return ExecutePop(stack);
    // Wyświetla stopień wielomianu z wierzchołka stosu w zależności od
    // danej zmiennej
    case DEG_BY:
      return RunDegBy(stack, line);
    // Zastępuje wielomian z wierzchołka stosu jego wartością w danym punkcie
    case AT:
      return RunAt(stack, line);
    // Składa wielomiany ze stosu
    case COMPOSE:
      return RunCompose(stack, line);
    // Niepoprawne polecenie -- błąd
    case INVALID_COMMAND:
      return InvalidCommandName;
    // Błąd typu polecenia
    default:
      assert(false);
      return InvalidCommandName;
  }
}


//////////////////////////////////////////
//                                      //
//        Parsowanie wielomianów        //
//                                      //
//////////////////////////////////////////


/**
 * Konwertuje ciąg charów do wielomianu stałego, przypisuje go do
 * odpowiadającej wskaźnikowi zmiennej i ustawia wskaźnik oryginalnego
 * ciągu znaków na pierwszy nieprzetworzony znak. Funkcja rozpatruje
 * tylko liczby w zapisie dziesiętnym. W przypadku sukcesu zwraca
 * @p NoError; w przypadku napotkania błędu w trakcie konwersji,
 * zwraca @p ParsingErr. Funkcja zakłada, że przekazane wskaźniki wskazują
 * na istniejące i poprawne struktury danych.
 * @param[in] number : adres tablicy znaków
 * @param[in] p : wielomian, do którego zostanie przypisany wynik
 * @return @p NoError, jeśli konwersja zakończyła się sukcesem; @p parsinErr,
 * jeśli napotkano błędy
 */
static inline InputErr ConvertToPolyCoeffT(char **number, Poly *p) {
  // Wskaźnik na pierwszy nieprzetworzony znak
  char *ptr = NULL;
  // Konwersja ciągu do liczby typu long
  poly_coeff_t val = strtol(*number, &ptr, 10);
  // Liczba wykracza poza akceptowalny zakres lub konwersja nie powiodła się
  // -- nie przetworzono nawet jednego znaku. Błąd
  if (errno == ERANGE || ptr == *number) {
    return ParsingErr;
  }
  // Konwersja zakończona sukcesem
  else {
    // Aktualizacja oryginalnego wskaźnika na ciąg znaków
    *number = ptr;
    // Przypisanie wielomianu
    *p = PolyFromCoeff(val);
    return NoError;
  }
}


/**
 * Odpowiednik funkcji @p strtol dla typu @p int. Konwertuje ciąg znaków
 * @p num na liczbę typu @p int. Ustawia wskaźnik @p ptr w miejscu
 * pierwszego nieprzetworzonego znaku w ciągu @p num. Przy konwersji
 * funkcja przyjmuje system liczbowy o podstawie równej zmiennej @p base.
 * Musi ona być w zakresie @p [2,36]. Jeżeli otrzymana w trakcie konwersji
 * liczba wykracza poza zakres typu @p int, zwraca odpowiednio maksymalną
 * lub minimalną wartość, jaką może przechowywać ten typ oraz ustawia wartość
 * @p errno na @p ERANGE. W przypadku braku błędów zwraca znalezioną wartość.
 * @param[in] num : ciąg znaków konwertowany do liczby typu @p int
 * @param[in] ptr : wskaźnik na ciąg znaków @p char, który po wykonaniu
 * funkcji wskazuje na pierwszy nieprzetworzony znak w ciągu @p num
 * @param[in] base : podstawa systemu liczbowego, w ramach którego dokonywana
 * jest konwersja
 * @return wartość liczby odpowiadającej pewnemu początkowemu spójnemu
 * podciągowi ciągu @p num
 */
static inline int strtoi(char *num, char **ptr, const int base) {
  // System liczbowy musi być zgodny z wymaganiem funkcji `strtol`,
  // tj. musi być w zakresie [2, 36] (funkcja nie akceptuje podstawy równej 0)
  assert(2 <= base && base <= 32);

  // Obliczanie wartości liczbowej odpowiadającej maksymalnemu poprawnemu
  // ciągowi zaczynającemu się na początku `num`
  int val = strtol(num, ptr, base);
  // Wykroczono poza zakres
  if (val < INT_MIN) {
    errno = ERANGE;
    val = INT_MIN;
  }
  // Wykroczono poza zakres
  else if (val > INT_MAX) {
    errno = ERANGE;
    val = INT_MAX;
  }

  return val;
}

/**
 * Konwertuje maksymalny podciąg przekazanego ciągu znaków rozpoczynający się
 * na jego początku i przedstawiający liczbę w systemie dziesiętnym do
 * liczby typu @p poly_exp_t. Przypisuje jej wartość do zmiennej odpowiadającej
 * przekazanemu wskaźnikowi @p val. Jeśli otrzymana liczba wykracza poza zakres
 * tego typu, konwersja się nie powiodła lub jej wartość jest ujemna, zwraca
 * błąd @p ParsingErr. W przypadku błędu, zmienna odpowiadająca wskaźnikowi
 * @p val nie jest modyfikowana. W przypadku sukcesu zwraca @p NoError.
 * Funkcja modyfikuje także wskaźnik odpowiadający oryginalnej tablicy znaków,
 * ustawiając go na pierwszy nieprzetworzony znak. Funkcja zakłada, że
 * przekazane wskaźniki wskazują na istniejące i poprawne
 * struktury danych / zmienne.
 * @param[in] exp : adres tablicy znaków
 * @param[in] val : wskaźnik na zmienną, do której powinna zostać przypisana
 * obliczona wartość
 * @return @p NoError, jeśli konwersja zakończyła się sukcesem; @p parsinErr,
 * jeśli napotkano błędy
 */
static inline InputErr ConvertToPolyExpT(char **exp, poly_exp_t *val) {
  // Wskaźnik na pierwszy nieprzetworzony znak w ciągu `exp`
  char *ptr = NULL;
  // Konwersja maksymalnego podciągu zaczynającego się na początku ciągu `exp`
  *val = strtoi(*exp, &ptr, 10);
  // Wykroczono poza zakres typu `int`; nie przekonwertowano żadnego znaku;
  // wartość jest ujemna -- błąd
  if (errno == ERANGE || ptr == *exp || *val < 0) {
    return ParsingErr;
  }
  // Sukces konwersji -- przypisanie wartości do zmiennej odpowiadajacej
  // wskaźnikowi `exp` -- brak błędu
  else {
    *exp = ptr;
    return NoError;
  }
}

/**
 * Usuwa z pamięci wektor wraz ze wszystkimi jednomianami,
 * z których się składa.
 * @param[in] vector : wektor jednomianów
 */
static inline void FreeVector(MonoVector *vector) {
  // Usunięcie jednomianów
  DestroyMonos(vector);
  // Usunięcie wektora
  DestroyVector(vector);
}

static inline InputErr GetPoly(char **input, Poly *p, bool recursiveCall);

/**
 * Konwertuje ciąg znaków na odpowiadający im wielomian i przypisuje jego
 * wartość do zmiennej odpowiadającej danemu wskaźnikowi. W przypadku sukcesu
 * zwraca @p NoError. Jeśli jednak podczas tego procesu wystąpi błąd,
 * funkcja nie modyfikuje @p *newPoly i zwraca @p ParsingErr. Funkcja zakłada,
 * że dane wskaźniki wskazują na istniejące i poprawne struktury danych.
 * @param[in] ptr : wskaźnik na ciąg znaków reprezentujacych wielomian
 * @param[in] newPoly : wskaźnik na wielomian będący wynikiem tej funkcji
 * @return @p NoError -- w przypadku sukcesu w konwersji;
 * @p ParsingErr -- w przypadku wystąpienia błędu podczas konwersji
 */
static inline InputErr GetPolyInMono(char **ptr, Poly *newPoly) {
  // Wielomian nie jest stały -- składa się z jednomianów
  if (**ptr == '(') {
    // Konwersja jednomianów na wielomian i przesunięcie wskaźnika `*ptr` na
    // pierwszy nieprzeanalizowany znak w ciągu `*ptr`
    if (GetPoly(ptr, newPoly, true) != NoError) {
      // Konwersja na wielomian zakończona błędem.
      return ParsingErr;
    }
  }
  // Wielomian jest stały
  else if (isdigit(**ptr) || **ptr == '-') {
    // Konwersja ciągu na wielomian i przesunięcie wskaźnika `*ptr` na
    // pierwszy nieprzeanalizowany znak w tym ciągu
    if (ConvertToPolyCoeffT(ptr, newPoly) != NoError) {
      // Konwersja na wielomian zakończona błędem.
      return ParsingErr;
    }
  }
  // Niedozwolony znak -- błąd
  else {
    return ParsingErr;
  }

  // Konwersja zakończona sukcesem -- brak błędu
  return NoError;
}

/**
 * Konwertuje ciąg znaków na odpowiadającą im liczbę typu @p poly_exp_t
 * i przypisuje jej wartość do zmiennej odpowiadającej danemu wskaźnikowi.
 * W przypadku sukcesu zwraca @p NoError. Jeśli jednak podczas tego procesu
 * wystąpi błąd, funkcja nie modyfikuje @p *exp i zwraca @p ParsingErr.
 * Funkcja zakłada, że dane wskaźniki wskazują na istniejące i poprawne
 * zmienne.
 * @param[in] ptr : wskaźnik na ciąg znaków reprezentujacych liczbę typu
 * @p poly_exp_t
 * @param[in] exp : wskaźnik na zmienną typu @p poly_exp_t będącą wynikiem
 * działania tej funkcji
 * @return @p NoError -- w przypadku sukcesu w konwersji;
 * @p ParsingErr -- w przypadku wystąpienia błędu podczas konwersji
 */
static inline InputErr GetExpInMono(char **ptr, poly_exp_t *exp) {
  // Liczba jest nieujemna i nie może zawierać znaków '+'/'-'
  if (!isdigit(**ptr)) {
    return ParsingErr;
  }
  else {
    // Konwersja ciągu na liczbę typu `poly_exp_t` i ustawienie wskaźnika
    // `*ptr` na pierwszy nieprzetworzony znak
    if (ConvertToPolyExpT(ptr, exp) != NoError) {
      return ParsingErr;
    }
  }

  // Konwersja zakończona sukcesem
  return NoError;
}

/**
 * Konwertuje ciąg znaków na odpowiadający mu jednomian i dodaje go do
 * danego wektora jednomianów. Jednocześnie przesuwa wskaźnik @p *ptr
 * tak, aby wskazywał na pierwszy nieprzeanalizowany przez tę funkcję
 * znak w tym ciągu znaków. W przypadku suckesu zwraca @p NoError;
 * w przypadku napotkania błędu zwraca @p ParsingErr. Funkcja zakłada,
 * że dane wskaźniki wskazują na istniejące i poprawne dane.
 * @param[in] ptr : wskaźnik na ciąg znaków
 * @param[in] vector : wskaźnik na wektor jednomianów
 * @return @p NoError -- w przypadku sukcesu; @p ParsingErr -- w przypadku
 * błędu podczas konwersji
 */
static inline InputErr GetMono(char **ptr, MonoVector *vector) {
  // Jednomian musi zaczynać się nawiasem
  if (**ptr != '(') {
    return ParsingErr;
  }

  // Przesunięcie wskaźnika na kolejny znak
  *ptr = *ptr + 1;

  // Wielomian, z którego składa się szukany jednomian
  Poly newPoly;

  // Wczytanie wielomianu `newPoly`
  if (GetPolyInMono(ptr, &newPoly) != NoError) {
    // Błąd podczas wczytywania
    return ParsingErr;
  }

  // Wielomian musi być oddzielony od wykładnika pojedynczym przecinkiem
  if (**ptr != ',') {
    // Błąd, zwolnienie pamięci przydzielonej na wielomian
    PolyDestroy(&newPoly);
    return ParsingErr;
  }

  // Wykładnik odpowiadający szukanemu jednomianowi
  poly_exp_t exp;
  // Przesunięcie wskaźnika na wartość wykładnika
  *ptr = *ptr + 1;
  
  // Wczytanie wykładnika jednomianu
  if (GetExpInMono(ptr, &exp) != NoError) {
    // Błąd, zwolnienie pamięci przydzielonej na wielomian
    PolyDestroy(&newPoly);
    return ParsingErr;
  }

  // Jeśli wielomian jest zerowy, funkcja nie dodaje go do wektora
  if (!PolyIsZero(&newPoly)) {
    // Dodanie jednomianu do wektora
    AppendMono(vector, MonoFromPoly(&newPoly, exp));
  }

  // Wskaźnik `*ptr` wskazuje teraz na pierwszy znak po wczytanym wykładniku

  // Jednomian musi kończyć się nawiasem
  if (**ptr != ')') {
    return ParsingErr;
  }

  // Przesunięcie wskaźnika na pierwszy znak po jedomianie
  *ptr = *ptr + 1;

  // Sukces
  return NoError;
}

/**
 * Przeprowadza końcową analizę ciągu znaków reprezentującego wielomian
 * po przeprowadzeniu konwersji jego części na jednomian. W przypadku wykrycia
 * błędu w tym ciągu zwraca @p parsinErr. W przeciwnym razie zwraca @p NoError.
 * Funkcja modyfikuje także zmienną @p *continueLoop, której wartość
 * odpowiada temu, czy należy kontynuować wczytywanie kolejnych jednomianów.
 * Jeśli należy to robić, funkcja przesuwa wskaźnik @p *ptr na kolejny
 * znak. Funckja zakłada, że wszystkie przekazane wskaźniki wskazują na
 * istniejące i poprawne dane.
 * @param[in] ptr : wskaźnik na ciąg znaków
 * @param[in] recursiveCall : czy badany jedomian jest częścią pewnego
 * jednomianu
 * @param[in] continueLoop : czy kontynuować wczytywanie jednomianów
 * @return @p NoError -- w przypadku braku wykrycia błędów; @p ParsingErr
 * -- w przeciwnym razie
 */
static inline InputErr CloseMono(char **ptr, bool recursiveCall,
                                 bool *continueLoop) {
  // Badanie znaku następującego bezpośrednio po jednomianie
  switch (**ptr) {
    case ',':
      // Jednomian jest częścią innego jednomianu; przecinek wskazuje,
      // że następnie zostany wczytany jego wykładnik -- należy
      // przerwać budowanie wielomianu
      if (recursiveCall) {
        *continueLoop = false;
        break;
      }
      // Jeśli jednomian nie jest częścią innego jednomianu, to
      // przecinek jest niedozwolonym znakiem -- błąd
      else {
        return ParsingErr;
      }
    case '+':
      // Należy wczytać kolejny jednomian
      *ptr = *ptr + 1;
      *continueLoop = true;
      break;
    case '\0':
      // Koniec ciągu znaków, należy przerwać wczytywnie kolejnych
      // jednomianów
      *continueLoop = false;
      break;
    default:
      // Niedozwolony znak -- błąd
      return ParsingErr;
  }

  // Brak wykrytych błędów
  return NoError;
}

/**
 * Konwertuje ciąg znaków na odpowiadający im wielomian nie będący wielomianem
 * stałym -- a więc zbudowany z jednomianów. Przesuwa następnie
 * wskaźnik @p *input tak, aby wskazywał na pierwszy nieprzetworzony znak
 * w tym ciągu. Przypisuje wynik do zmiennej @p *p. W przypadku sukcesu
 * zwraca @p NoError; w przypadku napotkania błędu zwraca @p ParsingErr
 * i nie modyfikuje @p *p. Funkcja zakłada, że dane wskaźniki wskazują
 * na istniejące i poprawne dane.
 * @param[in] input : wskaźnik na ciąg znaków
 * @param[in] p : wskaźnik na wielomian będący wynikiem konwersji
 * @param[in] recursiveCall : czy funkcja została wywołana rekurencyjnie
 * @return @p NoError -- w przypadku sukcesu; @p ParsingErr -- w przypadku
 * napotkania błędu
 */
static inline InputErr GetPoly(char **input, Poly *p, bool recursiveCall) {
  // Wektor jednomianów -- ich suma jest wielomianem wynikowym
  MonoVector vector = CreateMonoVector();
  // Czy kontynuować wczytywanie jednomianów
  bool continueLoop = true;

  while (continueLoop) {
    // Wczytanie jednomianu
    if (GetMono(input, &vector) != NoError) {
      // Napotkano błąd. Usunięcie z pamięci jednomianów i wektora jednomianów
      FreeVector(&vector);
      return ParsingErr;
    }
    
    // Wskaźnik `*input` wskazuje teraz na pierwszy znak
    // po wczytanym jednomianie

    // Analiza znaku następującego po jednomianie. Jeśli należy wczytywać
    // kolejne jednomiany, wskaźnik `*input` zostanie przesunięty na pierwszy
    // znak kolejnego jednomianu. W przeciwnym razie `continueLoop` zmieni
    // wartość na false
    if (CloseMono(input, recursiveCall, &continueLoop) != NoError) {
      // Niepoprawny znak -- błąd.
      // Usunięcie z pamięci jednomianów i wektora jednomianów
      FreeVector(&vector);
      return ParsingErr;
    }
  }

  // Wskaźnik `*input` wskazuje na pierwszy nieprzetworzony znak

  // Sumuje jednomiany z wektora jednomianów -- jest to szukany wielomian
  if (VectorLength(&vector) > 0) {
    *p = PolyAddMonos(VectorLength(&vector), ConvertToArr(&vector));
  }
  // W przypadku braku jednomianów, jest to wielomian zerowy
  else {
    *p = PolyZero();
  }
  // Zwolnienie pamięci przydzielonej na wektor
  DestroyVector(&vector);
  return NoError;
}

/**
 * Konwertuje ciąg znaków prawdopodobnie przedstawiający wielomian złożony
 * z jednomianów na wielomian i dodaje go do przekazanego stosu wielomianów.
 * W przypadku sukcesu -- zwraca @p NoError; w przypadku napotkaniu błędu
 * -- zwraca @p ParsingErr. Funkcja zakłada, że przekazane wskaźniki na
 * stos wielomianów i ciąg znaków wskazują na istniejące i poprawne dane.
 * @param[in] stack : stos wielomianów
 * @param[in] poly : ciąg znaków
 * @return @p NoError w przypadku sukcesu w dodaniu wielomianu do stosu;
 * @p ParsingErr w przypadku napotkania błędu związanego z parsowaniem
 * wielomianu użytkownika
 */
static inline InputErr ParsePoly(stack_t *stack, char **poly) {
  // Wielomian odpowiadający wielomianowi przedstawionemu za pomocą ciągu
  // znaków w tablicy `poly`
  Poly newPoly;
  // Napotkano problemy z parsowaniem wielomianu -- błąd
  if (GetPoly(poly, &newPoly, false) != NoError) {
    return ParsingErr;
  }
  // Dodanie wielomianu do stosu -- brak błędu
  else {
    PushPoly(stack, newPoly);
    return NoError;
  }
}

/**
 * Dodaje do przekazanego stosu wielomianów wielomian przekazany przez
 * użytkownika. Jeśli jest on poprawny, zwraca @p NoError. W przypadku
 * napotkania błędu podczas jego parsowania zwraca @p ParsingErr.
 * Funkcja zakłada, że przekazane wskaźniki na stos wielomianów i na
 * linię tekstu (string) wskazują na istniejące i poprawne struktury danych.
 * @param[in] stack : stos wielomianów
 * @param[in] line : wielomian przekazany przez użytkownika
 * @return @p NoError w przypadku sukcesu w dodaniu wielomianu do stosu;
 * @p ParsingErr w przypadku napotkania błędu związanego z parsowaniem
 * wielomianu użytkownika
 */
static inline InputErr ExecutePoly(stack_t *stack, string_t *line) {
  // Konwersja całego stringa na tablicę charów
  char *poly = GetCharArrayAt(line, 0);
  char *copy = poly;
  InputErr errors = NoError;

  // Podany wielomian jest wielomianem stałym
  if (CharAt(line, 0) == '-' || isdigit(CharAt(line, 0))) {
    char *remainingChar = NULL;
    // Konwersja maksymalnego poprawnego ciągu przedstawiającego liczbę
    // w systemie dziesiętnym na liczbę typu long
    long argValue = strtol(poly, &remainingChar, 10);
    // Wartość liczby wykracza poza zakres typu long lub ciąg znaków
    // zawiera niedozwolony znak (spójny podciąg znaków przedstawiający
    // liczbę nie kończy się wraz z końcem tablicy znaków) -- błąd
    if (errno == ERANGE || *remainingChar != '\0') {
      return ParsingErr;
    }
    // Linia przedstawia poprawny wielomian stały;
    // dodanie go do przekazanego stosu -- brak błędów
    else {
      PushPoly(stack, PolyFromCoeff((poly_coeff_t) argValue));
      poly = remainingChar;
      errors = NoError;
    }
  }
  // Podany wielomian jest przedstawiony w postaci jednomianów lub jest błędny
  else {
    errors = ParsePoly(stack, &poly);
  }

  // Pojawił się null char w środku stringa -- błąd.
  // Jeśli został dodany wielomian (a więc jeślie `errors` = NoError),
  // wielomian ten musi zostać usunięty
  if ((poly - copy) / sizeof(char) < StringLength(line)
      && errors == NoError) {
    ExecutePop(stack);
    errors = ParsingErr;
  }

  return errors;
}


//////////////////////////////////////////
//                                      //
//     Konwersja linii na polecenie     //
//           lub wielomian              //
//                                      //
//////////////////////////////////////////


/**
 * Wykonuje daną linię otrzymaną od użytkownika. W przypadku, gdy jest
 * to wielomian, dodaje go na przekazany stos wielomianów. W przypadku
 * polecenia, wykonuje je. Następnie zwraca komunikat informujący
 * o napotkanych błędach lub powodzeniu operacji. Funkcja zakłada, że
 * przekazane wskaźniki na stos wielomianów i na linię tekstu (string)
 * wskazują na istniejące i poprawne struktury danych.
 * @param[in] stack : stos wielomianów
 * @param[in] line : linia
 * @return komunikat o napotkanych błędach lub ich braku
 */
static inline InputErr ExecuteLine(stack_t *stack, string_t *line) {
  // Linia jest pusta lub należy ją zignorować -- brak błędów
  if (StringLength(line) == 0 || CharAt(line, 0) == '#') {
    return NoError;
  }

  // Linia zawiera polecenie
  if (isalpha(CharAt(line, 0))) {
    return ExecuteCommand(stack, line);
  }
  // Linia zawiera wielomian
  else {
    return ExecutePoly(stack, line);
  }
}


//////////////////////////////////////////
//                                      //
//           Obsługa błędów             //
//                                      //
//////////////////////////////////////////


/**
 * Wypisuje komunikat o błędzie dotyczący wykonanej akcji wraz z numerem
 * odpowiadajęcej mu linii. W przypadku braku błędów nie robi nic.
 * @param[in] errorMessage : komunikat o błędach
 * @param[in] numberOfLine : number linii, w której nastąpił błąd
 */
static inline void PrintError(InputErr errorMessage, size_t numberOfLine) {
  switch (errorMessage) {
    // Niepoprawna nazwa polecenia
    case InvalidCommandName:
      fprintf(stderr, "ERROR %zu WRONG COMMAND\n", numberOfLine);
      break;
    // Niepoprawny argument polecenia DEG_BY
    case NoDegByParam:
      fprintf(stderr, "ERROR %zu DEG BY WRONG VARIABLE\n", numberOfLine);
      break;
    // Niepoprawny argument polecenia AT
    case NoAtParam:
      fprintf(stderr, "ERROR %zu AT WRONG VALUE\n", numberOfLine);
      break;
    case NoComposeParam:
      fprintf(stderr, "ERROR %zu COMPOSE WRONG PARAMETER\n", numberOfLine);
      break;
    // Brak odpowiedniej liczby wielomianów na stosie wielomianów
    case StackUnderflow:
      fprintf(stderr, "ERROR %zu STACK UNDERFLOW\n", numberOfLine);
      break;
    // Błąd podczas parsowania wielomianu
    case ParsingErr:
      fprintf(stderr, "ERROR %zu WRONG POLY\n", numberOfLine);
      break;
    // Brak błędów
    case NoError:
      break;
    // Błędny komunikat
    default:
      assert(false);
  }
}


//////////////////////////////////////////
//                                      //
//        Kalkulator wielomianów        //
//                                      //
//////////////////////////////////////////


/**
 * Kalkulator wielomianów wielu zmiennych. Wczytuje polecenia od użytkownika
 * i wykonuje je. W przypadku błędu wyświetla je. Kontynuuje aż do napotkania
 * linii zakończonej znakiem @p EOF.
 * 
 * @details
 * Tworzy stos wielomianów, na którym będzie przechowywać wielomiany tworzone
 * przez użytkownika oraz string, który będzie przyjmować polecenia.
 * Wczytuje pojedynczą linię,  wykonuje zawartą w niej instrukcję
 * i wypisuje ewentualny napotkany w trakcie błąd. Po zakończeniu działania
 * zwalnia całą zaalokowaną pamięć -- stos wraz ze wciąż znajdującymi się
 * na nim wielomianami oraz string.
 */
static inline void RunCalculator() {
  // Stos wielomianów, na których będą wykonywane polecenia
  stack_t polyStack = CreateStack();
  // String przyjmujący polecenia od użytkownika
  string_t newLine = CreateString();
  // Typ polecenia -- wielomian / operacja na wielomianach
  LineType type;
  // Numer aktualnie rozważanej linii
  size_t numberOfLine = 1;
  // Czy kontynuować działanie kalkulatora
  bool continueLoop = true;

  while (continueLoop) {
    // Wczytanie linii od użytkownika
    type = ScanString(&newLine);

    switch (type) {
      // Ostatnie polecenie -- koniec pliku
      case EndOfFile:
        // Zatrzymanie kalkulatora po przetworzeniu linii
        continueLoop = false;
        // Wykonaj polecenie i wypisz ewentualny błąd
        PrintError(ExecuteLine(&polyStack, &newLine), numberOfLine);
        break;
      // Polecenie, brak zatrzymania kalkulatora
      case EndOfLine:
        // Wykonaj polecenie i wypisz ewentualny błąd
        PrintError(ExecuteLine(&polyStack, &newLine), numberOfLine);
        break;
      // Błędny typ linii -- błąd
      default:
        assert(false);
    }

    // Aktualizacja numeru linii
    numberOfLine++;
    // Zresetowanie stringa
    ResetString(&newLine);
    // Dostowanie stosu do ilości znajdujących się w nim wielomianów
    AdjustStack(&polyStack);
  }

  // Zwolnienie pamięci przydzielonej na stringa
  DestroyString(&newLine);
  // Zwolnienie pamięci przydzielonej na stos i usunięcie pozostałych
  // na nim wielomianów
  DestroyPolys(&polyStack);
  DestroyStack(&polyStack);
}

/**
 * Uruchamia kalkulator.
 */
int main() {
  // Uruchomienie kalkulatora
  RunCalculator();
  
  return 0;
}

