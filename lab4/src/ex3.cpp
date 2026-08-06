#include <time.h>
#include "deciph.hpp"

// =======================
//  KONWERSJE 8 BAJTÓW <-> HEX
// =======================

void bytes_to_hex(const unsigned char in[8], char hex[17]) {
    for (int i = 0; i < 8; i++)
        sprintf(&hex[2*i], "%02x", in[i]);
    hex[16] = 0;
}

void hex_to_bytes(const char hex[16], unsigned char out[8]) {
    for (int i = 0; i < 8; i++) {
        unsigned int b;
        sscanf(&hex[2*i], "%02x", &b);
        out[i] = (unsigned char)b;
    }
}

// =======================
//   DES ECB BLOCK
// =======================

void des_ecb_encrypt(
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

    bytes_to_hex(in, hex);
    HEXTOBIN(hex, in_bits, hexinbin);

    des(in_bits, key_bits, out_bits,
        desflg, ip, ipm, ksflg, ipc1, ipc2,
        glicd, cyflg, iet, ipp, is, ibin);

    // bits -> hex -> bytes
    for (int i = 0; i < 16; i++) {
        int val = (out_bits[i*4] << 3) |
                  (out_bits[i*4+1] << 2) |
                  (out_bits[i*4+2] << 1) |
                   out_bits[i*4+3];
        ohex[i] = (val < 10 ? '0' + val : 'a' + val - 10);
    }
    ohex[16] = 0;

    hex_to_bytes(ohex, out);
}

void des_ecb_decrypt(
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

    bytes_to_hex(in, hex);
    HEXTOBIN(hex, in_bits, hexinbin);

    // TU BYŁO des(...), ALE MUSI BYĆ d_des(...)
    d_des(in_bits, key_bits, out_bits,
          desflg, ip, ipm, ksflg, ipc1, ipc2,
          glicd, cyflg, iet, ipp, is, ibin);

    for (int i = 0; i < 16; i++) {
        int val = (out_bits[i*4] << 3) |
                  (out_bits[i*4+1] << 2) |
                  (out_bits[i*4+2] << 1) |
                   out_bits[i*4+3];
        ohex[i] = (val < 10 ? '0' + val : 'a' + val - 10);
    }
    ohex[16] = 0;

    hex_to_bytes(ohex, out);
}

// =======================
//    ENCRYPT FILE ECB
// =======================

void encrypt_file_ecb(
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
    if (!fin || !fout) { printf("File error\n"); exit(1); }

    unsigned char block[8], cblock[8];
    size_t n;
    int last_len = 0;

    srand(time(NULL));

    while ((n = fread(block, 1, 8, fin)) == 8) {
        des_ecb_encrypt(block, cblock, key_bits, hexinbin,
                        ip, ipm, ipc1, ipc2, glicd, iet, ipp, is, ibin);
        fwrite(cblock, 1, 8, fout);
    }

    // ostatni blok
    last_len = (int)n;
    if (n > 0) {
        for (size_t i = n; i < 8; i++)
            block[i] = rand() % 256;

        des_ecb_encrypt(block, cblock, key_bits, hexinbin,
                        ip, ipm, ipc1, ipc2, glicd, iet, ipp, is, ibin);
        fwrite(cblock, 1, 8, fout);
    }

    // blok META
    unsigned char meta[8];
    meta[0] = last_len;
    for (int i = 1; i < 8; i++)
        meta[i] = rand() % 256;

    des_ecb_encrypt(meta, cblock, key_bits, hexinbin,
                    ip, ipm, ipc1, ipc2, glicd, iet, ipp, is, ibin);
    fwrite(cblock, 1, 8, fout);

    fclose(fin);
    fclose(fout);

    printf("Encrypted.\n");
}

// =======================
//    DECRYPT FILE ECB
// =======================

void decrypt_file_ecb(
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

    if (!fin || !fout) { printf("File error\n"); exit(1); }

    fseek(fin, 0, SEEK_END);
    long size = ftell(fin);
    fseek(fin, 0, SEEK_SET);

    if (size < 16 || size % 8 != 0) {
        printf("Invalid encrypted file\n");
        exit(1);
    }

    long blocks = size / 8;

    unsigned char cblock[8], pblock[8], meta_block[8];

    // czytamy wszystko oprócz dwóch ostatnich
    for (long i = 0; i < blocks - 2; i++) {
        fread(cblock, 1, 8, fin);
        des_ecb_decrypt(cblock, pblock, key_bits, hexinbin,
                        ip, ipm, ipc1, ipc2, glicd, iet, ipp, is, ibin);
        fwrite(pblock, 1, 8, fout);
    }

    // przedostatni blok = ostatni blok danych
    unsigned char last_data_c[8];
    fread(last_data_c, 1, 8, fin);

    // ostatni blok = META
    fread(cblock, 1, 8, fin);
    des_ecb_decrypt(cblock, meta_block, key_bits, hexinbin,
                    ip, ipm, ipc1, ipc2, glicd, iet, ipp, is, ibin);

    int last_len = meta_block[0];

    des_ecb_decrypt(last_data_c, pblock, key_bits, hexinbin,
                    ip, ipm, ipc1, ipc2, glicd, iet, ipp, is, ibin);

    if (last_len == 0)
        fwrite(pblock, 1, 8, fout);
    else
        fwrite(pblock, 1, last_len, fout);

    fclose(fin);
    fclose(fout);

    printf("Decrypted.\n");
}

// =======================
//          MAIN
// =======================

int main() {
    char keyhex[17];
    gl64array key_bits;
    gl4_16array hexinbin;
    gl64array ip, ipm;
    gl56array ipc1, glicd;
    gl48array ipc2, iet;
    gl32array ipp;
    gl16_4_8array is;
    gl4_16array ibin;

    FILE *infile;
    int i, j;

    // to jest potrzebne: hex.dat -> hexinbin
    glopen(&infile, "hex.dat");
    for (i = 0; i < 16; i++)
        for (j = 0; j < 4; j++)
            fscanf(infile, "%d", &(hexinbin[j][i]));
    fclose(infile);

    // RESZTA (.dat) będzie czytana wewnątrz des/d_des/ks/cyfun

    int mode;
    printf("DES ECB FILE MODE\n");
    printf("1 - Encrypt file\n");
    printf("2 - Decrypt file\n> ");
    scanf("%d", &mode);

    char infile_name[256], outfile_name[256], key_ascii[9];

    printf("Input file: ");
    scanf("%s", infile_name);

    printf("Output file: ");
    scanf("%s", outfile_name);

    printf("ASCII key (8 chars): ");
    scanf("%8s", key_ascii);

    for (i = 0; i < 8; i++)
        sprintf(&keyhex[2*i], "%02x", (unsigned char)key_ascii[i]);
    keyhex[16] = 0;

    HEXTOBIN(keyhex, key_bits, hexinbin);

    if (mode == 1)
        encrypt_file_ecb(infile_name, outfile_name,
                         key_bits, hexinbin,
                         ip, ipm, ipc1, ipc2, glicd,
                         iet, ipp, is, ibin);
    else
        decrypt_file_ecb(infile_name, outfile_name,
                         key_bits, hexinbin,
                         ip, ipm, ipc1, ipc2, glicd,
                         iet, ipp, is, ibin);

    return 0;
}

