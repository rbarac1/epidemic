# SETTINGS RESET
unset multiplot
reset
set encoding utf8

# gif TERMINAL
set term gif animate delay 4 loop 1\
    font "Arial,18pt" \
    size 1100,800
set output "epidemic.gif"

#PNG OUTPUT FOR INITIAL CONDITIONS
#set term png font "Arial,18pt" size 1000,700 fontscale 1.0 dl 2.0
#set output 'epidemic0.png'

stats 'walker_positions.dat' #adding the number of steps manually can be faster
nk=STATS_blocks # total number of steps
dk=5  # block step size
k=1   # initial block

set xlabel 'x'
set ylabel 'y'
set size ratio -1

stats 'Nwalkers.dat' name 'b'
    set label 1 sprintf("N_{w,initial} = %i", b_max_y) at screen 0.2, screen 0.98
    set label 2 sprintf("N_{w,final} = %i", b_min_y) at screen 0.2, screen 0.94
#set label 1 "N_w = 20" at screen 0.2, screen 0.95
#set label 1 "immune" at screen 0.74, screen 0.91
#set label 2 "susceptible" at screen 0.74, screen 0.5
#set label 3 "infected" at screen 0.74, screen 0.1

unset key

set palette model RGB defined ( 0 'red', 1 'green' , 2 'blue')
set cbrange [0:2]
set cbtics ("infected" 0, "susceptible" 1, "protected" 2)
#use this cbtics if immunity is the only source of protection
#set cbtics ("infected" 0, "susceptible" 1, "immune" 2)
#unset colorbox

# PLOTTING THE IMAGES
    load "step.plt"

# RESET SETTINGS
unset output
set terminal GNUTERM
