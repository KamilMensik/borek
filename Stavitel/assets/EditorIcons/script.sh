for filename in *.svg; do
  Inkscape --export-type=png --export-dpi=200 --export-background-opacity=0 "${filename}" -w 128 -h 128
done
