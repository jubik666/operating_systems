# Пример на M2 Pro (10 логических ядер):
#   ./run_experiments.sh 2000 10

N=${1:-2000}
CORES=${2:-$( (command -v sysctl >/dev/null 2>&1 && sysctl -n hw.ncpu) || (command -v nproc >/dev/null 2>&1 && nproc) || echo 4)}
SEED=${3:-42}

echo "n=$N, логических ядер: $CORES, seed=$SEED"
echo


SEQ_LINE=$(./lab2 "$N" 0 "$SEED" | grep '^sequential')
echo "$SEQ_LINE"
echo

BELOW1=1
BELOW2=$((CORES / 5 > 1 ? CORES / 5 : 2))
BELOW3=$((CORES / 2 > BELOW2 ? CORES / 2 : BELOW2 + 1))
BELOW4=$((CORES - 1 > BELOW3 ? CORES - 1 : BELOW3 + 1))

THREADS_LIST="$BELOW1 $BELOW2 $BELOW3 $BELOW4 $CORES 16 128 1024"

printf "%-10s %-10s %-12s %-12s %-10s %-10s\n" "T" "n" "time_ms" "residual" "speedup" "efficiency"

for T in $THREADS_LIST; do
    LINE=$(./lab2 "$N" "$T" "$SEED" | grep '^parallel')
    T_MS=$(echo "$LINE" | sed -n 's/.*time_ms=\([0-9.]*\).*/\1/p')
    RES=$(echo "$LINE" | sed -n 's/.*residual=\([0-9.e+-]*\).*/\1/p')
    SPD=$(echo "$LINE" | sed -n 's/.*speedup=\([0-9.]*\).*/\1/p')
    EFF=$(echo "$LINE" | sed -n 's/.*efficiency=\([0-9.]*\).*/\1/p')
    printf "%-10s %-10s %-12s %-12s %-10s %-10s\n" "$T" "$N" "$T_MS" "$RES" "$SPD" "$EFF"
done
