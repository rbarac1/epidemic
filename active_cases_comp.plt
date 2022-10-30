#ONE OF THE .dat FILES IS GIVEN AN INDEX "1" MANUALLY

unset multiplot
reset
set encoding utf8

#PLOT SETTINGS: IMAGE SIZE, FONT SIZE, IMAGE NAME
set term png font "Arial,18pt" size 1000,700 fontscale 1.0 dl 2.0
set output 'active_cases_comp.png'

set samples 10000
set bmargin 0.
set lmargin 0.
set rmargin 0.
set tmargin 0.

#set style line 1  lt 1 lw 2 lc rgb "#071F61"

set multiplot
  set origin 0.15,0.13 #MOVING OF THE ORIGIN TO FIT THE AXIS LABELS
  set size   0.8,0.75 #SIZE OF THE FIGURE IN THE TOTAL IMAGE (MAX IS 1.0 X 1.0)
    set title "Number of active cases over time, different mask ratio comparison"
    set key top right #PUTS THE LEGEND IN THE TOP RIGHT CORNER OF THE FIGURE
    #unset key
    set grid #DRAWS THE GRID
    set xlabel 't[steps]'
    set ylabel 'Number of active cases'


    stats 'Nwalkers.dat' name 'b'
    set label 1 sprintf("N_{w,initial} = %i", b_max_y) at graph 0.05, graph 0.95
    #set label 2 sprintf("N_{w,final} = %i", b_min_y) at graph 0.05, graph 0.87
    #set label 1 sprintf("N_{w,initial} = %i", b_max_y) at graph 0.75, graph 0.95
    #set label 2 sprintf("N_{w,final} = %i", b_min_y) at graph 0.75, graph 0.9
    #set label 3 "N_{w,masks}/N_{w,initial} = 50%" at graph 0.75, graph 0.85

    stats 'active_cases.dat' name 'a'
    plot [a_min_x:a_max_x] 'active_cases1.dat' w l lw 3.0 lc "red" title "50% masks",\
    'active_cases.dat' w l lw 3.0 lc "blue" title "80% masks"

unset multiplot
unset output
set term GNUTERM