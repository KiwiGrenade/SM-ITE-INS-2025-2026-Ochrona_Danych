# zad3_primitives.py
from zad2 import mult_order

def primitive_elements(p: int):
    return [a for a in range(1, p) if mult_order(a, p) == p - 1]


def demo_zad3():
    primes = [3, 5, 7]
    for p in primes:
        prim = primitive_elements(p)
        print(f"GF({p}) → elementy pierwotne: {prim}")


if __name__ == "__main__":
    demo_zad3()
