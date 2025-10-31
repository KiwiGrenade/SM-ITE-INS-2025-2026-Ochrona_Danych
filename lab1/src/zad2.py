# zad2_orders.py
def mult_order(a: int, p: int):
    if a % p == 0:
        return None
    val, order = a % p, 1
    while val != 1:
        val = (val * a) % p
        order += 1
        if order > p - 1:
            return None
    return order


def demo_zad2():
    primes = [3, 5, 7]
    for p in primes:
        print(f"\n=== GF({p}) ===")
        for a in range(1, p):
            print(f"rząd({a}) = {mult_order(a, p)}")


if __name__ == "__main__":
    demo_zad2()
