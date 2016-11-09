set terminal png size 1024,768
set termoption enhanced
set encoding utf8

# Pour changer le titre
set title "Mon super graphique"

set style data lines

# Les legendes des axes
set xlabel "Taille Buffer"
set ylabel "Temps CPU"

# Pour changer le nom du fichier produit
set output 'graphique.png'

# La commande pour tracer. Le \ est obligatoire avant un passage à la ligne
plot 'gnuplot.values' u 1:2 title "Colonne 1" lw 2 linecolor rgb "red", \
     '' u 1:3 title "Colonne 2" lw 2 linecolor rgb "blue"
