
for i in "$@"; do
    mkdir machines$i
    for j in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20; do
        ./machinegen $i machine

        mv machine_m.txt machines$i/_m$j.txt
    done
done