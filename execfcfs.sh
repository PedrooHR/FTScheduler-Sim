rootdir=$(pwd)
fcfsdir=$(pwd)/fcfs/
ppqdir=$(pwd)/scheduler/
ftdir=$(pwd)/input_tools/FaultTest
mgendir=$(pwd)/input_tools/MachineGen
cd $fcfsdir
make cleanall
make
rm *.cfg *.txt

cd $rootdir

rm Results/fcfs/*.txt

for run in 1 2 3 4 5 6 7 8 9 10; do
    #update machines and faults
    # cd  $ppqdir/Datasets/avianflu/ && rm *_f.cfg *_m.txt && cp $ftdir/machines50/_f$run.cfg avianflu_f.cfg && cp $mgendir/machines50/_m$run.txt avianflu_m.txt
    # cd $fcfsdir/Datasets/avianflu/ && rm *_f.cfg *_m.txt && cp $ftdir/machines50/_f$run.cfg avianflu_f.cfg && cp $mgendir/machines50/_m$run.txt avianflu_m.txt
    # cd  $ppqdir/Datasets/Epigenomics/ && rm *_f.cfg *_m.txt && cp $ftdir/machines50/_f$run.cfg Epigenomics_f.cfg && cp $mgendir/machines50/_m$run.txt Epigenomics_m.txt
    # cd $fcfsdir/Datasets/Epigenomics/ && rm *_f.cfg *_m.txt && cp $ftdir/machines50/_f$run.cfg Epigenomics_f.cfg && cp $mgendir/machines50/_m$run.txt Epigenomics_m.txt
    # cd  $ppqdir/Datasets/Inspiral/ && rm *_f.cfg *_m.txt && cp $ftdir/machines50/_f$run.cfg Inspiral_f.cfg && cp $mgendir/machines50/_m$run.txt Inspiral_m.txt
    # cd $fcfsdir/Datasets/Inspiral/ && rm *_f.cfg *_m.txt && cp $ftdir/machines50/_f$run.cfg Inspiral_f.cfg && cp $mgendir/machines50/_m$run.txt Inspiral_m.txt
    # cd  $ppqdir/Datasets/Montage/ && rm *_f.cfg *_m.txt && cp $ftdir/machines50/_f$run.cfg Montage_f.cfg && cp $mgendir/machines50/_m$run.txt Montage_m.txt
    # cd $fcfsdir/Datasets/Montage/ && rm *_f.cfg *_m.txt && cp $ftdir/machines50/_f$run.cfg Montage_f.cfg && cp $mgendir/machines50/_m$run.txt Montage_m.txt
    # cd  $ppqdir/Datasets/motif/ && rm *_f.cfg *_m.txt && cp $ftdir/machines50/_f$run.cfg motif_f.cfg && cp $mgendir/machines50/_m$run.txt motif_m.txt
    # cd $fcfsdir/Datasets/motif/ && rm *_f.cfg *_m.txt && cp $ftdir/machines50/_f$run.cfg motif_f.cfg && cp $mgendir/machines50/_m$run.txt motif_m.txt
    # cd  $ppqdir/Datasets/psload/ && rm *_f.cfg *_m.txt && cp $ftdir/machines50/_f$run.cfg psload_f.cfg && cp $mgendir/machines50/_m$run.txt psload_m.txt
    # cd $fcfsdir/Datasets/psload/ && rm *_f.cfg *_m.txt && cp $ftdir/machines50/_f$run.cfg psload_f.cfg && cp $mgendir/machines50/_m$run.txt psload_m.txt
    # cd  $ppqdir/Datasets/psmerge/ && rm *_f.cfg *_m.txt && cp $ftdir/machines50/_f$run.cfg psmerge_f.cfg && cp $mgendir/machines50/_m$run.txt psmerge_m.txt
    # cd $fcfsdir/Datasets/psmerge/ && rm *_f.cfg *_m.txt && cp $ftdir/machines50/_f$run.cfg psmerge_f.cfg && cp $mgendir/machines50/_m$run.txt psmerge_m.txt
    # cd  $ppqdir/Datasets/scoop/ && rm *_f.cfg *_m.txt && cp $ftdir/machines50/_f$run.cfg scoop_f.cfg && cp $mgendir/machines50/_m$run.txt scoop_m.txt
    # cd $fcfsdir/Datasets/scoop/ && rm *_f.cfg *_m.txt && cp $ftdir/machines50/_f$run.cfg scoop_f.cfg && cp $mgendir/machines50/_m$run.txt scoop_m.txt
    # cd  $ppqdir/Datasets/Sipht/ && rm *_f.cfg *_m.txt && cp $ftdir/machines50/_f$run.cfg Sipht_f.cfg && cp $mgendir/machines50/_m$run.txt Sipht_m.txt
    # cd $fcfsdir/Datasets/Sipht/ && rm *_f.cfg *_m.txt && cp $ftdir/machines50/_f$run.cfg Sipht_f.cfg && cp $mgendir/machines50/_m$run.txt Sipht_m.txt
    # cd  $ppqdir/Datasets/gene2life/ && rm *_f.cfg *_m.txt && cp $ftdir/machines16/_f$run.cfg gene2life_f.cfg && cp $mgendir/machines16/_m$run.txt gene2life_m.txt
    # cd $fcfsdir/Datasets/gene2life/ && rm *_f.cfg *_m.txt && cp $ftdir/machines16/_f$run.cfg gene2life_f.cfg && cp $mgendir/machines16/_m$run.txt gene2life_m.txt
    # cd  $ppqdir/Datasets/floodplain/ && rm *_f.cfg *_m.txt && cp $ftdir/machines14/_f$run.cfg floodplain_f.cfg && cp $mgendir/machines14/_m$run.txt floodplain_m.txt
    # cd $fcfsdir/Datasets/floodplain/ && rm *_f.cfg *_m.txt && cp $ftdir/machines14/_f$run.cfg floodplain_f.cfg && cp $mgendir/machines14/_m$run.txt floodplain_m.txt
    # cd  $ppqdir/Datasets/leadmm/ && rm *_f.cfg *_m.txt && cp $ftdir/machines12/_f$run.cfg leadmm_f.cfg && cp $mgendir/machines12/_m$run.txt leadmm_m.txt
    # cd $fcfsdir/Datasets/leadmm/ && rm *_f.cfg *_m.txt && cp $ftdir/machines12/_f$run.cfg leadmm_f.cfg && cp $mgendir/machines12/_m$run.txt leadmm_m.txt
    # cd  $ppqdir/Datasets/leadadas/ && rm *_f.cfg *_m.txt && cp $ftdir/machines12/_f$run.cfg leadadas_f.cfg && cp $mgendir/machines12/_m$run.txt leadadas_m.txt
    # cd $fcfsdir/Datasets/leadadas/ && rm *_f.cfg *_m.txt && cp $ftdir/machines12/_f$run.cfg leadadas_f.cfg && cp $mgendir/machines12/_m$run.txt leadadas_m.txt
    # cd  $ppqdir/Datasets/molsci/ && rm *_f.cfg *_m.txt && cp $ftdir/machines10/_f$run.cfg molsci_f.cfg && cp $mgendir/machines10/_m$run.txt molsci_m.txt
    # cd $fcfsdir/Datasets/molsci/ && rm *_f.cfg *_m.txt && cp $ftdir/machines10/_f$run.cfg molsci_f.cfg && cp $mgendir/machines10/_m$run.txt molsci_m.txt
    # cd  $ppqdir/Datasets/cadsr/ && rm *_f.cfg *_m.txt && cp $ftdir/machines8/_f$run.cfg cadsr_f.cfg && cp $mgendir/machines8/_m$run.txt cadsr_m.txt
    # cd $fcfsdir/Datasets/cadsr/ && rm *_f.cfg *_m.txt && cp $ftdir/machines8/_f$run.cfg cadsr_f.cfg && cp $mgendir/machines8/_m$run.txt cadsr_m.txt
    # cd  $ppqdir/Datasets/glimmer/ && rm *_f.cfg *_m.txt && cp $ftdir/machines8/_f$run.cfg glimmer_f.cfg && cp $mgendir/machines8/_m$run.txt glimmer_m.txt
    # cd $fcfsdir/Datasets/glimmer/ && rm *_f.cfg *_m.txt && cp $ftdir/machines8/_f$run.cfg glimmer_f.cfg && cp $mgendir/machines8/_m$run.txt glimmer_m.txt
    # cd  $ppqdir/Datasets/mcstas/ && rm *_f.cfg *_m.txt && cp $ftdir/machines6/_f$run.cfg mcstas_f.cfg && cp $mgendir/machines6/_m$run.txt mcstas_m.txt
    # cd $fcfsdir/Datasets/mcstas/ && rm *_f.cfg *_m.txt && cp $ftdir/machines6/_f$run.cfg mcstas_f.cfg && cp $mgendir/machines6/_m$run.txt mcstas_m.txt
    # cd  $ppqdir/Datasets/leaddm/ && rm *_f.cfg *_m.txt && cp $ftdir/machines6/_f$run.cfg leaddm_f.cfg && cp $mgendir/machines6/_m$run.txt leaddm_m.txt
    # cd $fcfsdir/Datasets/leaddm/ && rm *_f.cfg *_m.txt && cp $ftdir/machines6/_f$run.cfg leaddm_f.cfg && cp $mgendir/machines6/_m$run.txt leaddm_m.txt
    # cd  $ppqdir/Datasets/mememast/ && rm *_f.cfg *_m.txt && cp $ftdir/machines4/_f$run.cfg mememast_f.cfg && cp $mgendir/machines4/_m$run.txt mememast_m.txt
    # cd $fcfsdir/Datasets/mememast/ && rm *_f.cfg *_m.txt && cp $ftdir/machines4/_f$run.cfg mememast_f.cfg && cp $mgendir/machines4/_m$run.txt mememast_m.txt
    # cd  $ppqdir/Datasets/stencilred/ && rm *_f.cfg *_m.txt && cp $ftdir/machines50/_f$run.cfg stencilred_f.cfg && cp $mgendir/machines50/_m$run.txt stencilred_m.txt
    cd $fcfsdir/Datasets/stencilred/ && rm *_f.cfg *_m.txt && cp $ftdir/machines50/_f$run.cfg stencilred_f.cfg && cp $mgendir/machines50/_m$run.txt stencilred_m.txt

    #run on fcfs
    cd $fcfsdir 
    bash $fcfsdir/rundatasets.sh 
    cat Results/compiled.txt >> $rootdir/Results/fcfs/Compiled.txt
    
done
