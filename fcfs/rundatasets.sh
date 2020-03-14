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


# for JobString in Genome/*.txt; do
#     jobnoext=${JobString:7:-4}
#     echo "Running $jobnoext Dataset"

#     echo "Copying files to root"
#     cp Genome/$jobnoext.txt $jobnoext.txt
#     cp Genome/$jobnoext.cfg $jobnoext.cfg
    
#     mkdir $rootdir/Results/$jobnoext

#     echo "Running Application for parameter #Redundancy=2"
#     ./scheduler $jobnoext 1 2 >> "ExLogs/"$jobnoext"_ExLogRed2.txt"
#     cp EventLog.txt $rootdir/Results/$jobnoext/EventLog_Red2.txt 

#     echo "Running Application for parameter #Redundancy=3"
#     ./scheduler $jobnoext 1 3 >> "ExLogs/"$jobnoext"_ExLogRed3.txt"
#     cp EventLog.txt $rootdir/Results/$jobnoext/EventLog_Red3.txt 

#     for arg in 900 1800 2700 3600;
#     do
#         echo "Running Application for parameter CP Interval=$arg"
#         ./scheduler $jobnoext 0 $arg >> "ExLogs/"$jobnoext"_ExLogCP"$arg".txt"
#         cp EventLog.txt $rootdir/Results/$jobnoext/EventLog_CP$arg.txt 
#     done
 
#     echo "Running Application with Restarting"
#     ./scheduler $jobnoext >> "ExLogs/"$jobnoext"_ExLogDefault.txt"
#     cp EventLog.txt $rootdir/Results/$jobnoext/EventLog_Default.txt 

#     echo "" >> compiled.txt

# done

# cp compiled.txt $rootdir/Results/compiled.txt
# rm -rf *.cfg *.txt
