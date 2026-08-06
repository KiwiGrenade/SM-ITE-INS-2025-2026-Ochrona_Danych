#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <iostream>

// Zmienne globalne
long p, q, n, Klucz_d, Klucz_e;
long ns = 9; // zakres wartosci dla testu Fermata

// --- FUNKCJE MATEMATYCZNE ---

int odd(int p) {
    return p % 2;
}

// Funkcja obliczająca potęgę modulo: x^e mod n
long potega_mod(long x, long e, long p) {
    long wynik, c, lokalne_q;
    c = x;
    wynik = 1;
    lokalne_q = e;
    do {
        if (odd(lokalne_q)) wynik = (wynik * c) % p;
        c = (c * c) % p;
        lokalne_q = lokalne_q >> 1;
    } while (!(lokalne_q == 0));
    return wynik;
}

// Test pierwszości Fermata
long liczba_pierwsza(long p) {
    long q, x;
    q = p - 1;
    x = 1;
    do {
        while ((potega_mod(x, q, p) != 1)) {
            q = p;
            p = p + 1;
            x = 1;
        }
        x = x + 1;
    } while (!(x >= ns));
    return p;
}

// Rozszerzony algorytm Euklidesa dla odwrotności
long multi_inv(long x, long p) {
    long inv, m, n_local, qt, r, rt, s, st, t;
    qt = 1; rt = 0; st = p;
    m = 0; r = 1; s = x;
    while (s != 0) {
        n_local = st / s;
        t = m;
        m = qt - n_local * m;
        qt = t;
        t = r;
        r = rt - n_local * r;
        rt = t;
        t = s;
        s = st % s;
        st = t;
    }
    inv = rt;
    if (inv < 0) inv = inv + p;
    return inv;
}

// NWD
long gcd_euclid(long x, long y) {
    long reszta;
    reszta = x % y;
    while (reszta != 0) {
        x = y;
        y = reszta;
        reszta = x % y;
    }
    return y;
}

// --- GŁÓWNA FUNKCJA ---

int main() {
    long f;
    srand((unsigned)time(0));

    // 1. Generowanie liczb pierwszych p i q
    printf("--- Zadanie 1: Pojedynczy Znak (Linux) ---\n");
    printf("Generowanie kluczy RSA...\n");
    
    p = rand() % 200 + 16; 
    if (!odd(p)) p = p + 1;
    p = liczba_pierwsza(p);

    q = rand() % 200 + 16;
    if (!odd(q)) q = q + 1;
    q = liczba_pierwsza(q);

    // 2. Obliczanie n i f (phi)
    n = p * q;
    f = (p - 1) * (q - 1);

    // 3. Generowanie klucza publicznego e
    do {
        Klucz_e = rand() % 32500;
        Klucz_e += rand() % 32500 + 1L;
        if (!odd(Klucz_e)) Klucz_e = Klucz_e + 1;
    } while (!(gcd_euclid(Klucz_e, f) == 1));

    // 4. Generowanie klucza prywatnego d
    Klucz_d = multi_inv(Klucz_e, f);

    printf("Klucze wygenerowane:\n");
    printf("p=%ld, q=%ld\n", p, q);
    printf("Klucz Publiczny (e, n): (%ld, %ld)\n", Klucz_e, n);
    printf("Klucz Prywatny  (d, n): (%ld, %ld)\n", Klucz_d, n);
    printf("--------------------------------------------------\n");

    // --- REALIZACJA ZADANIA 1 ---
    
    char inputChar;
    printf("\n[Zadanie 1] Podaj jeden znak do zaszyfrowania: ");
    if (scanf(" %c", &inputChar) != 1) {
        printf("Blad wczytywania znaku.\n");
        return 1;
    }

    long M = (long)inputChar; // Konwersja znaku na liczbę
    
    // SZYFROWANIE: C = M^e (mod n)
    long C = potega_mod(M, Klucz_e, n);

    printf("\nWyniki operacji:\n");
    printf("1. Wiadomosc jawna (M): '%c' (ASCII: %ld)\n", inputChar, M);
    
    // WYDRUK KRYPTOGRAMU W HEX
    printf("2. Kryptogram (C) HEX : 0x%X (DEC: %ld)\n", (unsigned int)C, C);

    // DESZYFROWANIE: M = C^d (mod n)
    long M_decrypted = potega_mod(C, Klucz_d, n);

    printf("3. Po deszyfrowaniu   : '%c' (ASCII: %ld)\n", (char)M_decrypted, M_decrypted);

    // Weryfikacja poprawności
    if (M == M_decrypted) {
        printf("\nSUKCES: Wiadomosc odszyfrowana jest identyczna z oryginalem.\n");
    } else {
        printf("\nBLAD: Deszyfrowanie nie powiodlo sie.\n");
    }

    printf("\nNacisnij Enter, aby zakonczyc...");
    // Podwójny getchar: pierwszy łapie \n pozostawiony przez scanf, drugi czeka na użytkownika
    getchar(); 
    getchar(); 
    return 0;
}
