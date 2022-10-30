    #plotting of the data in blocks
  
    set title sprintf("t / Δt = %i",k)

    #grid size is set manually in the plot command

    plot [0:400][0:400] 'walker_positions.dat' i k u 1:($5==1 ? $2 : 1/0):( $3 == 0 ? (2-$4) : 0 ) w p pt 5 ps 1 lc palette title "mask shape",\
    'walker_positions.dat' i k u 1:($5==0 ? $2 : 1/0):( $3 == 0 ? (2-$4) : 0 ) w p pt 7 ps 1 lc palette title "no mask shape"

    if(k == 50) {dk = 5;}
    if(k == 250) {dk = 10;}
    if(k == 1000) {dk = 100;}
    if(k == 1600) {dk = 200;}
    k = k + dk
    # plotting again if we haven't reached the last block
    if (k < nk) reread 
