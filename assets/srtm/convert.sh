# get more data
# wget 'http://srtm.csi.cgiar.org/SRT-ZIP/SRTM_V41/SRTM_Data_GeoTiff/srtm_13_06.zip'
# GTOPO30 https://earthexplorer.usgs.gov/
for x in *.tif; do
    echo $x
    # -scale range defined by 12_03 range
    BASE=`basename $x .tif`
    PNG="${BASE}.png"
    SMALL_PNG="${BASE}-2048x2048.png"
    if [ -f $SMALL_PNG ]; then
        continue
    fi
    gdal_translate -of PNG -ot Byte -scale '-300' '4371' $x $PNG
    convert -scale 2048x2048 $PNG $SMALL_PNG
done
