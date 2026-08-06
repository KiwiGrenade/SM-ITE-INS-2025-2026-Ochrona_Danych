#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "deciph.hpp"

// =======================
//  KONWERSJE 8 BAJTÓW <-> HEX
// =======================

void bytes_to_hex(const unsigned char in[8], char hex[17]) {
    for (int i = 0; i < 8; i++)
        sprintf(&hex[2 * i], "%02x", in[i]);
    hex[16] = 0;
}

void hex_to_bytes(const char hex[16], unsigned char out[8]) {
    for (int i = 0; i < 8; i++) {
        unsigned int b;
        sscanf(&hex[2 * i], "%02x", &b);
        out[i] = (unsigned char)b;
    }
}

// =======================
//   DES BLOCK (ENCRYPT / DECRYPT)
//   — używa funkcji z CIPH.cpp / DECIPH.cpp
// =======================

void des_block_encrypt(
    const unsigned char in[8],
    unsigned char out[8],
    gl64array key_bits,
    gl4_16array hexinbin,
    gl64array ip, gl64array ipm,
    gl56array ipc1, gl48array ipc2, gl56array glicd,
    gl48array iet, gl32array ipp, gl16_4_8array is, gl4_16array ibin
) {
    char hex[17], ohex[17];
    gl64array in_bits, out_bits;
    int ksflg = 1, cyflg = 1, desflg = 1;

    // 8 bajtów -> 16 hex -> bity
    bytes_to_hex(in, hex);
    HEXTOBIN(hex, in_bits, hexinbin);

    // klasyczny DES (szyfrowanie) z CIPH.cpp
    des(in_bits, key_bits, out_bits,
        desflg, ip, ipm, ksflg, ipc1, ipc2,
        glicd, cyflg, iet, ipp, is, ibin);

    // bity -> hex -> 8 bajtów
    for (int i = 0; i < 16; i++) {
        int val = (out_bits[i * 4] << 3) |
                  (out_bits[i * 4 + 1] << 2) |
                  (out_bits[i * 4 + 2] << 1) |
                   out_bits[i * 4 + 3];
        ohex[i] = (val < 10 ? '0' + val : 'a' + val - 10);
    }
    ohex[16] = 0;

    hex_to_bytes(ohex, out);
}

void des_block_decrypt(
    const unsigned char in[8],
    unsigned char out[8],
    gl64array key_bits,
    gl4_16array hexinbin,
    gl64array ip, gl64array ipm,
    gl56array ipc1, gl48array ipc2, gl56array glicd,
    gl48array iet, gl32array ipp, gl16_4_8array is, gl4_16array ibin
) {
    char hex[17], ohex[17];
    gl64array in_bits, out_bits;
    int ksflg = 1, cyflg = 1, desflg = 1;

    // 8 bajtów -> 16 hex -> bity
    bytes_to_hex(in, hex);
    HEXTOBIN(hex, in_bits, hexinbin);

    // odwrotny DES z DECIPH.cpp (d_des)
    d_des(in_bits, key_bits, out_bits,
          desflg, ip, ipm, ksflg, ipc1, ipc2,
          glicd, cyflg, iet, ipp, is, ibin);

    // bity -> hex -> 8 bajtów
    for (int i = 0; i < 16; i++) {
        int val = (out_bits[i * 4] << 3) |
                  (out_bits[i * 4 + 1] << 2) |
                  (out_bits[i * 4 + 2] << 1) |
                   out_bits[i * 4 + 3];
        ohex[i] = (val < 10 ? '0' + val : 'a' + val - 10);
    }
    ohex[16] = 0;

    hex_to_bytes(ohex, out);
}

// =======================
//    ENCRYPT FILE (CBC)
// =======================

void encrypt_file_cbc(
    const char *inname,
    const char *outname,
    gl64array key_bits,
    gl4_16array hexinbin,
    gl64array ip, gl64array ipm,
    gl56array ipc1, gl48array ipc2, gl56array glicd,
    gl48array iet, gl32array ipp, gl16_4_8array is, gl4_16array ibin
) {
    FILE *fin = fopen(inname, "rb");
    FILE *fout = fopen(outname, "wb");
    if (!fin || !fout) {
        printf("File error\n");
        exit(1);
    }

    unsigned char block[8], cblock[8], iv[8], prev[8];
    size_t n;
    int last_len = 0;

    srand((unsigned int)time(NULL));

    // losowy IV, zapisany jawnie na początku pliku
    for (int i = 0; i < 8; i++)
        iv[i] = (unsigned char)(rand() % 256);
    fwrite(iv, 1, 8, fout);

    // początkowe "prev" do CBC
    for (int i = 0; i < 8; i++)
        prev[i] = iv[i];

    // czytamy pełne bloki po 8 bajtów
    while ((n = fread(block, 1, 8, fin)) == 8) {
        // CBC: P' = P xor prev
        for (int i = 0; i < 8; i++)
            block[i] ^= prev[i];

        des_block_encrypt(block, cblock, key_bits, hexinbin,
                          ip, ipm, ipc1, ipc2, glicd, iet, ipp, is, ibin);

        fwrite(cblock, 1, 8, fout);

        // nowy prev = aktualny szyfrogram
        for (int i = 0; i < 8; i++)
            prev[i] = cblock[i];
    }

    // ostatni blok (niepełny)
    last_len = (int)n;
    if (n > 0) {
        // dopchaj losowymi bajtami do 8
        for (size_t i = n; i < 8; i++)
            block[i] = (unsigned char)(rand() % 256);

        // CBC: P' = P xor prev
        for (int i = 0; i < 8; i++)
            block[i] ^= prev[i];

        des_block_encrypt(block, cblock, key_bits, hexinbin,
                          ip, ipm, ipc1, ipc2, glicd, iet, ipp, is, ibin);
        fwrite(cblock, 1, 8, fout);

        // prev = ostatni blok szyfrogramu danych
        for (int i = 0; i < 8; i++)
            prev[i] = cblock[i];
    }

    // blok META: [0] = długość ostatniego (niepełnego) bloku (0..7),
    // reszta bajtów losowa
    unsigned char meta[8], meta_xor[8];
    meta[0] = (unsigned char)last_len;
    for (int i = 1; i < 8; i++)
        meta[i] = (unsigned char)(rand() % 256);

    // CBC dla meta: M' = meta xor prev (prev = C_last_data)
    for (int i = 0; i < 8; i++)
        meta_xor[i] = meta[i] ^ prev[i];

    des_block_encrypt(meta_xor, cblock, key_bits, hexinbin,
                      ip, ipm, ipc1, ipc2, glicd, iet, ipp, is, ibin);
    fwrite(cblock, 1, 8, fout);

    fclose(fin);
    fclose(fout);

    printf("Encrypted (CBC).\n");
}

// =======================
//    DECRYPT FILE (CBC)
// =======================

void decrypt_file_cbc(
    const char *inname,
    const char *outname,
    gl64array key_bits,
    gl4_16array hexinbin,
    gl64array ip, gl64array ipm,
    gl56array ipc1, gl48array ipc2, gl56array glicd,
    gl48array iet, gl32array ipp, gl16_4_8array is, gl4_16array ibin
) {
    FILE *fin = fopen(inname, "rb");
    FILE *fout = fopen(outname, "wb");
    if (!fin || !fout) {
        printf("File error\n");
        exit(1);
    }

    // rozmiar pliku
    fseek(fin, 0, SEEK_END);
    long size = ftell(fin);
    fseek(fin, 0, SEEK_SET);

    // musimy mieć:
    // - 8 bajtów IV
    // - co najmniej 2 bloki szyfrogramu (dane + meta) = 16 bajtów
    if (size < 8 + 16 || ((size - 8) % 8) != 0) {
        printf("Invalid CBC encrypted file\n");
        fclose(fin);
        fclose(fout);
        return;
    }

    long cipher_bytes = size - 8;
    long blocks = cipher_bytes / 8;        // liczba bloków C (bez IV)
    long data_blocks = blocks - 1;         // ostatni blok to META

    unsigned char iv[8];
    unsigned char cblock[8], pblock[8];
    unsigned char prev[8];

    // wczytaj IV
    fread(iv, 1, 8, fin);
    for (int i = 0; i < 8; i++)
        prev[i] = iv[i];

    // jeśli data_blocks < 1 -> plik ma tylko meta (bez danych) – teoretycznie
    // nie ma sensu, ale niech przejdzie

    // 1) odszyfruj wszystkie bloki danych oprócz ostatniego
    for (long b = 0; b < data_blocks - 1; b++) {
        fread(cblock, 1, 8, fin);

        unsigned char x[8];
        des_block_decrypt(cblock, x, key_bits, hexinbin,
                          ip, ipm, ipc1, ipc2, glicd, iet, ipp, is, ibin);

        // P = x xor prev
        for (int i = 0; i < 8; i++)
            pblock[i] = x[i] ^ prev[i];

        fwrite(pblock, 1, 8, fout);

        // prev = C (do następnego bloku)
        for (int i = 0; i < 8; i++)
            prev[i] = cblock[i];
    }

    // 2) ostatni blok danych (C_last_data)
    unsigned char last_data_c[8];
    fread(last_data_c, 1, 8, fin);

    // 3) blok META (C_meta)
    unsigned char meta_c[8];
    fread(meta_c, 1, 8, fin);

    // --- odczyt META ---
    unsigned char meta_x[8], meta_plain[8];

    // x_meta = D_K(C_meta)
    des_block_decrypt(meta_c, meta_x, key_bits, hexinbin,
                      ip, ipm, ipc1, ipc2, glicd, iet, ipp, is, ibin);

    // meta_plain = x_meta xor last_data_c  (bo w szyfrowaniu meta xor C_last_data)
    for (int i = 0; i < 8; i++)
        meta_plain[i] = meta_x[i] ^ last_data_c[i];

    int last_len = meta_plain[0];  // 0..7

    // --- odszyfruj ostatni blok danych ---
    unsigned char last_x[8], last_plain[8];

    // prev z punktu widzenia ostatniego bloku danych to:
    //  - jeśli data_blocks == 1 -> IV
    //  - jeśli >1             -> ostatni C poprzedniego kroku (już w prev)
    // prev aktualnie = C_{N-1} (bo po pętli powyżej)
    des_block_decrypt(last_data_c, last_x, key_bits, hexinbin,
                      ip, ipm, ipc1, ipc2, glicd, iet, ipp, is, ibin);

    for (int i = 0; i < 8; i++)
        last_plain[i] = last_x[i] ^ prev[i];

    if (last_len == 0) {
        // ostatni blok był pełny (8 bajtów prawdziwych danych)
        fwrite(last_plain, 1, 8, fout);
    } else {
        fwrite(last_plain, 1, last_len, fout);
    }

    fclose(fin);
    fclose(fout);

    printf("Decrypted (CBC).\n");
}

// =======================
//          MAIN
// =======================

int main() {
    FILE *infile;
    int i, j;

    gl64array key_bits;
    gl4_16array hexinbin;
    gl64array ip, ipm;
    gl56array ipc1, glicd;
    gl48array ipc2, iet;
    gl32array ipp;
    gl16_4_8array is;
    gl4_16array ibin;

    char key_ascii[9];
    char keyhex[17];
    char infile_name[256], outfile_name[256];
    int mode;

    printf("DES CBC FILE MODE\n");
    printf("1 - Encrypt file (CBC)\n");
    printf("2 - Decrypt file (CBC)\n> ");
    scanf("%d", &mode);

    printf("Input file: ");
    scanf("%s", infile_name);

    printf("Output file: ");
    scanf("%s", outfile_name);

    printf("ASCII key (8 chars): ");
    scanf("%8s", key_ascii);

    // ---- wczytanie tablic HEX -> BIN z hex.dat (jak w CIPH.cpp) ----
    glopen(&infile, "hex.dat");
    for (i = 0; i <= 15; i++)
        for (j = 0; j <= 3; j++)
            fscanf(infile, "%d", &(hexinbin[j][i]));
    fclose(infile);

    // UWAGA:
    // Dla des()/d_des() NIE MUSISZ tu wczytywać desinp.dat, ksinpu.dat, cyfuni.dat,
    // bo ich funkcje ks(), cyfun(), des(), d_des() i tak czytają te pliki
    // za każdym wywołaniem (flagi desflg/ksflg/cyflg przekazywane są przez wartość).
    // Jednak tablice ip, ipm, ipc1, ipc2, glicd, iet, ipp, is, ibin
    // muszą istnieć jako argumenty — będą wypełniane przez te funkcje.

    // ASCII -> HEX
    for (i = 0; i < 8; i++)
        sprintf(&keyhex[2 * i], "%02x", (unsigned char)key_ascii[i]);
    keyhex[16] = 0;

    // HEX -> bity
    HEXTOBIN(keyhex, key_bits, hexinbin);

    if (mode == 1) {
        encrypt_file_cbc(infile_name, outfile_name,
                         key_bits, hexinbin,
                         ip, ipm, ipc1, ipc2, glicd,
                         iet, ipp, is, ibin);
    } else if (mode == 2) {
        decrypt_file_cbc(infile_name, outfile_name,
                         key_bits, hexinbin,
                         ip, ipm, ipc1, ipc2, glicd,
                         iet, ipp, is, ibin);
    } else {
        printf("Wrong mode.\n");
    }

    return 0;
}

