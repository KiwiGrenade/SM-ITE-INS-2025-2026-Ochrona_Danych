# zad1_gf_tables.py
def gf_tables(p: int):
    """Zwraca: (add_table, mul_table, add_inv_dict, mul_inv_dict)"""
    add = [[(i + j) % p for j in range(p)] for i in range(p)]
    mul = [[(i * j) % p for j in range(p)] for i in range(p)]
    add_inv = {i: (-i) % p for i in range(p)}
    mul_inv = {i: (None if i == 0 else pow(i, p - 2, p)) for i in range(p)}
    return add, mul, add_inv, mul_inv


def demo_zad1():
    primes = [3, 5, 7]
    for p in primes:
        print(f"\n=== GF({p}) ===")
        add, mul, add_inv, mul_inv = gf_tables(p)
        print("Dodawanie:")
        for row in add:
            print(row)
        print("Mnożenie:")
        for row in mul:
            print(row)
        print("Elementy przeciwne:", add_inv)
        print("Elementy odwrotne:", mul_inv)


if __name__ == "__main__":
    demo_zad1()
