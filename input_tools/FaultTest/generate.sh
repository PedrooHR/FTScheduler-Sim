
for i in "$@"; do
    mkdir machines$i
    for j in 1 2 3 4 5 6 7 8 9 10; do
        ./ft 25200 $i 1000 $i 1 > _f.cfg

        mv _f.cfg machines$i/_f$j.cfg
    done
done