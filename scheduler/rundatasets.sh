rootdir=$(pwd)
make cleanall
rm *.txt *.cfg
rm -rf Results/
rm -rf EvLogs/
make
mkdir Results
mkdir EvLogs

datasetsdir="$(pwd)/Datasets"

cd $datasetsdir

for JobString in *; do
    echo "Running $JobString Dataset"

    echo "Copying files to root"
    cp $JobString/* $rootdir
    cd $rootdir
    
    mkdir $rootdir/Results/$JobString
    echo "Running Application for parameter Rfactor=1"
    ./scheduler $JobString 1 >> "EvLogs/"$JobString"_ExLog1.txt"
    cp EventLog.txt $rootdir/Results/$JobString/EventLog_1.txt 
    echo "Running Application for parameter Rfactor=2"
    ./scheduler $JobString 2 >> "EvLogs/"$JobString"_ExLog2.txt"
    cp EventLog.txt $rootdir/Results/$JobString/EventLog_2.txt
    echo "Running Application for parameter Rfactor=3"
    ./scheduler $JobString 3 >> "EvLogs/"$JobString"_ExLog3.txt"
    cp EventLog.txt $rootdir/Results/$JobString/EventLog_3.txt
    echo "Running Application for parameter Rfactor=4"
    ./scheduler $JobString 4 >> "EvLogs/"$JobString"_ExLog4.txt"
    cp EventLog.txt $rootdir/Results/$JobString/EventLog_4.txt

    rm *.cfg *.txt

    cd $datasetsdir

done


