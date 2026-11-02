#!/usr/bin/env python3
"""Готовит данные для бенчмарка из data-distinct.csv (datacoon/russiannames).

Выход:
  names100k.csv     - 100 000 уникальных пар "Фамилия;Имя", отсортированных как strcmp по UTF-8
  test_real.csv     - 2000 пар НЕ из списка, с реальным распределением первых букв
  test_uniform.csv  - 2000 пар НЕ из списка, первая буква выбирается равновероятно
  weights.h         - таблица весов первых букв (К = 1.0), порядок: Ё, А..Я (порядок байтов UTF-8)
"""
import csv, collections, random, sys

SRC = sys.argv[1]
random.seed(42)

ALPHA = ["Ё"] + [chr(c) for c in range(ord("А"), ord("Я") + 1)]  # порядок = порядок байтов UTF-8
allowed = set("абвгдеёжзийклмнопрстуфхцчшщъыьэюяАБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ-")

rows, seen = [], set()
with open(SRC, encoding="utf-8", newline="") as f:
    for d in csv.DictReader(f):
        fam, nm = d["last_name"].strip(), d["first_name"].strip()
        if not fam or not nm or fam[0] not in ALPHA:
            continue
        if not (set(fam) <= allowed and set(nm) <= allowed):
            continue
        if len(fam.encode()) > 60 or len(nm.encode()) > 60:  # влезает в char[64]
            continue
        k = (fam, nm)
        if k not in seen:
            seen.add(k)
            rows.append(k)

cnt = collections.Counter(f[0] for f, _ in rows)
mx = max(cnt.values())
print(f"уникальных пар: {len(rows)}")

sample = random.sample(rows, 100_000)
sample_set = set(sample)
sample.sort()
with open("names100k.csv", "w", encoding="utf-8") as o:
    for a, b in sample:
        o.write(f"{a};{b}\n")

rest = [r for r in rows if r not in sample_set]

real = random.sample(rest, 2000)
with open("test_real.csv", "w", encoding="utf-8") as o:
    for a, b in real:
        o.write(f"{a};{b}\n")

by_letter = collections.defaultdict(list)
for r in rest:
    by_letter[r[0][0]].append(r)
letters = [c for c in ALPHA if len(by_letter[c]) >= 500]
uni, used = [], set()
while len(uni) < 2000:
    r = random.choice(by_letter[random.choice(letters)])
    if r not in used:
        used.add(r)
        uni.append(r)
with open("test_uniform.csv", "w", encoding="utf-8") as o:
    for a, b in uni:
        o.write(f"{a};{b}\n")

with open("weights.h", "w", encoding="utf-8") as o:
    o.write("/* Сгенерировано gen_data.py. Вес = число пар (фамилия;имя) с этой первой буквой / максимум (К = 1.0).\n")
    o.write("   Порядок индексов = порядок байтов UTF-8 заглавных букв: Ё, А, Б, ..., Я. */\n")
    o.write("#ifndef WEIGHTS_H\n#define WEIGHTS_H\n\n#define LETTER_COUNT 33\n\n")
    o.write("static const double LETTER_WEIGHTS[LETTER_COUNT] = {\n")
    for c in ALPHA:
        o.write(f"    {cnt[c] / mx:.6f}, /* {c} */\n")
    o.write("};\n\n#endif\n")

print("буквы в равномерном тесте:", "".join(letters))
print("топ-8:", ", ".join(f"{c} {n / len(rows) * 100:.1f}%" for c, n in cnt.most_common(8)))
