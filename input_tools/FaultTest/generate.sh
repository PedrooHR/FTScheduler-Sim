
for i in "$@"; do
    mkdir machines$i
    for j in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20; do
        ./ft 25200 $i 1000 $i 3 > _f.cfg

        mv _f.cfg machines$i/_f$j.cfg
    done
done