rootdir=$(pwd)
#make cleanall
rm *.txt *.cfg
rm -rf Results/
rm -rf ExLogs/
#make
mkdir Results
mkdir ExLogs

touch compiled.txt

datasetsdir="$(pwd)/Datasets"

cd $datasetsdir

for JobString in *; do
    echo "Running $JobString Dataset"

    echo "Copying files to root"
    cp $JobString/* $rootdir
    cd $rootdir
    
    mkdir $rootdir/Results/$JobString

    echo "Running Application for parameter #Redundancy=2"
    ./scheduler $JobString 1 2 >> "ExLogs/"$JobString"_ExLogRed2.txt"
    cp EventLog.txt $rootdir/Results/$JobString/EventLog_Red2.txt 

    echo "Running Application for parameter #Redundancy=3"
    ./scheduler $JobString 1 3 >> "ExLogs/"$JobString"_ExLogRed3.txt"
    cp EventLog.txt $rootdir/Results/$JobString/EventLog_Red3.txt 

    for arg in 900 1800 2700 3600;
    do
        echo "Running Application for parameter CP Interval=$arg"
        ./scheduler $JobString 0 $arg >> "ExLogs/"$JobString"_ExLogCP"$arg".txt"
        cp EventLog.txt $rootdir/Results/$JobString/EventLog_CP$arg.txt 
    done
 
    echo "Running Application with Restarting"
    ./scheduler $JobString >> "ExLogs/"$JobString"_ExLogDefault.txt"
    cp EventLog.txt $rootdir/Results/$JobString/EventLog_Default.txt 

    echo "" >> compiled.txt

    cd $datasetsdir
done
cd $rootdir

cp compiled.txt $rootdir/Results/compiled.txt
rm -rf *.cfg *.txt