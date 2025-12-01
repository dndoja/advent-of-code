OUTPUT=data/day3.txt
if [ -f $OUTPUT ] ; then
    rm $OUTPUT 
fi

grep -E "(mul\([0-9]*,[0-9]*\))|(do\(\))|(don't\(\))" data/day3_raw.txt -o >> $OUTPUT
sed -E -i "" -e  "s/(mul)|\(|\)//g" $OUTPUT
