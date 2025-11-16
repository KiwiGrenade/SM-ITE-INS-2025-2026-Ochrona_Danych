ZECH = {
    4: [None, 2, 1],
    8: [None, 3, 6, 1, 5, 4, 2],
    16: [None, 4, 8, 14, 1, 10, 13, 9, 2, 7, 5, 12, 11, 6, 3],
}

def lab(q):
    return [("0" if v == 0 else ("1" if v == 1 else f"a{v-1}")) for v in range(q)]

def S(x, y, q):
    if x == 0 or y == 0:
        return x + y
    if x == y:
        return 0
    if x < y:
        x, y = y, x
    z = ZECH[q][x - y]
    return ((y + z - 1) % (q - 1)) + 1

def P(x, y, q):
    if x == 0 or y == 0:
        return 0
    return ((x + y - 2) % (q - 1)) + 1

def print_table(op, q):
    name = "Mnożenie" if op == "mul" else "Dodawanie"
    L = lab(q)
    print(f"\n{name} w GF(2^{ {4:2,8:3,16:4}[q] })")
    header = [" "] + L
    w = max(len(s) for s in L) + 2
    print("".join(s.center(w) for s in header))
    for x in range(q):
        row = []
        for y in range(q):
            v = P(x, y, q) if op == "mul" else S(x, y, q)
            row.append(L[v])
        print("".join([L[x].center(w)] + [c.center(w) for c in row]))

def main():
    for a in ("mul", "add"):
        for q in (4, 8, 16):
            print_table(a, q)

    print("\nCzęść interaktywna")
    try:
        q = int(input("Wybierz q z {4,8,16}: "))
        if q not in (4, 8, 16):
            raise ValueError
        x = int(input(f"x w 0..{q-1}: "))
        y = int(input(f"y w 0..{q-1}: "))
        if not (0 <= x < q and 0 <= y < q):
            raise ValueError
        L = lab(q)
        sx = S(x, y, q)
        px = P(x, y, q)
        print(f"x + y = {sx} ({L[sx]})")
        print(f"x * y = {px} ({L[px]})")
    except Exception:
        print("Błędne dane.")

if __name__ == "__main__":
    main()

