#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

// Na Linuxie nie potrzebujemy conio.h ani windowsowych flag
// Używamy standardowych nagłówków

long p, q, n, Klucz_d, Klucz_e;
long ns = 9; 

int odd(int p) { return p % 2; }

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

long liczba_pierwsza(long p) {
    long q, x;
    q = p - 1; x = 1;
    do {
        while ((potega_mod(x, q, p) != 1)) {
            q = p; p = p + 1; x = 1;
        }
        x = x + 1;
    } while (!(x >= ns));
    return p;
}

long multi_inv(long x, long p) {
    long inv, m, n_local, qt, r, rt, s, st, t;
    qt = 1; rt = 0; st = p; m = 0; r = 1; s = x;
    while (s != 0) {
        n_local = st / s; t = m; m = qt - n_local * m; qt = t;
        t = r; r = rt - n_local * r; rt = t;
        t = s; s = st % s; st = t;
    }
    inv = rt;
    if (inv < 0) inv = inv + p;
    return inv;
}

long gcd_euclid(long x, long y) {
    long reszta;
    reszta = x % y;
    while (reszta != 0) {
        x = y; y = reszta; reszta = x % y;
    }
    return y;
}

// --- FUNKCJE PLIKOWE ---

void szyfruj_plik(const char* plik_we, const char* plik_wy) {
    FILE *fin, *fout;
    unsigned char ch;
    long m, c;

    fin = fopen(plik_we, "rb");
    if (!fin) { printf("Blad otwarcia pliku wejsciowego: %s\n", plik_we); return; }
    fout = fopen(plik_wy, "wb");
    if (!fout) { printf("Blad tworzenia pliku wyjsciowego: %s\n", plik_wy); fclose(fin); return; }

    printf("Szyfrowanie '%s' -> '%s'...\n", plik_we, plik_wy);
    
    clock_t start = clock();

    while (fread(&ch, sizeof(unsigned char), 1, fin) == 1) {
        m = (long)ch;
        c = potega_mod(m, Klucz_e, n);
        fwrite(&c, sizeof(long), 1, fout);
    }

    clock_t end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("Gotowe. Czas: %.6f s.\n", time_taken);

    fclose(fin);
    fclose(fout);
}

void deszyfruj_plik(const char* plik_we, const char* plik_wy) {
    FILE *fin, *fout;
    unsigned char ch_out;
    long c, m;

    fin = fopen(plik_we, "rb");
    if (!fin) { printf("Blad otwarcia pliku zaszyfrowanego: %s\n", plik_we); return; }
    fout = fopen(plik_wy, "wb");
    if (!fout) { printf("Blad tworzenia pliku odszyfrowanego: %s\n", plik_wy); fclose(fin); return; }

    printf("Deszyfrowanie '%s' -> '%s'...\n", plik_we, plik_wy);

    while (fread(&c, sizeof(long), 1, fin) == 1) {
        m = potega_mod(c, Klucz_d, n);
        ch_out = (unsigned char)m;
        fwrite(&ch_out, sizeof(unsigned char), 1, fout);
    }

    printf("Gotowe.\n");

    fclose(fin);
    fclose(fout);
}

int main() {
    long f;
    char nazwa_pliku[100];
    
    // Inicjalizacja generatora losowego
    srand((unsigned)time(0));

    printf("--- RSA na Linuxie ---\n");
    
    // Generowanie kluczy
    p = rand() % 200 + 16; if (!odd(p)) p++; p = liczba_pierwsza(p);
    q = rand() % 200 + 16; if (!odd(q)) q++; q = liczba_pierwsza(q);
    n = p * q;
    f = (p - 1) * (q - 1);
    
    do {
        Klucz_e = rand() % 32500;
        Klucz_e += rand() % 32500 + 1L;
        if (!odd(Klucz_e)) Klucz_e++;
    } while (!(gcd_euclid(Klucz_e, f) == 1));
    
    Klucz_d = multi_inv(Klucz_e, f);

    printf("Klucze: e=%ld, d=%ld, n=%ld\n\n", Klucz_e, Klucz_d, n);

    printf("Podaj nazwe pliku do zaszyfrowania: ");
    if (scanf("%99s", nazwa_pliku) != 1) {
        printf("Blad wczytywania nazwy.\n");
        return 1;
    }

    // Wywołanie funkcji
    szyfruj_plik(nazwa_pliku, "zaszyfr");
    deszyfruj_plik("zaszyfr", "odszyfr");

    printf("\nSprawdz pliki 'zaszyfr' i 'odszyfr'.\n");
    
    // Zastępstwo dla system("pause") na Linuxie:
    printf("Nacisnij Enter, aby zakonczyc...");
    getchar(); // Zbiera znak nowej linii po scanf
    getchar(); // Czeka na enter użytkownika
    
    return 0;
}
