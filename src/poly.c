/** @file
  Implementacja klasy wielomianów rzadkich wielu zmiennych
 
  @author Dawid Mędrek
  @date 2021
*/

#include <stdlib.h>

#include "poly.h"

////////////////////////////
//                        //
//       CHECK_PTR        //
//                        //
////////////////////////////

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

//////////////////////////////
//                          //
//       PolyDestroy        //
//                          //
//////////////////////////////

/**
 * Jeśli wielomian jest stały, to nie robi nic, gdyż nie została
 * zaalokowana żadna pamięć. W przeciwnym wypadku, wielomian ten ma
 * niepustą tablicę jednomianów; każdy z nich zostaje usunięty z pamięci
 * za pomocą funkcji @p MonoDestroy. Następnie zostaje zwolniona pamięć
 * zaalokowana na tablicę jednomianów
 */
void PolyDestroy(Poly *p) {
  if (p != NULL) {
    if (!PolyIsCoeff(p)) {
      for (size_t i = 0; i < p->size; i++) {
        MonoDestroy(&p->arr[i]);
      }

      free(p->arr);
    }
  }
}

////////////////////////////
//                        //
//       PolyClone        //
//                        //
////////////////////////////

/**
 * Jeżeli wielomian jest stały, to zwraca taki sam wielomian
 * wygenerowany funkcją @p PolyFromCoeff. W przeciwnym wypadku wielomian
 * posiada tablicę jednomianów. Funkcja tworzy wówczas tablicę typu Mono
 * o wielkości tej samej, co w oryginalnym wielomianie i sprawdza, czy
 * została na nią poprawnie zaalokowana pamięć. Jeśli nie -- program kończy
 * się awaryjnie kodem @p 1. Inaczej każdy z jednomianów, z których
 * składał się oryginalny wielomian, jest kopiowany do nowo stworzonej
 * tablicy za pomocą funkcji @p MonoClone. Funkcja zwraca wówczas wielomian
 * złożony z tej tablicy oraz jej rozmiaru.
 */
Poly PolyClone(const Poly *p) {
  assert(p != NULL);

  if (PolyIsCoeff(p)) {
    return PolyFromCoeff(p->coeff);
  }
  else {
    const size_t numOfMono = p->size;

    // Tablica jednomianów wielomianu wyjściowego
    Mono *newArr = malloc(numOfMono * sizeof(Mono));

    CHECK_PTR(newArr);

    // Uzupełnienie tablicy kopiami jednomianów z oryginalnego wielomianu
    for (size_t i = 0; i < numOfMono; i++) {
      newArr[i] = MonoClone(&p->arr[i]);
    }

    return (Poly) {.size = numOfMono, .arr = newArr};
  }
}

//////////////////////////
//                      //
//       PolyAdd        //
//                      //
//////////////////////////

/**
 * Zwraca liczbę parami różnych wykładników z tablic wielomianów.
 * @param[in] p : wielomian nie będący wielomianem stałym
 * @param[in] q : wielomian nie będący wielomianem stałym
 * @return liczba jednomianów o parami różnych wykładnikach z tablic
 * wielomianów @p p i @p q
 * 
 * @details
 * Rozważa jednomiany na indeksach @f$i@f$ oraz @f$j@f$ kolejno w tablicach
 * wielomianów @f$p@f$ i @f$q@f$. Zlicza ten, który ma mniejszy wykładnik
 * i zwiększa odpowiadający indeks o @p 1. W przypadku, gdy oba rozważane
 * jednomiany mają ten sam wykładnik, zlicza je tylko raz, zwiększając
 * oba indeksy.
 */
static inline size_t NumOfUniqueExps(const Poly *p, const Poly *q) {
  // Indeksy wskazujące na jednomiany kolejno z tablic wielomianu p i q
  size_t i = 0, j = 0;
  // Zmienna zliczająca wykładniki
  size_t counter = 0;

  while (i < p->size || j < q->size) {
    if (i < p->size && j < q->size) {
      if (MonoGetExp(&p->arr[i]) < MonoGetExp(&q->arr[j])) {
        i++;
      }
      else if (MonoGetExp(&p->arr[i]) > MonoGetExp(&q->arr[j])) {
        j++;
      }
      else {
        i++;
        j++;
      }

      counter++;
    }
    else if (i < p->size) {
      // Zliczenie pozostałych jednomianów
      counter += p->size - i;
      break;
    }
    else {
      // Zliczenie pozostałych jednomianów
      counter += q->size - j;
      break;
    }
  }

  return counter;
}

/**
 * Oblicza i zwraca sumę wielomianu stałego i wielomianu nie będącego
 * wielomianem stałym o niezerowym wyrazie wolnym.
 * @param[in] p : wielomian stały
 * @param[in] q : wielomian nie będący wielomianem stałym o niezerowym
 * wyrazie wolnym
 * @return @f$p + q@f$
 * 
 * @details
 * Nowy wielomian może różnić się od wielomianu @f$q@f$ tylko wyrazem wolnym.
 * Funkcja oblicza go, a następnie sprawdza, czy jest tożsamościowo równy
 * zeru. Jeśli jest, nowy wielomian składa się z jednego jednomianu mniej.
 * W przeciwnym razie, składa się z takiej samej ilości jednomianów. Trzeba
 * pamiętać, że suma wielomianu stałego i wielomianu nie będącego wielomianem
 * stałym nie może być wielomianem stałym.
 */
static inline Poly SumConstPoly0(const Poly *p, const Poly *q) {
  // Tablica jednomianów wyjściowego wielomianu
  Mono *newArr;
  // Obliczanie wyrazu wolnego w nowym wielomianie
  Mono tmpMono = (Mono){.p = PolyAdd(p, &q->arr[0].p), .exp = 0};

  // Nowy wyraz wolny jest tożsamościowo równy zeru
  if (PolyIsZero(&tmpMono.p)) {
    MonoDestroy(&tmpMono);

    // Wyjściowy wielomian nie ma wyrazu wolnego
    newArr = malloc((q->size - 1) * sizeof(Mono));

    CHECK_PTR(newArr);

    for (size_t i = 1; i < q->size; i++) {
      newArr[i - 1] = MonoClone(&q->arr[i]);
    }

    return (Poly) {.size = q->size - 1, .arr = newArr};
  }
  // Nowy wyraz wolny jest niezerowy
  else {
    // Wyjściowy wielomian składa się z takiej samej liczby
    // jednomianów co oryginalny
    newArr = malloc(q->size * sizeof(Mono));

    CHECK_PTR(newArr);

    // Wyraz wolny
    newArr[0] = tmpMono;

    for (size_t i = 1; i < q->size; i++) {
      newArr[i] = MonoClone(&q->arr[i]);
    }

    return (Poly) {.size = q->size, .arr = newArr};
  }
}

/**
 * Zwraca sumę wielomianu stałego i wielomianu nie będącego
 * wielomianem stałym.
 * @param[in] p : wielomian stały
 * @param[in] q : wielomian nie będący wielomianem stałym
 * @returns @f$p + q@f$
 * 
 * @details
 * Jeśli wielomian @f$q@f$ nie posiada wyrazu wolnego (jednomianu
 * o wykładniku równym @p 0), to wyjściowy wielomian będzie różnił się od
 * oryginalnego tym, że jego wyraz wolny będzie równy wielomianowi @f$p@f$.
 * W przeciwnym razie oblicza ich sumę za pomocą funkcji
 * pomocniczej @p SumConstPoly0.
 * @sa SumConstPoly0
 */
static inline Poly SumConstPoly(const Poly *p, const Poly *q) {
  // Wielomian q nie ma wyrazu wolnego
  if (MonoGetExp(&q->arr[0]) != 0) {
    // Tablica jednomianów wyjściowego wielomianu.
    // Będzie mieć o jeden jednomian więcej
    // -- będzie on wyrazem wolnym
    Mono *newArr = malloc((1 + q->size) * sizeof(Mono));

    CHECK_PTR(newArr);

    // Wyraz wolny wyjściowego wielomianu
    newArr[0] = (Mono) {.p = PolyFromCoeff(p->coeff), .exp = 0};

    for (size_t i = 1; i <= q->size; i++) {
      newArr[i] = MonoClone(&q->arr[i - 1]);
    }

    return (Poly) {.size = 1 + q->size, .arr = newArr};
  }
  // Wielomian q posiada wyraz wolny
  else {
    return SumConstPoly0(p, q);
  }
}

/**
 * Zwraca ten spośród dwóch jednomianów, który ma mniejszy wykładnik (lub ich
 * sumę, jeśli wykładniki są równe). Zwiększa przy tym wartość tego indeksu
 * o @p 1, którego odpowiadający jednomian zwróciła funkcja (lub oba z nich,
 * jeśli zwraca ich sumę). Zakłada, że żaden ze wskaźników nie jest pusty
 * oraz że co najmniej jeden z indeksów mieści się w rozmiarze odowiadającej
 * mu tablicy (nie dotyczy to natomiast obu z nich jednocześnie).
 * @param[in] p : wielomian nie będący wielomianem stałym
 * @param[in] q : wielomian nie będący wielomianem stałym
 * @param[in] i : indeks odnoszący się do tablicy jednomianów wielomianu @p p
 * @param[in] j : indeks odnoszący się do tablicy jednomianów wielomianu @p q
 * @return ten spośród jednomianów @p p->arr[*i], @p q->arr[*j], który
 * ma mniejszy wykładnik (lub ich suma, jeśli wykładniki są równe).
 * 
 * @details
 * Rozważa trzy przypadki. Jeśli oba indeksy mieszczą się w zakresie
 * odpowiadających im tablic lub jeśli tylko jeden z nich spełnia ten warunek.
 * W przypadku, gdy jest rozważany tylko jeden wielomian (któryś z indeksów
 * jest poza zakresem odpowidającej mu tablicy) oraz w przypadku, gdy
 * porównywane wykładniki są różne, kopiuje odpowiedni jednomian, modyfikuje
 * wartość indeksu i zwraca wynik. W przypadku, gdy porównywane wykładniki
 * są równe, sumuje jednomiany, modyfikuje wartości obu indeksów i zwraca
 * otrzymaną sumę.
 */
static inline Mono NextMonoAdd(const Poly *p, const Poly *q,
                               size_t *i, size_t *j) {
  // Wyjściowy jednomian
  Mono monomial;

  // Oba indeksy mieszczą się w zakresie odpowiadającym im tablic
  if (*i < p->size && *j < q->size) {
    if (MonoGetExp(&p->arr[*i]) < MonoGetExp(&q->arr[*j])) {
      monomial = MonoClone(&p->arr[*i]);
      *i = *i + 1;
    }
    else if (MonoGetExp(&p->arr[*i]) > MonoGetExp(&q->arr[*j])) {
      monomial = MonoClone(&q->arr[*j]);
      *j = *j + 1;
    }
    else {
      monomial = (Mono) {
        // Sumuje wielomiany
        .p = PolyAdd(&p->arr[*i].p, &q->arr[*j].p),
        .exp = MonoGetExp(&p->arr[*i])
      };

      *i = *i + 1;
      *j = *j + 1;
    }
  }
  // Indeks `j` wyszedł poza zakres swojej tablicy
  else if (*i < p->size) {
    monomial = MonoClone(&p->arr[*i]);
    *i = *i + 1;
  }
  // Indkes `i` wyszedł poza zakres swojej tablicy
  else {
    monomial = MonoClone(&q->arr[*j]);
    *j = *j + 1;
  }

  return monomial;
}

/**
 * Zwraca wielomian składający się z jednomianów znajdujących się
 * w tablicy jednomianów. Modyfikuje zawartość otrzymanej tablicy, jak
 * również ją samą (być może zwalnia przydzieloną na nią pamięć).
 * Zakłada, że wskaźnik na tablicę jednomianów nie jest pusty oraz że jest
 * spełniona nierówność: @p liczba @p jednomianów @p w @p tablicy @f$\le@f$
 * @p rozmiar @p tablicy. Zakłada także, że jednomiany w tablicy są
 * uporządkowane rosnąco ze względu na odpowiadające im wykładniki
 * (w szczególności żaden wykładnik nie może się powtórzyć). Nie ma również
 * "pustych" miejsc w tablicy jednomianów na indeksach mniejszych od liczby
 * jednomianów w tej tablicy, tzn. pod każdym z indeksów @p 0..numOfMonos
 * znajduje się jakiś jednomian.
 * @param[in] monos : tablica jednomianów
 * @param[in] numOfMonos : liczba jednomianów w tablicy @p monos
 * @param[in] sizeOfArr : rozmiar tablicy @p monos
 * @return wielomian złożony z jednomianów znajdujących się w tablicy @p monos
 * 
 * @details
 * Jeśli liczba jednomianów jest równa zeru, zwraca wielomian zerowy.
 * Jeśli liczba jednomianów jest równa jeden, sprawdza, czy wyjściowy
 * wielomian nie będzie wielomianem stałym. Jeśli tak, zwalnia całą
 * przydzieloną na jednomiany i tablicę pamięć, a następnie zwraca odpowiedni
 * wielomian stały. W przeciwnym wypadku (jeśli jest taka potrzeba) modyfikuje
 * ilość pamięci przydzieloną na tablicę i zwraca wielomian składający się
 * z niej.
 */
static inline Poly BuildPolyFromMonos(Mono *monos, const size_t numOfMonos,
                                      const size_t sizeOfArr) {
  // Brak jednomianów -- wielomian jest zerowy
  if (numOfMonos == 0) {
    free(monos);
    return PolyZero();
  }
  // W tablicy monos znajduje się dokładnie jeden jednomian.
  // Sprawdza, czy nie jest on tożsamościowy z pewna funkcją stałą
  else if (numOfMonos == 1 && PolyIsCoeff(&monos[0].p) &&
           MonoGetExp(&monos[0]) == 0) {
    const poly_coeff_t polyCoeff = monos[0].p.coeff;
    MonoDestroy(&monos[0]);
    free(monos);

    return PolyFromCoeff(polyCoeff);
  }
  // W tablicy monos znajduje się co najmniej jeden jednomian
  // nie będący tożsamościowy równy żadnej funkcji stałej
  else {
    // Zmiana rozmiaru tablicy monos, jeśli jest taka potrzeba
    if (numOfMonos != sizeOfArr) {
      monos = realloc(monos, numOfMonos * sizeof(Mono));

      assert(monos != NULL);
    }

    return (Poly) {.size = numOfMonos, .arr = monos};
  }
}

/**
 * Sumuje dwa wielomiany nie będące wielomianami stałymi. Zakłada, że
 * przekazane wskaźniki na wielomiany nie są puste.
 * @param[in] p : wielomian nie będący wielomianem stałym
 * @param[in] q : wielomian nie będący wielomianem stałym
 * @return @f$p + q@f$
 * 
 * @details
 * Oblicza liczbę unikalnych wykładników wśrod jednomianów, z których
 * składają się wielomiany @p p i @p q, a następnie alokuje tablicę
 * jednomianów o takim rozmiarze. Rozważa kolejne jednomiany z tablic
 * jednomianów wielomianów @p p i @p q i zapisuje ich kopie w nowo stworzonej
 * tablicy jednomianów (lub ich sumę, jeśli mają te same wykładniki).
 * Służy do tego funkcja pomocnicza @p NextMonoAdd. Ostatecznie jednomiany
 * w nowej tablicy są uporządkowane w sposób rosnący ze względu
 * na wykładniki, a wśród nich nie ma żadnego tożsamościowo równego zeru.
 * Zwraca następnie wielomian złożony z tych jednomian, stworzny za pomocą
 * funkcji pomocniczej @p BuildPolyFromMonos.
 * @sa NextMonoAdd, BuildPolyFromMonos
 */
static inline Poly SumPolyPoly(const Poly *p, const Poly *q) {
  // Liczba parami różnych wykładników wśród jednomianów
  // z tablic jednomianów wielomianów wejściowych
  const size_t newSize = NumOfUniqueExps(p, q);

  // Tablica jednomianów wyjściowego wielomianu
  Mono *newArr = malloc(newSize * sizeof(Mono));

  // Indeksy rozważanych aktualnie jednomianów
  // -- odpowiednio w tablicy wielomianu p i q
  size_t i = 0, j = 0;
  // Indeks, pod którym są zapisywane kolejne jednomiany w tablicy newArr
  size_t index = 0;
  // Jednomian pomocniczy
  Mono tmpMonomial;

  while (i < p->size || j < q->size) {
    // Wybór kolejnego do zapisania w tablicy jednomianu
    // i modyfikacja indeksów `i` oraz `j`
    tmpMonomial = NextMonoAdd(p, q, &i, &j);

    // Aktualizacja tablicy jednomianów
    if (!PolyIsZero(&tmpMonomial.p)) {
      newArr[index] = tmpMonomial;
      index++;
    }
    // Zwolnienie nieużywanej pamięci
    else {
      MonoDestroy(&tmpMonomial);
    }
  }

  // W tym momencie zmienna `index` jest równa liczbie jednomianów
  // w tablicy newArr

  // Analizuje newArr i zwraca wielomian składający się z jednomianów
  // znajdujących się w tablicy newArr
  return BuildPolyFromMonos(newArr, index, newSize);
}

/**
 * Jeśli @f$p@f$ i @f$q@f$ są wielomianami stałymi, to zwraca wielomian
 * stale równy sumie współczynników tych wielomianów stworzony przy pomocy
 * funkcji @p PolyFromCoeff.
 * Jeśli dokładnie jeden z tych wielomianów jest wielomianem stałym,
 * to ich sumę oblicza i zwraca funkcja pomocnicza @p SumConstPoly.
 * Jeżeli zaś żaden z tych wielomianów nie jest wielomianem stałym,
 * sumę @f$p + q@f$ oblicza funkcja @p SumPolyPoly, a następnie zwraca wynik.
 * @sa PolyFromCoeff, SumConstPoly, SumPolyPoly
 */
Poly PolyAdd(const Poly *p, const Poly *q) {
  if (PolyIsCoeff(p) && PolyIsCoeff(q)) {
    return PolyFromCoeff(p->coeff + q->coeff);
  }
  else if (PolyIsCoeff(p)) {
    if (PolyIsZero(p)) {
      return PolyClone(q);
    }

    return SumConstPoly(p, q);
  }
  else if (PolyIsCoeff(q)) {
    return PolyAdd(q, p);
  }
  else {

    return SumPolyPoly(p, q);
  }
}

///////////////////////////////
//                           //
//       PolyAddMonos        //
//                           //
///////////////////////////////


/**
 * Zwraca kopię tablicy jednomianów. Zakłada, że oryginalna
 * tablica nie jest pusta oraz że pod każdym indeksem znajduje się
 * w niej pewien jednomian. Zakłada również, że przekazany rozmiar tablicy
 * odpowiada stanowi faktycznemu. Przejmuje elementy oryginalnej tablicy
 * na własność.
 * @param[in] size : rozmiar tablicy jednomianów
 * @param[in] monos : tablica jednomianów
 * @return kopia tablicy @p monos
 * 
 * @details
 * Alokuje pamięć dla tablicy jednomianów o tym samym rozmiarze,
 * kopiuje jej elementy, a następnie zwraca wynik.
 */
static inline Mono *CopyMonoArr(const size_t size, const Mono monos[]) {
  Mono *monosCopy = malloc(size * sizeof(Mono));
  CHECK_PTR(monosCopy);

  // Kopiuje tablicę
  for (size_t i = 0; i < size; i++) {
    monosCopy[i] = monos[i];
  }

  return monosCopy;
}

/**
 * Jeśli @p count jest równy zeru lub @p monos jest równy @p NULL, zwraca
 * wielomian zerowy. W przeciwnym razie tworzy posortowaną kopię tablicy
 * jednomianów i wywołuje na niej funkcję @p PolyOwnMonos, której wynik zwraca.
 * @sa CopyMonoArr, PolyOwnMonos
 */
Poly PolyAddMonos(size_t count, const Mono monos[]) {
  if (count == 0 || monos == NULL) { return PolyZero(); }

  // Kopiuje tablicę `monos`
  Mono *monosCopy = CopyMonoArr(count, monos);

  // Sumuje jednomiany i zwraca wielomian wynikowy
  return PolyOwnMonos(count, monosCopy);
}


//////////////////////////
//                      //
//     PolyOwnMonos     //
//                      //
//////////////////////////


/**
 * Funkcja porównawcza dla jednomianów.
 * Porównuje wykładniki dwóch jednomianów.
 * Zakłada, że @p p i @p q nie są pustymi wskaźnikami.
 * @param[in] p : jednomian
 * @param[in] q : jednomian
 * @return
 * jeśli @p MonoGetExp(p) > @p MonoGetExp(q) -- zwraca @p 1;
 * jeśli @p MonoGetExp(p) > @p MonoGetExp(q) -- zwraca @p -1;
 * w przeciwnym razie zwraca @p 0.
 */
static int MonoCmp(const void *p, const void *q) {
  if (MonoGetExp(p) > MonoGetExp(q))      { return  1; }
  else if (MonoGetExp(p) < MonoGetExp(q)) { return -1; }
  else                                    { return  0; }
}

/**
 * Sortuje jednomiany w danej tablicy.
 * Posortowana tablica jest uporządkowana względem wykładników jednomianów
 * w porządku niemalejącym.
 * Zakłada, że @p size > 0, @p monos != @p NULL.
 * @param[in] size : rozmiar tablicy
 * @param[in] monos : tablica jednomianów
 */
static inline void SortMonos(const size_t size, Mono *monos) {
  qsort(monos, size, sizeof(Mono), MonoCmp);
}

/**
 * Jeśli @p count jest równy zeru lub @p monos jest równy @p NULL, zwraca
 * wielomian zerowy. W przeciwnym razie sortuje tablicę jednomianów,
 * a następnie sumuje jednomiany i zwraca wynik przy pomocy
 * funkcji @p BuildPolyFromMonos.
 * @sa SortMonos, BuildPolyFromMonos
 */
Poly PolyOwnMonos(size_t count, Mono *monos) {
  if (count == 0 || monos == NULL) { return PolyZero(); }

  // Sortowanie jednomianów ze względu na ich wykładniki
  SortMonos(count, monos);

  // Indeks, pod którym będą zapisywane kolejne jednomiany
  // w tablicy monos
  size_t index = 0;
  // Tymczasowa zmienna dla obliczanego wielomianu
  Poly tmpPoly;

  for (size_t i = 1; i < count; i++) {
    // Sumuje jednomiany o tym samym wykładniku
    if (MonoGetExp(&monos[index]) == MonoGetExp(&monos[i])) {
      tmpPoly = PolyAdd(&monos[index].p, &monos[i].p);
      PolyDestroy(&monos[index].p);
      MonoDestroy(&monos[i]);

      // Modyfikuje wartość wielomianu; wykładnik pozostaje bez zmian
      monos[index].p = tmpPoly;
    }
    // Znaleziono nowy wykładnik
    else {
      // Jeśli ostatni jednomian nie jest zerem, przesuwa indeks,
      // aby nowy jednomian zapisać na kolejnej pozycji
      if (!PolyIsZero(&monos[index].p)) {
        index++;
      }
      // W przeciwnym razie usuwa ostatni jednomian;
      // nowy jednomian będzie zapisany w tym samym miejscu
      else {
        MonoDestroy(&monos[index]);
      }

      monos[index] = monos[i];
    }
  }

  // W tym momencie zmienna `index` zaznacza ostatnie miejsce w tablicy
  // monos, na którym zapisano jednomian

  // Jeśli jest to jednomian zerowy, usuwa go
  if (PolyIsZero(&monos[index].p)) {
    MonoDestroy(&monos[index]);
  }
  // W przeciwnym razie zwiększa zmienną `index` o 1 tak, aby
  // jej wartość była równa liczbie jednomianów w tablicy monos
  else {
    index++;
  }

  // W tym momencie zmienna `index` jest równa liczbie jednomianów
  // w tablicy monos

  // Zwraca wielomian zbudowany z jednomianów w tablicy monos
  return BuildPolyFromMonos(monos, index, count);
}


//////////////////////////
//                      //
//    PolyCloneMonos    //
//                      //
//////////////////////////


/**
 * Tworzy kopię tablicy jednomianów złożoną z pełnych kopii jednomianów,
 * z których składa się oryginalna tablica. Funkcja zakłada, że @p size jest
 * wartością większą od zera, a @p monos nie jest równy @p NULL.
 * @param[in] size : liczba jednomianów w tablicy
 * @param[in] monos : tablica jednomianów
 * @return pełna kopia tablicy @p monos
 */
static inline Mono *CloneMonoArr(const size_t size, const Mono monos[]) {
  // Sprawdzenie warunków początkowych -- założeń funkcji
  assert(size > 0 && monos != NULL);

  // Tworzenie nowej tablicy
  Mono *monosArr = malloc(size * sizeof(Mono));
  // Sprawdzenie, czy zaalokowano poprawnie pamięć na tablicę
  CHECK_PTR(monosArr);

  // Kopiowanie jednomianów
  for (size_t i = 0; i < size; i++) {
    monosArr[i] = MonoClone(&monos[i]);
  }

  return monosArr;
}

/**
 * Jeśli @p count jest równy zeru lub @p monos równy @p NULL, zwraca
 * wielomian zerowy. W przeciwnym razie wykonuje pełną kopię tablicy @p monos
 * i sumuje jednomiany przy użyciu funkcji @p PolyOwnMonos.
 * @sa CloneMonoArr, PolyOwnMonos
 */
Poly PolyCloneMonos(size_t count, const Mono monos[]) {
  if (count == 0 || monos == NULL) { return PolyZero(); }

  Mono *monosArr = CloneMonoArr(count, monos);

  return PolyOwnMonos(count, monosArr);
}


//////////////////////////
//                      //
//       PolyMul        //
//                      //
//////////////////////////


/**
 * Oblicza i zwraca wynik mnożenia wielomianu stałego różnego od zera
 * oraz wielomianu nie będącego wielomianem stałym. Zakłada, że wskaźniki
 * na wielomiany będące argumentami są niepuste.
 * @param[in] p : wielomian stały
 * @param[in] q : wielomian nie będący wielomianem stałym
 * @return @f$p * q@f$
 * 
 * @details
 * Alokuje tablicę o tym samym rozmiarze co wielomian @p q. Mnoży
 * każdy z jednomianów, z których się składa przez wielomian stały,
 * uzupełniając stworzoną tablicę. Następnie zwraca wielomian składający
 * się z jednomianów znajdujących się w tej tablicy, korzystając
 * z funkcji pomocniczej @p BuildPolyFromMonos.
 * @sa BuildPolyFromMonos
 */
static inline Poly MulCoeffPoly(const Poly *p, const Poly *q) {
  // Tablica jednomianów wielomianu wyjściowego
  Mono *newArr = malloc(q->size * sizeof(Mono));

  CHECK_PTR(newArr);

  // Wielomian tymczasowy
  Poly tmp;
  // Indeks, wskazujący na miejsce w tablicy, do którego zapisywane
  // są kolejne jednomiany
  size_t index = 0;

  for (size_t i = 0; i < q->size; i++) {
    // Mnoży wielomian stały przez każdy z wielomianów
    // odpowiadających jednomianom, z który składa się wielomian q
    tmp = PolyMul(p, &q->arr[i].p);

    // Dodaje pomnożony jednomian do tablicy
    // jednomianów wielomianu wyjściowego
    if (!PolyIsZero(&tmp)) {
      newArr[index] = (Mono) {
        .exp = MonoGetExp(&q->arr[i]),
        .p = tmp
      };
      index++;
    }
    // Możliwy scenariusz w przypadku overflow współczynników
    else {
      PolyDestroy(&tmp);
    }
  }

  // Zmienna index jest aktualnie równa liczbie jednomianów (niezerowych)
  // w tablicy newArr

  // Analizuje newArr i zwraca wielomian składający się z jednomianów
  // znajdujących się w tablicy newArr
  return BuildPolyFromMonos(newArr, index, q->size);
}

/**
 * Sprawdza, czy żaden ze wskaźników na wielomiany nie jest
 * pustym wskaźnikiem. Następnie dopasowuje wielomiany do odpowiedniego
 * przypadku; każdy z nich jest bowiem albo wielomianem stałym, albo nie.
 * Następnie zwraca wynik obliczony za pomocą odpowiedniej funkcji pomocniczej
 * lub (w przypadku, gdy oba wielomiany nie są stałe) mnoży każdy jednomian
 * wielomianu @p p z każdym jednomianem wielomianu @p q i poszczególne wyniki
 * cząstkowe zapisuje do tablicy. Następnie sumuje je za pomocą funkcji
 * @p PolyOwnMonos, zwalnia stworzoną wcześniej tablicę i zwraca wynik.
 * @sa PolyOwnMonos, MulCoeffPoly
 */
Poly PolyMul(const Poly *p, const Poly *q) {
  assert(p != NULL && q != NULL);

  if (PolyIsCoeff(p) && PolyIsCoeff(q)) {
    return PolyFromCoeff(p->coeff * q->coeff);
  }
  else if (PolyIsCoeff(p)) {
    if (p->coeff == 0) {
      return PolyZero();
    }

    return MulCoeffPoly(p, q);
  }
  else if (PolyIsCoeff(q)) {
    return PolyMul(q, p);
  }
  else {
    Mono *newArr = malloc((p->size * q->size) * sizeof(Mono));

    CHECK_PTR(newArr);

    // Mnoży każdy jednomian z każdym
    for (size_t i = 0; i < p->size; i++) {
      for (size_t j = 0; j < q->size; j++) {
        newArr[q->size * i + j] = (Mono) {
          .exp = p->arr[i].exp + q->arr[j].exp,
          .p = PolyMul(&p->arr[i].p, &q->arr[j].p)
        };
      }
    }

    // Sumuje obliczone jednomiany i tworzy z nich wielomian
    return PolyOwnMonos(p->size * q->size, newArr);
  }
}

//////////////////////////
//                      //
//       PolyNeg        //
//                      //
//////////////////////////

/**
 * Tworzy wielomian stale równy @p -1, a następnie zwraca jego
 * iloczyn z oryginalnym wielomianem. Wielomian tożsamościowo
 * równy @p -1 jest usuwany.
 */
Poly PolyNeg(const Poly *p) {
  Poly tmp = PolyFromCoeff(-1);
  Poly result = PolyMul(p, &tmp);

  PolyDestroy(&tmp);

  return result;
}

//////////////////////////
//                      //
//       PolySub        //
//                      //
//////////////////////////

/**
 * Zwraca różnicę wielomianu stałego i wielomianu nie będącego wielomianem
 * stałym, który posiada niezerowy wyraz wolny. Zakłada, że wskaźniki
 * na wielomiany nie są puste.
 * @param[in] p : wielomian stały
 * @param[in] q : wielomian nie będący wielomianem stałym mający wyraz wolny
 * @return @f$p - q@f$
 * 
 * @details
 * Oblicza wyraz wolny (jednomian o wykładniku równym zeru) wielomianu
 * wyjściowego. Jeśli jest tożsamościowo równy zeru, usuwa go, a następnie
 * alokuje tablicę jednomianów o rozmiarze mniejszym o 1 od oryginalnej.
 * Przypisuje do niej jednomiany przeciwne do oryginalnych (z pominięciem
 * wyrazu wolnego) i zwraca wynik. Jeśli zaś wyraz wolny wielomianu
 * wyjściowego jest różny od zera, tablica jednomianów ma ten sam rozmiar.
 * Pozostałe czynności są analogiczne. Należy zwrócić uwagę, że wynikiem
 * tej funkcji nie może być wielomian stały.
 */
static inline Poly SubConstPoly0(const Poly *p, const Poly *q) {
  // Tablica jednomianów nowego wielomianu
  Mono *newArr;
  // Obliczanie jednomianu o zerowym wykładniku
  Mono zeroMono = (Mono) {.p = PolySub(p, &q->arr[0].p), .exp = 0};

  // Wyraz wolny w nowym wielomianie jest równy 0
  if (PolyIsZero(&zeroMono.p)) {
    MonoDestroy(&zeroMono);

    // Tablica o rozmiarze o 1 mniejszym od oryginalnej (brak wyrazu
    // wolnego). Jeśli zeroMono.p == 0, to wyraz wolny wielomianu q musiał
    // być funkcją stałą. Jeśli jednak q nie jest wielomianem stałym,
    // to q->size > 1 (istnieje co najmniej jeden jednomian nie będący
    // tożsamościowo równy żadnej funkcji stałej)
    newArr = malloc((q->size - 1) * sizeof(Mono));

    CHECK_PTR(newArr);

    // Uzupełnianie tablicy jednomianów jednomianami przeciwnymi
    // do oryginalnych
    for (size_t i = 0; i < q->size - 1; i++) {
      newArr[i] = (Mono) {
        .p = PolyNeg(&q->arr[i + 1].p),
        .exp = MonoGetExp(&q->arr[i + 1])
      };
    }

    return (Poly) {.size = q->size - 1, .arr = newArr};
  }
  // Wyraz wolny w nowym wielomianie jest niezerowy
  else {
    newArr = malloc(q->size * sizeof(Mono));

    CHECK_PTR(newArr);

    // Wyraz wolny nowego wielomianu
    newArr[0] = zeroMono;

    // Uzupełnianie tablicy jednomianów jednomianami przeciwnymi
    // do oryginalnych
    for (size_t i = 1; i < q->size; i++) {
      newArr[i] = (Mono) {
        .p = PolyNeg(&q->arr[i].p),
        .exp = MonoGetExp(&q->arr[i])
      };
    }

    return (Poly) {.size = q->size, .arr = newArr};
  }
}

/**
 * Zwraca różnicę wielomianu stałego i wielomianu nie będącego wielomianem
 * stałym. Zakłada, że wskaźniki na wielomiany nie są puste.
 * @param[in] p : wielomian stały
 * @param[in] q : wielomian nie będący wielomianem stałym
 * @return @f$p - q@f$
 * 
 * @details
 * Sprawdza, czy wielomian @p q posiada niezerowy wyraz wolny (jednomian
 * o wykładniku równym zeru). Jeśli tak, to zwraca wynik funkcji pomocniczej
 * @p SubConstPoly0 wywołanej dla tych samych argumentów. W przeciwnym razie
 * wielomian @p q nie posiada wyrazu wolnego. Wynika z tego, że tablica
 * jednomianów wielomianu wyjściowego ma rozmiar o 1 większy od oryginalnej.
 * Wyraz wolny jest tożsamościowo równy otrzymanemu wielomianowi stałemu,
 * zaś pozostałe jednomiany są przeciwne do tych, z których składa się
 * wielomian @p q
 * 
 * @sa SubConstPoly0
 */
static inline Poly SubConstPoly(const Poly *p, const Poly *q) {
  // Przypadek, gdy wielomian q posiada niezerowy wyraz wolny
  if (MonoGetExp(&q->arr[0]) == 0) {
    return SubConstPoly0(p, q);
  }
  // Przypadek, gdy wielomian q nie posiada wyrazu wolnego
  else {
    // Tablica jednomianów nowego wielomianu
    Mono *newArr = malloc((1 + q->size) * sizeof(Mono));

    CHECK_PTR(newArr);

    // Wyraz wolny nowego wielomianu
    newArr[0] = (Mono) {.p = PolyClone(p), .exp = 0};

    // Uzupełnienie tablicy jednomianów nowego wielomianu jednomianami
    // przeciwnymi do oryginalnych
    for (size_t i = 1; i <= q->size; i++) {
      newArr[i] = (Mono) {
        .p = PolyNeg(&q->arr[i - 1].p),
        .exp = MonoGetExp(&q->arr[i - 1])
      };
    }

    return (Poly) {.size = 1 + q->size, .arr = newArr};
  }
}

/**
 * Oblicza i zwraca różnicę wielomianu nie będącego wielomianem stałym, który
 * posiada niezerowy wyraz wolny, oraz wielomianu stałego. Zakłada, że
 * wskaźniki na wielomiany są niepuste.
 * @param[in] p : wielomian nie będący wielomianem stałym, który ma niezerowy
 * wyraz wolny
 * @param[in] q : wielomian stały
 * @return @f$p - q@f$
 * 
 * @details
 * Oblicza wyraz wolny nowego wielomianu. W zależności od tego, czy jest
 * tożsamościowo równy zeru, czy nie, alokuje odpowiednią ilość pamięci
 * na tablicę jednomianów nowego wielomianu. Następnie ją uzupełnia
 * i zwraca wynik.
 */
static inline Poly SubPolyConst0(const Poly *p, const Poly *q)
{
  // Tablica jednomianów wielomianu wyjściowego
  Mono *newArr;
  // Wyraz wolny wielomianu wyjściowego
  Mono zeroMono = (Mono) {.p = PolySub(&p->arr[0].p, q), .exp = 0};

  // Wyraz wolny jest równy zeru
  if (PolyIsZero(&zeroMono.p)) {
    MonoDestroy(&zeroMono);

    if (p->size > 1) {
      // Wielomian wyjściowy ma o jeden jednomian mniej od wielomianu p
      newArr = malloc((p->size - 1) * sizeof(Mono));

      CHECK_PTR(newArr);

      // Pozostałe jednomiany są takie same
      for (size_t i = 0; i < p->size - 1; i++) {
        newArr[i] = (Mono) {
          .p = PolyClone(&p->arr[i + 1].p),
          .exp = MonoGetExp(&p->arr[i + 1])
        };
      }

      return (Poly) {.size = p->size - 1, .arr = newArr};
    }
    else {
      return PolyZero();
    }
  }
  // Wyraz wolny wielomianu wyjściowego jest różny od zera
  else {
    // Wielomian wyjściowy ma taką samą liczbę jednomianów co wielomian p
    newArr = malloc(p->size * sizeof(Mono));

    CHECK_PTR(newArr);

    // Wyraz wolny wielomianu wyjściowego
    newArr[0] = zeroMono;

    // Pozostałe jednomiany są takie same
    for (size_t i = 1; i < p->size; i++) {
      newArr[i] = (Mono) {
        .p = PolyClone(&p->arr[i].p),
        .exp = MonoGetExp(&p->arr[i])
      };
    }

    return (Poly) {.size = p->size, .arr = newArr};
  }
}

/**
 * Oblicza i zwraca różnicę wielomianu nie będącego wielomianem stałym
 * i wielomianu stałego różnego od zera.
 * @param[in] p : wielomian nie będący wielomianem stałym
 * @param[in] q : wielomian stały różny od zera
 * @return @f$p - q@f$
 * 
 * @details
 * Jeśli wielomian @p p nie ma wyrazu wolnego, to deklaruje tablicę
 * jednomianów o rozmiarze o 1 większej od oryginalnej (wielomianu @p p).
 * Wyraz wolny nowego wielomianu jest tożsamościowo równy wielomanowi @p q.
 * Pozostałe jednomiany pozostają bez zmian.
 * Jeśli @p p posiada niezerowy wyraz wolny, to zwraca wynik obliczony
 * za pomocą funkcji pomocniczej @p SubPolyConst0.
 * @sa SubPolyConst0
 */
static inline Poly SubPolyConst(const Poly *p, const Poly *q) {
  // Wielomian p nie ma wyrazu wolnego
  if (MonoGetExp(&p->arr[0]) != 0) {
    // Tablica jednomianów wielomianu wyjściowego
    Mono *newArr;
    newArr = malloc((1 + p->size) * sizeof(Mono));

    CHECK_PTR(newArr);

    // Wyraz wolny wielomianu wyjściowego
    newArr[0] = (Mono) {.p = PolyNeg(q), .exp = 0};

    for (size_t i = 1; i <= p->size; i++) {
      newArr[i] = (Mono) {
        .p = PolyClone(&p->arr[i - 1].p),
        .exp = MonoGetExp(&p->arr[i - 1])
      };
    }

    return (Poly) {.size = 1 + p->size, .arr = newArr};
  }
  // Wielomian p posiada niezerowy wyraz wolny
  else {
    return SubPolyConst0(p, q);
  }
}

/**
 * Rozważa dwa jednomiany znajdujące się na wskazanych pozycjach w tablicach
 * dwóch niestałych wielomianów. Wybiera ten, który ma mniejszy wykładnik
 * i zwraca jego kopię (w przypadku jednomianu z tablicy wielomianu @p p)
 * lub jednomian do niego przeciwny (w przypadku jednomianu z tablicy
 * wielomianu @p q). Jeśli zaś oba jednomiany mają ten sam wykładnik,
 * oblicza ich różnicę i zwraca ją. Następnie modyfikuje wartość tego indeksu,
 * który odnosi się do zwracanego jednomianu, zwiększając jego wartość
 * o 1 lub, w przypadku, gdy zwracana jest różnica jednomianów, zwiększa
 * wartości obu z nich.
 * Funkcja zakłada, że żaden ze wskaźników nie jest pusty
 * oraz że co najmniej jeden z indeksów mieści się w rozmiarze odowiadającej
 * mu tablicy (nie dotyczy to natomiast obu z nich jednocześnie).
 * @param[in] p : wielomian nie będący wielomianem stałym
 * @param[in] q : wielomian nie będący wielomianem stałym
 * @param[in,out] i : indeks odnoszący się do tablicy jednomianów
 * wielomianu @p p
 * @param[in,out] j : indeks odnoszący się do tablicy jednomianów
 * wielomianu @p q
 * @return ten spośród jednomianów @p p->arr[*i], (-1) * @p q->arr[*j],
 * który ma mniejszy wykładnik (lub ich suma, jeśli są one równe).
 * 
 * @details
 * Rozważa trzy przypadki. Jeśli oba indeksy mieszczą się w zakresie
 * odpowiadających im tablic lub jeśli tylko jeden z nich spełnia ten warunek.
 * Jeśli wykładniki odpowiednich jednomianów są różne, to w przypadku, gdy
 * jednomian z tablicy wielomianu @p p ma mniejszy wykładnik, zwraca jego
 * kopię, zwiększając wcześniej wartość indeksu @p i o 1. W przypadku, gdy
 * jedomian z tablicy wielomianu @p q ma mniejszy wykładnik, zwraca
 * jednomian do niego przeciwny i zwiększa wartość indeksu @p j o 1. Jeżeli
 * zaś oba jednomiany mają ten sam wykładnik, zwraca różnicę jednomianu
 * z tablicy wielomianu @p p i jednomianu z tablicy wielomianu @p q i zwiększa
 * o 1 wartości obu indeksów.
 */
static inline Mono NextMonoSub(const Poly *p, const Poly *q,
                               size_t *i, size_t *j) {
  // Wyjściowy jednomian
  Mono monomial;

  // Oba indeksy mieszczą się w zakresie odpowiadającym im tablic
  if (*i < p->size && *j < q->size) {
    if (MonoGetExp(&p->arr[*i]) < MonoGetExp(&q->arr[*j])) {
      monomial = MonoClone(&p->arr[*i]);
      *i = *i + 1;
    }
    else if (MonoGetExp(&p->arr[*i]) > MonoGetExp(&q->arr[*j])) {
      monomial = (Mono) {
        .p = PolyNeg(&q->arr[*j].p),
        .exp = MonoGetExp(&q->arr[*j])
      };

      *j = *j + 1;
    }
    else {
      // Oblicza różnicę jednomianów
      monomial = (Mono) {
        .p = PolySub(&p->arr[*i].p, &q->arr[*j].p),
        .exp = MonoGetExp(&p->arr[*i])
      };

      *i = *i + 1;
      *j = *j + 1;
    }
  }
  // Indeks `j` wyszedł poza zakres swojej tablicy
  else if (*i < p->size) {
    monomial = MonoClone(&p->arr[*i]);
    *i = *i + 1;
  }
  // Indeks `i` wyszedł poza zakres swojej tablicy
  else {
    monomial = (Mono) {
      .p = PolyNeg(&q->arr[*j].p),
      .exp = MonoGetExp(&q->arr[*j])
    };
    *j = *j + 1;
  }

  return monomial;
}

/**
 * Zwraca różnicę dwóch wielomianów nie będących wielomianami stałymi.
 * Zakłada, że przekazane wskaźniki na wielomiany nie są puste.
 * @param[in] p : wielomian nie będący wielomianem stałym
 * @param[in] q : wielomian nie będący wielomianem stałym
 * @return @f$p - q@f$
 * 
 * @details
 * Oblicza liczbę unikalnych wykładników wśrod jednomianów, z których
 * składają się wielomiany @p p i @p q, a następnie alokuje tablicę
 * jednomianów o takim rozmiarze. Rozważa kolejne jednomiany z tablic
 * jednomianów wielomianów @p p i @p q i zapisuje ich kopie
 * (lub jednomiany do nich przeciwne) w nowo stworzonej tablicy
 * jednomianów (jeśli mają te same wykładniki, zapisuje zamiast tego
 * ich różnicę). Służy do tego funkcja pomocnicza @p NextMonoSub.
 * Ostatecznie jednomiany w nowej tablicy są uporządkowane w sposób
 * rosnący ze względu na wykładniki, a wśród nich nie ma żadnego
 * tożsamościowo równego zeru. Zwraca następnie wielomian złożony
 * z tych jednomian, stworzny za pomocą funkcji
 * pomocniczej @p BuildPolyFromMonos.
 * @sa NextMonoSub, BuildPolyFromMonos, SumPolyPoly
 */
static inline Poly SubPolyPoly(const Poly *p, const Poly *q) {
  // Liczba parami różnych wykładników wśród jednomianów
  // z tablic jednomianów wielomianów wejściowych
  const size_t newSize = NumOfUniqueExps(p, q);

  // Tablica jednomianów wyjściowego wielomianu
  Mono *newArr = malloc(newSize * sizeof(Mono));

  // Indeksy rozważanych aktualnie jednomianów
  // -- odpowiednio w tablicy wielomianu p i q
  size_t i = 0, j = 0;
  // Indeks, pod którym są zapisywane kolejne jednomiany w tablicy newArr
  size_t index = 0;
  // Jednomian pomocniczy
  Mono tmpMonomial;

  while (i < p->size || j < q->size) {
    // Wybór kolejnego do zapisania w tablicy jednomianu
    // i modyfikacja indeksów `i` oraz `j`
    tmpMonomial = NextMonoSub(p, q, &i, &j);

    //Aktualizacja tablicy jednomianów
    if (!PolyIsZero(&tmpMonomial.p)) {
      newArr[index] = tmpMonomial;
      index++;
    }
    // Zwolnienie nieużywanej pamięci
    else {
      MonoDestroy(&tmpMonomial);
    }
  }

  // W tym momencie zmienna `index` jest równa liczbie jednomianów
  // w tablicy newArr

  // Analizuje newArr i zwraca wielomian składający się z jednomianów
  // znajdujących się w tablicy newArr
  return BuildPolyFromMonos(newArr, index, newSize);
}

/**
 * Sprawdza za pomocą asercji, czy żaden ze wskaźników na wielomiany
 * nie jest pustym wskaźnikiem.
 * Następnie sprawdza rodzaj każdego z wielomianu: wielomian stały / niestały,
 * i oblicza różnicę @f$p - q@f$ za pomocą jednej z funkcji pomocniczych:
 * @p PolyFromCoeff, @p SubConstPoly, @p SubPolyConst, @p SubPolyPoly
 * @sa PolyFromCoeff, SubConstPoly, SubPolyConst, SubPolyPoly
 */
Poly PolySub(const Poly *p, const Poly *q) {
  assert(p != NULL && q != NULL);

  if (PolyIsCoeff(p) && PolyIsCoeff(q)) {
    return PolyFromCoeff(p->coeff - q->coeff);
  }
  else if (PolyIsCoeff(p)) {
    if (PolyIsZero(p)) {
      return PolyNeg(q);
    }

    return SubConstPoly(p, q);
  }
  else if (PolyIsCoeff(q)) {
    if (PolyIsZero(q)) {
      return PolyClone(p);
    }

    return SubPolyConst(p, q);
  }
  else {
    return SubPolyPoly(p, q);
  }
}

///////////////////////////
//                       //
//       PolyDegBy       //
//                       //
///////////////////////////

/**
 * Sprawdza, czy wskaźnik na wielomian nie jest pusty za pomocą asercji.
 * Jeśli wielomian jest stały, to zachodzi jeden z następujących przypadków: 
 * a) wielomian jest tożsamościowo równy 0 -- wówczas wynikiem jest -1; 
 * b) w przeciwnym wypadku wynikiem jest 0, bo jest to stopień wielomianu 
 * stałego nie będącego zerem. 
 * Jeśli zaś wielomian nie jest wielomianem stałym, to zachodzi jeden
 * z dwóch przypadków: 
 * a) rozpatruje "poziom" wielomianu, na którym znajdują się zmienne 
 * o indeksie @p var_idx, 
 * b) rozpatruje "niższy poziom" wielomianu niż @p var_idx
 * -- musi się zagłębić w poszczególne jednomiany.
 */
poly_exp_t PolyDegBy(const Poly *p, size_t var_idx) {
  assert(p != NULL);

  if (PolyIsCoeff(p)) {
    if (PolyIsZero(p)) { return -1; }
    else               { return  0; }
  }
  else
  {
    // Maksymalny znaleziony wykładnik zmiennej o indeksie var_idx
    poly_exp_t maxExp = -1;

    // Rozpatruje "poziom" wielomianu, na którym znajdują się zmienne
    // o indeksie var_idx
    if (var_idx == 0) {
      for (size_t i = 0; i < p->size; i++) {
        if (MonoGetExp(&p->arr[i]) > maxExp) {
          maxExp = MonoGetExp(&p->arr[i]);
        }
      }
    }
    // Rozpatruje "niższy poziom" wielomianu niż var_idx.
    // Funkcja zagłębia się w poszczególne jednomiany
    else {
      // Tymczasowy wynik z poszczególnych jednomianów
      poly_exp_t tmpResult;

      for (size_t i = 0; i < p->size; i++) {
        tmpResult = PolyDegBy(&p->arr[i].p, var_idx - 1);

        // Aktualizujemy wynik
        if (tmpResult > maxExp) {
          maxExp = tmpResult;
        }
      }
    }

    return maxExp;
  }
}

//////////////////////////
//                      //
//       PolyDeg        //
//                      //
//////////////////////////

/**
 * Jeśli wielomian jest stały, to zwraca odpowiedni stopień. 
 * W przypadku wielomianu nie będącego wielomianem stałym, sumuje 
 * wykładnik każdego z jednomianów z wynikiem otrzymanym z odpowiadającego
 * mu wielomianu i aktualizuje wynik. Następnie zwraca najwyższy z nich.
 */
poly_exp_t PolyDeg(const Poly *p) {
  assert(p != NULL);

  if (PolyIsCoeff(p)) {
    if (PolyIsZero(p)) { return -1; }
    else               { return  0; }
  }
  else {
    // Największy znaleziony dotąd wynik
    poly_exp_t maxExp = -1;
    poly_exp_t tmpResult;

    for (size_t i = 0; i < p->size; i++) {
      // Nie jest to możliwe, aby któryś z jednomianów był
      // tożsamościowo równy zeru
      tmpResult = PolyDeg(&p->arr[i].p) + MonoGetExp(&p->arr[i]);

      //Aktualizuje wynik
      if (tmpResult > maxExp) {
        maxExp = tmpResult;
      }
    }

    return maxExp;
  }
}

///////////////////////////
//                       //
//       PolyIsEq        //
//                       //
///////////////////////////

/**
 * Sprawdza, czy żaden ze wskaźników na wielomiany
 * nie jest pustym wskaźnikiem.
 * Następnie bada rodzaj wielomianów: wielomian stały / niestały.
 * Jeśli nie są tego samego rodzaju -- nie mogą być równe.
 * Jeśli oba są wielomianami stałymi -- porównuje ich współczynniki.
 * Jeśli oba są wielomianami niestałymi -- porównuje każdy z ich jednomianów.
 */
bool PolyIsEq(const Poly *p, const Poly *q) {
  assert(p != NULL && q != NULL);

  if (PolyIsCoeff(p) && PolyIsCoeff(q)) {
    return p->coeff == q->coeff;
  }
  else if (PolyIsCoeff(p) || PolyIsCoeff(q)) {
    return false;
  }
  else {
    // Jeśli wielomiany nie mają takiej samej liczby jednomianów,
    // nie mogą być równe
    if (p->size != q->size) {
      return false;
    }

    // Porównywanie każdego z jednomianów. Najpierw ich wykładniki,
    // a potem równość wielomianów, z których są złożone
    for (size_t i = 0; i < p->size; i++) {
      if (MonoGetExp(&p->arr[i]) != MonoGetExp(&q->arr[i])) {
        return false;
      }
      else if (!PolyIsEq(&p->arr[i].p, &q->arr[i].p)) {
        return false;
      }
    }

    return true;
  }
}

//////////////////////////
//                      //
//        PolyAt        //
//                      //
//////////////////////////

/**
 * Zwraca wynik potęgowania liczby całkowitej do naturalnej potęgi. 
 * Zakłada, że @f$0^0 = 1@f$. Korzysta z algortymu szybkiego potęgowania.
 * @param[in] base : podstawa potęgi
 * @param[in] exp : wykładnik
 * @return @f$base ^ { exp }@f$
 */
static inline poly_coeff_t FastExp(poly_coeff_t base, poly_exp_t exp) {
  poly_coeff_t accumulator = 1;

  while (exp > 0) {
    if (exp % 2 != 0) {
      accumulator *= base;
    }

    base *= base;
    exp /= 2;
  }

  return accumulator;
}

/**
 * Na początku sprawdza, czy wskaźnik na wielomian nie jest pusty.
 * Jeśli wielomian jest stały, to zwraca jego kopię (w każdym punkcie
 * jest sobie równy). W przeciwnym wypadku, rozważa dwa przypadki. Jeśli
 * argument jest równy zeru, zwraca wielomian odpowiadający jednomianowi
 * o wykładniku równym zeru (lub wielomian zerowy, jeśli takiego nie posiada).
 * Inaczej argument jest różny od zera. Wówczas, mając jednomian @f$px_i^k@f$
 * oblicza @f$p \cdot x^k@f$ dla każdego z jednomianów, a następnie sumuje je.
 * Jest to wynik.
 */
Poly PolyAt(const Poly *p, poly_coeff_t x) {
  assert(p != NULL);

  if (PolyIsCoeff(p)) {
    return PolyClone(p);
  }
  else {
    // Jako że wykładniki jednomianów nie powtarzają się,
    // wystarczy rozpatrzeć tylko pierwszy jednomian
    if (x == 0) {
      // Jeśli wykładnik jest równy 0, to z równości 0^0 = 1 wynika,
      // że wynikiem będzie kopia wielomianu,
      // z którego składa się ten jednomian
      if (MonoGetExp(&p->arr[0]) == 0) {
        return PolyClone(&p->arr[0].p);
      }
      // W przeciwnym przypadku (gdy nie ma jednomianu o zerowym
      // wykładniku) wynikiem jest wielomian zerowy
      else {
        return PolyZero();
      }
    }
    else {
      // Zmienne tymczasowe dla wielomianów
      Poly tmp1, tmp2;
      // Wartość wielomianu w danym punkcie -- wynik
      Poly result = PolyZero();

      for (size_t i = 0; i < p->size; i++) {
        // Oblicza x^k, gdzie k to wartość wykładnika
        // dla danego jednomianu
        tmp1 = PolyFromCoeff(FastExp(x, p->arr[i].exp));
        // Mnoży wielomian, z którego składa się dany jednomian,
        // z wynikiem potęgowania
        tmp1 = PolyMul(&p->arr[i].p, &tmp1);
        // Sumuje aktualny wynik z nowym wielomianem
        tmp2 = PolyAdd(&result, &tmp1);
        // Zwalnia nieużywaną już pamięć
        PolyDestroy(&result);
        // Aktualizuje wynik
        result = tmp2;
        // Zwalnia nieużywaną pamięć
        PolyDestroy(&tmp1);
      }

      return result;
    }
  }
}


//////////////////////////
//                      //
//      PolyCompose     //
//                      //
//////////////////////////


/**
 * Podnosi wielomian do potęgi. Zakłada, ze przekazany wskaźnik na wielomian
 * wskazuje na istniejącą i poprawną strukturę danych. Zakłada także, że
 * @f$0 ^ 0 = 1@f$.
 * @param[in] p : wielomian
 * @param[in] exp : wartość wykładnika
 * @return @f$p ^ { exp }@f$
 * 
 * @details
 * Funkcja korzysta z algorytmu szybkiego potęgowania. Traktuje ujemne
 * wykładniki jako dodatnie.
 */
static inline Poly PolyFastExp(const Poly *p, poly_exp_t exp) {
  if (exp == 0) {
    return PolyFromCoeff(1);
  }
  else if (exp == 1) {
    return PolyClone(p);
  }

  // Akumulator
  Poly acc = PolyFromCoeff(1);
  // Wielomian wynikowy
  Poly result = PolyClone(p);

  while (exp != 0) {
    Poly tmp;

    if (exp % 2 != 0) {
      tmp = PolyMul(&acc, &result);
      PolyDestroy(&acc);
      acc = tmp;
    }

    // Jeśli `exp` jest równy 1, to nie ma sensu podnosić jeszcze raz
    // wielomianu do kwadratu, bo i tak nie wykorzystamy go przy
    // następnym wykonaniu pętli
    if (exp != 1) {
      tmp = PolyMul(&result, &result);
      PolyDestroy(&result);
      result = tmp;
    }

    exp /= 2;
  }

  PolyDestroy(&result);

  return acc;
}

/**
 * Mnoży wielomiany @f$p@f$ i @f$q@f$, usuwa je z pamięci i zwraca wynik
 * mnożenia. Funkcja zakłada, że przekazane wskaźniki wskazują na istniejące
 * i poprawne struktury danych.
 * @param[in] p : wielomina
 * @param[in] q : wielomian
 * @return @f$p * q@f$
 */
static inline Poly SmartPolyMul(Poly *p, Poly *q) {
  Poly tmp = PolyMul(p, q);
  PolyDestroy(p);
  PolyDestroy(q);
  
  return tmp;
}

/**
 * Dodaje wielomiany @f$p@f$ i @f$q@f$, usuwa je z pamięci i zwraca wynik
 * dodawania. Funkcja zakłada, że przekazane wskaźniki wskazują na istniejące
 * i poprawne struktury danych.
 * @param[in] p : wielomina
 * @param[in] q : wielomian
 * @return @f$p + q@f$
 */
static inline Poly SmartPolyAdd(Poly *p, Poly *q) {
  Poly tmp = PolyAdd(p, q);
  PolyDestroy(p);
  PolyDestroy(q);

  return tmp;
}

/**
 * Oblicza wartość złożenia wielomianu @f$p@f$ z @f$k@f$ wielomianami,
 * z których składa się tablica @p q. Funkcja zakłada, że przekazane
 * wskaźniki wskazują na istniejące i poprawne struktury danych.
 * @param[in] p : wielomian
 * @param[in] level : indeks głównej zmiennej w wielomianie @f$p@f$
 * @param[in] k : liczba wielomianów w tablicy @p q
 * @param[in] q : tablica wielomianów
 * @return Złożenie wielomianu @f$p@f$ z wielomianami z tablicy @p q.
 */
static inline Poly AuxPolyCompose(const Poly *p, const size_t level,
                                  const size_t k, const Poly q[]) {
  if (PolyIsCoeff(p)) {
    return PolyClone(p);
  }
  else if (level + 1 > k || PolyIsZero(&q[level])) {
    if (MonoGetExp(&p->arr[0]) == 0) {
      return AuxPolyCompose(&p->arr[0].p, level + 1, k, q);
    }
    else {
      return PolyZero();
    }
  }

  // Suma wyników jednomianów, z których składa się `p`; wynik funkcji
  Poly sum = PolyZero();
  // Wielomiany pomocnicze
  Poly tmp, tmp2;
  // Wielomian `q[level]` podniesiony do kolejnych potęg odpowiadających
  // jednomianom, z których składa się wielomian `p`
  Poly exp = PolyFromCoeff(1);
  // Wartość wykładnika ostatniego analizowanego jednomianu
  poly_exp_t expVal = 0;
  
  for (size_t i = 0; i < p->size; i++) {
    // Wielomian po złożeniu z wielomianami z tablicy `q`
    tmp = AuxPolyCompose(&p->arr[i].p, level + 1, k, q);
    
    if (!PolyIsZero(&tmp)) {
      tmp2 = PolyFastExp(&q[level], MonoGetExp(&p->arr[i]) - expVal);
      if (!PolyIsZero(&tmp2)) {
        exp = SmartPolyMul(&exp, &tmp2);
        // Aktualizacja wartości wykładnika
        expVal = MonoGetExp(&p->arr[i]);
        if (!PolyIsZero(&exp)) {
          // Obliczenie wartości tego jednomianu po podstawieniu
          // wielomianu `q[level]`
          tmp2 = PolyMul(&tmp, &exp);
          PolyDestroy(&tmp);

          if (!PolyIsZero(&tmp2)) {
            // Sumowanie wyniku
            sum = SmartPolyAdd(&sum, &tmp2);
          }
        }
        else {
          PolyDestroy(&tmp);
        }
      }
      else {
        PolyDestroy(&tmp);
      }
    }
  }

  PolyDestroy(&exp);

  return sum;
}

/**
 * Wywołuje funkcję @p AuxPolyCompose dla wielomianu @f$p@f$, zaczynając
 * od podstawienia pierwszego z wielomianów pod zmienną @f$x_0@f$. Następnie
 * zwraca wynik.
 */
Poly PolyCompose(const Poly *p, size_t k, const Poly q[]) {
  return AuxPolyCompose(p, 0, k, q);
}


