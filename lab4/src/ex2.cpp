#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "deciph.hpp"

// ==== Funkcje pomocnicze tylko dla zadania 2 ====

// bity DES -> string hex (16 znaków, bez spacji)
void bits_to_hex_string(gl64array bits, char hex[17]) {
    int i, j, inte, weight;
    for (i = 0; i <= 15; i++) {
        inte = 0;
        weight = 1;
        for (j = 0; j <= 3; j++) {
            inte = inte + bits[(i * 4 + 3 - j)] * weight;
            weight *= 2;
        }
        if (inte <= 9) hex[i] = char(inte + '0');
        else           hex[i] = char(inte - 10 + 'a');
    }
    hex[16] = '\0';
}

// 8 znaków ASCII -> 16 znaków hex
void ascii8_to_hex16(const char ascii[8], char hex[17]) {
    for (int i = 0; i < 8; ++i) {
        unsigned char b = (unsigned char)ascii[i];
        sprintf(&hex[2 * i], "%02x", b);
    }
    hex[16] = '\0';
}

// 16 hex -> 8 ASCII
void hex16_to_ascii8(const char hex[16], char ascii[9]) {
    for (int i = 0; i < 8; ++i) {
        unsigned int b;
        sscanf(&hex[2 * i], "%2x", &b);
        ascii[i] = (char)b;
    }
    ascii[8] = '\0';
}

// Wczytanie DOKŁADNIE 8 znaków ASCII (łącznie ze spacjami)
void read_ascii_8(char buf[9]) {
    int i = 0;
    int c;

    // pomijamy wcześniejsze \n/\r
    do {
        c = getchar();
        if (c == EOF) break;
    } while (c == '\n' || c == '\r');

    if (c == EOF) {
        for (i = 0; i < 8; ++i) buf[i] = ' ';
        buf[8] = '\0';
        return;
    }

    buf[i++] = (char)c;

    for (; i < 8; ++i) {
        c = getchar();
        if (c == '\n' || c == '\r' || c == EOF) {
            for (; i < 8; ++i) buf[i] = ' ';
            buf[8] = '\0';
            if (c != '\n' && c != '\r') {
                while ((c = getchar()) != '\n' && c != '\r' && c != EOF) {}
            }
            return;
        }
        buf[i] = (char)c;
    }
    buf[8] = '\0';

    // zjedz resztę linii jeśli user wpisał >8 znaków
    while ((c = getchar()) != '\n' && c != '\r' && c != EOF) {}
}

void print_bits_64(gl64array bits) {
    for (int i = 0; i < 64; ++i) printf("%d", bits[i]);
}

int main() {
    FILE* infile;
    int i, j;
    int desflg, ksflg, cyflg;
    gl32array     ipp;
    gl48array     ipc2, iet;
    gl56array     glicd, ipc1;
    gl64array     ip, ipm, inpt, key, jotput;
    gl16_4_8array is;
    gl4_16array   ibin;
    gl4_16array   hexinbin;

    char plaintext_ascii[9];
    char key_ascii[9];
    char text_hex[17], key_hex[17], cipher_hex[17];
    char cipher_hex_in[17];
    char plain_hex[17];
    char plain_ascii[9];
    int mode;

    // JEDYNE wczytywanie .dat w main – hex.dat
    glopen(&infile, "hex.dat");
    for (i = 0; i <= 15; i++)
        for (j = 0; j <= 3; j++)
            fscanf(infile, "%d", &(hexinbin[j][i]));
    fclose(infile);

    printf("*** PROGRAM N_CIPHER / N_DECIPHER ***\n\n");
    printf("Choose mode:\n");
    printf(" 1 - N_CIPHER (encrypt 8 ASCII chars)\n");
    printf(" 2 - N_DECIPHER (decrypt 16 hex chars)\n");
    printf("Type 1 or 2 + ENTER:\n");

    scanf("%d", &mode);
    getchar(); // ENTER po scanf
    if(mode == 1) {
        // ================== N_CIPHER ==================
        printf("\n*** PROGRAM N_CIPHER ***\n\n");

        printf("Type plaintext <8 ASCII characters + ENTER>:\n");
        printf("Text ascii:\n");
        printf("00000000\n");
        read_ascii_8(plaintext_ascii);
        printf("%s\n\n", plaintext_ascii);

        ascii8_to_hex16(plaintext_ascii, text_hex);
        HEXTOBIN(text_hex, inpt, hexinbin);

        printf("Text hex:   %s\n", text_hex);
        printf("Text bin:   ");
        print_bits_64(inpt);
        printf("\n\n");

        printf("Type Key <8 ASCII characters + ENTER>:\n");
        printf("Key ascii:\n");
        printf("00000000\n");
        read_ascii_8(key_ascii);
        printf("%s\n\n", key_ascii);

        ascii8_to_hex16(key_ascii, key_hex);
        HEXTOBIN(key_hex, key, hexinbin);

        printf("Key hex:   %s\n", key_hex);
        printf("Key bin:   ");
        print_bits_64(key);
        printf("\n\n");

        printf("Ciphering is running...");
        ksflg = 1; cyflg = 1; desflg = 1;
        des(inpt, key, jotput, desflg, ip, ipm, ksflg,
            ipc1, ipc2, glicd, cyflg, iet, ipp, is, ibin);
        printf("Done.\n\n");

        bits_to_hex_string(jotput, cipher_hex);

        printf("Cipher hex: ");
        for (i = 0; i < 16; ++i) {
            printf("%c", cipher_hex[i]);
            // if (i != 15) printf("   ");
        }
        printf("\n");

        printf("Cipher bin: ");
        print_bits_64(jotput);
        printf("\n\n");
    }
    else if (mode == 2) {
        // ================== N_DECIPHER ==================
        printf("\n*** PROGRAM N_DECIPHER ***\n\n");

        printf("Type ciphertext <16 hexadecimal characters [0..f] + ENTER>:\n");
        printf("Text hex:\n");
        scanf("%16s", cipher_hex_in);
        getchar();

        HEXTOBIN(cipher_hex_in, inpt, hexinbin);
        printf("%s\n", cipher_hex_in);
        printf("Text bin:   ");
        print_bits_64(inpt);
        printf("\n\n");

        printf("Type Key <8 ASCII characters + ENTER>:\n");
        printf("Key ascii:\n");
        printf("00000000\n");
        read_ascii_8(key_ascii);
        printf("%s\n\n", key_ascii);

        ascii8_to_hex16(key_ascii, key_hex);
        HEXTOBIN(key_hex, key, hexinbin);

        printf("Key hex:    %s\n", key_hex);
        printf("Key bin:    ");
        print_bits_64(key);
        printf("\n\n");

        printf("DeCiphering is running...");
        ksflg = 1; cyflg = 1; desflg = 1;
        d_des(inpt, key, jotput, desflg, ip, ipm, ksflg,
              ipc1, ipc2, glicd, cyflg, iet, ipp, is, ibin);
        printf("Done.\n\n");

        bits_to_hex_string(jotput, plain_hex);
        hex16_to_ascii8(plain_hex, plain_ascii);

        printf("Decipher ASCII:");
        for (i = 0; i < 8; ++i) {
            printf(" %c", plain_ascii[i]);
        }
        printf("\n");

        printf("DeCipher hex:");
        for (i = 0; i < 16; ++i) {
            printf(" %c", plain_hex[i]);
            // if (i != 15) printf("");
        }
        printf("\n");

        printf("DeCipher bin: ");
        print_bits_64(jotput);
        printf("\n\n");
    }
    else {
        printf("Wrong mode.\n");
    }

    printf("Press ENTER\n");
    getchar();
    return 0;
}

