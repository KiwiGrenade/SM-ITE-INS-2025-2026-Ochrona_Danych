# zad4_sequences.py
def sequence_from_poly(p, a_coeffs, init_state, max_iter=200000):
    m = len(a_coeffs)
    if len(init_state) != m:
        raise ValueError("init_state musi mieć długość m")
    state = list(init_state)
    seen = {tuple(state): 0}
    idx = 0
    while True:
        nxt = (-sum((a_coeffs[j] * state[j]) % p for j in range(m))) % p
        state = state[1:] + [nxt]
        idx += 1
        t = tuple(state)
        if t in seen:
            return idx - seen[t]
        seen[t] = idx
        if idx > max_iter:
            return None


def demo_zad4():
    tests = [
        (2, [1, 1, 0, 0], [1, 0, 0, 0], "x^4 + x + 1 nad GF(2)"),
        (3, [2, 1], [1, 0], "x^2 + x + 2 nad GF(3)"),
        (3 , [1, 2], [1, 0], "x^2 + 2x + 1 nad GF(3)"),
    ]
    for p, a, init, name in tests:
        period = sequence_from_poly(p, a, init)
        maxp = p ** len(a) - 1
        print(f"{name}: okres = {period}, maksymalny = {maxp}, pierwotny = {period == maxp}")


if __name__ == "__main__":
    demo_zad4()
