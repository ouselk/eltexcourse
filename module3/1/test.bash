#!/bin/bash
PROG="$(realpath "${1:-./a.out}")"

if [ ! -x "$PROG" ]; then
    echo "Не найден исполняемый файл: $PROG"
    exit 2
fi

WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT
cd "$WORK" || exit 2

# run <аргументы...>: печатает команду запуска, выполняет её и печатает код выхода
run() {
    printf '$ ./prog'
    printf ' %q' "$@"
    echo
    "$PROG" "$@"
    echo "код выхода: $?"
    echo
}

echo "===== Некорректные параметры ====="

run

run -p

echo hi > a.txt
run a.txt -p

run -p ch1 -p ch2 a.txt

run -p ch1

run no_such_file.txt

run "$(head -c 200 /dev/zero | tr '\0' 'a')"

echo "===== Корректная работа ====="

: > empty.txt                                            # пустой
head -c $((228*1024*1024)) /dev/urandom > big228.bin     # 228 МБ
LONGNAME="$(head -c 100 /dev/zero | tr '\0' 'l').txt"    # длинное имя (104 символа)
echo "long name file" > "$LONGNAME"
echo "обычный файл" > normal.txt                         # обычный

run empty.txt big228.bin "$LONGNAME" normal.txt

echo "===== Те же файлы через именованный канал ====="

run -p mych empty.txt big228.bin "$LONGNAME" normal.txt