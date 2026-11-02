#!/usr/bin/env python3
"""Независимый тестовый набор: фамилии из smashew/NameDatabases (surnames/ru.txt, лицензия Unlicense).

В источнике фамилии записаны латиницей, поэтому они переводятся обратно в кириллицу простым
правилом (транслитерация обратима неточно: «Ye/E/Э», «Kh/Х» и т.п. — оценки первой буквы приблизительные).
Имена в источнике отсутствуют и подставляются случайно из короткого списка.

Использование: gen_test_indep.py ru.txt names100k.csv weights.h
Выход: test_indep.csv (2000 пар, которых нет в names100k.csv), печатает сравнение распределения первых букв.
"""
import random, re, sys, math, collections

SRC, BASE, WH = sys.argv[1:4]
random.seed(7)

MULTI = [
    ("shch", "щ"), ("sch", "ш"), ("zh", "ж"), ("kh", "х"), ("ts", "ц"), ("tz", "ц"), ("ch", "ч"), ("sh", "ш"),
    ("yu", "ю"), ("ju", "ю"), ("ya", "я"), ("ja", "я"), ("yo", "ё"), ("jo", "ё"), ("ye", "е"), ("je", "е"),
    ("iya", "ия"), ("yy", "ый"), ("iy", "ий"), ("ij", "ий"), ("ii", "ий"), ("ey", "ей"), ("oy", "ой"),
    ("ay", "ай"), ("uy", "уй"), ("ff", "в"), ("ee", "и"), ("ie", "ие"),
]
SINGLE = {
    "a": "а", "b": "б", "c": "к", "d": "д", "e": "е", "f": "ф", "g": "г", "h": "х", "i": "и", "j": "й",
    "k": "к", "l": "л", "m": "м", "n": "н", "o": "о", "p": "п", "q": "к", "r": "р", "s": "с", "t": "т",
    "u": "у", "v": "в", "w": "в", "x": "кс", "y": "ы", "z": "з",
}
MULTI.sort(key=lambda t: -len(t[0]))


def back(s):
    s = s.lower()
    out, i = [], 0
    while i < len(s):
        for lat, cyr in MULTI:
            if s.startswith(lat, i):
                out.append(cyr)
                i += len(lat)
                break
        else:
            ch = s[i]
            if ch == "y" and i == len(s) - 1:
                out.append("й")           # ...ky -> ...кй уже покрыто 'iy'; одиночный конечный y -> й
            else:
                out.append(SINGLE[ch])
            i += 1
    r = "".join(out)
    return r[0].upper() + r[1:]


NAMES = ("Александр Сергей Андрей Алексей Дмитрий Иван Михаил Николай Владимир Игорь Павел Олег Юрий Виктор "
         "Максим Артём Денис Роман Евгений Константин Елена Ольга Наталья Татьяна Ирина Светлана Анна Мария "
         "Екатерина Юлия Марина Галина Людмила Оксана Надежда Вера").split()

lat = [l.strip() for l in open(SRC, encoding="utf-8", errors="strict") if re.fullmatch(r"[A-Za-z]{3,40}", l.strip())]
cyr = sorted({back(s) for s in lat if len(back(s).encode()) <= 60})
print(f"латинских фамилий: {len(lat)}, уникальных после обратной транслитерации: {len(cyr)}")

base = set()
for l in open(BASE, encoding="utf-8"):
    a, b = l.rstrip("\n").split(";")
    base.add((a, b))

pairs = []
for fam in random.sample(cyr, len(cyr)):
    nm = random.choice(NAMES)
    if (fam, nm) not in base:
        pairs.append((fam, nm))
    if len(pairs) == 2000:
        break
with open("test_indep.csv", "w", encoding="utf-8") as o:
    for a, b in pairs:
        o.write(f"{a};{b}\n")

# --- сравнение распределения первых букв с таблицей весов ---
alpha = ["Ё"] + [chr(c) for c in range(ord("А"), ord("Я") + 1)]
w = [float(x) for x in re.findall(r"^\s+([0-9.]+),", open(WH, encoding="utf-8").read(), re.M)]
wt = dict(zip(alpha, w))
cnt = collections.Counter(f[0] for f in cyr)
tot = sum(cnt.values())
mx = max(cnt.values())
xs = [wt[c] for c in alpha]
ys = [cnt[c] / mx for c in alpha]
mx_, my_ = sum(xs) / len(xs), sum(ys) / len(ys)
corr = sum((x - mx_) * (y - my_) for x, y in zip(xs, ys)) / math.sqrt(
    sum((x - mx_) ** 2 for x in xs) * sum((y - my_) ** 2 for y in ys))
print(f"корреляция весов (datacoon) и распределения первых букв в независимом источнике: {corr:.3f}")
print("буква  вес(datacoon)  доля в независимом источнике (К=1)")
for c in sorted(alpha, key=lambda c: -cnt[c])[:12]:
    print(f"  {c}     {wt[c]:.3f}          {cnt[c] / mx:.3f}   ({cnt[c] / tot * 100:.1f}%)")
