# zad5_stream_cipher.py
import sys
import argparse
from typing import List

def lfsr_bits(a_coeffs: List[int], init_state: List[int], n_bits: int):
    """Generuje bity z LFSR nad GF(2)."""
    m = len(a_coeffs)
    state = list(init_state)
    out = []
    for _ in range(n_bits):
        out.append(state[0])
        nxt = sum((a_coeffs[j] & state[j]) for j in range(m)) & 1
        state = state[1:] + [nxt]
    return out


def bits_to_bytes(bits):
    out = bytearray()
    for i in range(0, len(bits), 8):
        b = 0
        for j in range(8):
            b = (b << 1) | (bits[i+j] if i+j < len(bits) else 0)
        out.append(b)
    return bytes(out)


def xor_bytes(data: bytes, keystream_bits: List[int]):
    key = bits_to_bytes(keystream_bits)
    key = key[:len(data)]
    return bytes(d ^ k for d, k in zip(data, key))


def lfsr_keystream(length_bytes: int):
    a = [0]*10
    a[0], a[3] = 1, 1  # x^10 + x^3 + 1
    init = [1]+[0]*9
    return lfsr_bits(a, init, length_bytes*8)


def encrypt_file(input_path, output_path):
    with open(input_path, "rb") as f:
        data = f.read()
    key = lfsr_keystream(len(data))
    enc = xor_bytes(data, key)
    with open(output_path, "wb") as f:
        f.write(enc)
    print(f"Zaszyfrowano: {input_path} → {output_path}")


def decrypt_file(input_path, output_path):
    with open(input_path, "rb") as f:
        data = f.read()
    key = lfsr_keystream(len(data))
    dec = xor_bytes(data, key)
    with open(output_path, "wb") as f:
        f.write(dec)
    print(f"Odszyfrowano: {input_path} → {output_path}")


def demo_zad5():
    plain = b"Hello, world! This is LFSR stream cipher test."
    key = lfsr_keystream(len(plain))
    cipher = xor_bytes(plain, key)
    dec = xor_bytes(cipher, lfsr_keystream(len(cipher)))
    print("Tekst:", plain)
    print("Szyfrogram (hex):", cipher.hex())
    print("Odszyfrowany:", dec)
    print("Zgodność:", dec == plain)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--encrypt", nargs=2, metavar=("IN", "OUT"))
    parser.add_argument("--decrypt", nargs=2, metavar=("IN", "OUT"))
    parser.add_argument("--demo", action="store_true")
    args = parser.parse_args()

    if args.demo:
        demo_zad5()
    elif args.encrypt:
        encrypt_file(args.encrypt[0], args.encrypt[1])
    elif args.decrypt:
        decrypt_file(args.decrypt[0], args.decrypt[1])
    else:
        parser.print_help()
