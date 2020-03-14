rootdir=$(pwd)
#make cleanall
rm *.txt *.cfg
rm -rf Results/
rm -rf EvLogs/
#make
mkdir Results
mkdir EvLogs

touch compiled.txt

datasetsdir="$(pwd)/Datasets"

cd $datasetsdir

for JobString in *; do
    echo "Running $JobString Dataset"

    echo "Copying files to root"
    cp $JobString/* $rootdir
    cd $rootdir
    
    mkdir $rootdir/Results/$JobString

    for arg in "$@";
    do
        echo "Running Application for parameter Rfactor=$arg"
        ./scheduler $JobString $arg >> "EvLogs/"$JobString"_ExLog"$arg".txt"
        cp EventLog.txt $rootdir/Results/$JobString/EventLog_$arg.txt 
    done
    
    echo "" >> compiled.txt

    cd $datasetsdir
done
cd $rootdir

cp compiled.txt $rootdir/Results/compiled.txt
rm -rf *.cfg *.txt


# for JobString in Genome/*.txt; do
#     jobnoext=${JobString:7:-4}
#     echo "Running $jobnoext Dataset"

#     echo "Copying files to root"
#     cp Genome/$jobnoext.txt $jobnoext.txt
#     cp Genome/$jobnoext.cfg $jobnoext.cfg
    
#     mkdir $rootdir/Results/$jobnoext

#     for arg in "$@";
#     do
#         echo "Running Application for parameter Rfactor=$arg"
#         ./scheduler $jobnoext $arg >> "EvLogs/"$jobnoext"_ExLog"$arg".txt"
#         cp EventLog.txt $rootdir/Results/$jobnoext/EventLog_$arg.txt 
#     done
    
#     echo "" >> compiled.txt

#     rm $jobnoext.txt $jobnoext.cfg

# done

# cp compiled.txt $rootdir/Results/compiled.txt
# rm -rf *.cfg *.txt
