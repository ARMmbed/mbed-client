#!/bin/bash
input="binaries.txt"
valgrind_logs="valgrind_logs"
rm -rf $valgrind_logs
mkdir $valgrind_logs
find $1 -type f -executable -exec sh -c "file -i '{}' | grep -q 'x-executable; charset=binary'" \; -print > $input
while file= read -r binary
do
    valgrind --log-file="${valgrind_logs}/valgrind_$(basename $binary).log" "$binary"
done < "$input"
