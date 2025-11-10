def generate_field_tables(m, prim_poly):
    field_size = 1 << m
    mask = field_size - 1

    def mul_raw(a, b):
        res = 0
        while b:
            if b & 1:
                res ^= a
            b >>= 1
            a <<= 1
            if a & field_size:
                a ^= prim_poly
        return res & mask

    exp = [0] * (2 * field_size)
    log = [-1] * field_size
    exp[0] = 1
    log[1] = 0

    for i in range(1, field_size - 1):
        exp[i] = mul_raw(exp[i - 1], 2)
        log[exp[i]] = i

    for i in range(field_size - 1, 2 * field_size - 2):
        exp[i] = exp[i - (field_size - 1)]

    return exp, log, mul_raw


def gf_add(a, b):
    return a ^ b


def gf_mul(a, b, exp, log, m):
    if a == 0 or b == 0:
        return 0
    size = (1 << m) - 1
    return exp[(log[a] + log[b]) % size]


def gf_name(v, log):
    return '0' if v == 0 else f'a{log[v]}'


def print_table(m, exp, log, op='mul'):
    field_size = 1 << m
    print(f"\nTablica {('mnożenia' if op=='mul' else 'dodawania')} dla GF(2^{m})")
    header = ['   '] + [gf_name(i, log) for i in range(field_size)]
    print(' '.join(f"{x:>4}" for x in header))
    for i in range(field_size):
        row = [gf_name(i, log)]
        for j in range(field_size):
            if op == 'mul':
                v = gf_mul(i, j, exp, log, m)
            else:
                v = gf_add(i, j)
            row.append(gf_name(v, log))
        print(' '.join(f"{x:>4}" for x in row))

def gf_poly_degree4(x1, x2, x3, x4, add, mul):
    s1 = add(add(x1, x2), add(x3, x4))                           # suma x1+x2+x3+x4
    s2 = add(add(mul(x1, x2), mul(x3, x4)), mul(add(x1, x2), add(x3, x4)))  # kombinacja z wzoru
    s3 = add(add(mul(add(x1, x2), mul(x3, x4)), mul(x1, x3)), mul(x2, x4))
    s4 = mul(mul(x1, x2), mul(x3, x4))

    return [1, s1, s2, s3, s4]  # współczynniki przy x^4, x^3, x^2, x^1, x^0

def main():
    # pola i ich wielomiany pierwotne
    fields = {
        2: 0b111,    # x^2 + x + 1
        3: 0b1011,   # x^3 + x + 1
        4: 0b10011   # x^4 + x + 1
    }

    for m, prim_poly in fields.items():
        exp, log, mul_raw = generate_field_tables(m, prim_poly)
        print_table(m, exp, log, op='mul')
        print_table(m, exp, log, op='add')

    print("\nZadanie 5: Wielomian czwartego stopnia w GF(2^4)")
    m = 4
    prim_poly = fields[m]
    exp, log, mul_raw = generate_field_tables(m, prim_poly)

    def mul(a, b): return gf_mul(a, b, exp, log, m)
    def add(a, b): return gf_add(a, b)

    x1 = exp[1]  # α^1
    x2 = exp[2]  # α^2
    x3 = exp[4]  # α^4
    x4 = exp[8]  # α^8

    coeffs = gf_poly_degree4(x1, x2, x3, x4, add, mul)
    print("Współczynniki m_i(x):", [gf_name(c, log) for c in coeffs])


if __name__ == "__main__":
    main()

