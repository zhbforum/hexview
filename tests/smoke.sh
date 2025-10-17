set -euo pipefail

echo "[test] version"
./hexview -v | grep -E '^hexview '

echo "[test] hello compact"
echo -n "Hello World!" > sample.bin
./hexview sample.bin --compact --color=never | grep -q '48656C6C6F20576F726C6421'

echo "[test] seq offset/length"
python - << 'PY'
with open('seq.bin','wb') as f:
    f.write(bytes(range(256)))
PY
out=$(./hexview seq.bin -o 0x10 -l 16 --compact --color=never | head -n1)
echo "$out" | grep -q '101112131415161718191A1B1C1D1E1F'

echo "[test] stdin"
cat sample.bin | ./hexview - --compact --color=never > /dev/null

echo "[OK]"
