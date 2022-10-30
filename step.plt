    #plotting of the data in blocks
  
    set title sprintf("t / Δt = %i",k)

    #grid size is set manually in the plot command

    plot [0:50][0:50] 'walker_positions.dat' i k u 1:2:( $3 == 0 ? (2-$4) : 0 ) w p pt 7 ps 3 lc palette 
    k = k + dk
    # plotting again if we haven't reached the last block
    if (k < nk) reread 