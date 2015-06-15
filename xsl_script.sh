#!/bin/bash

echo 
echo "Creating report"
echo 

echo '<?xml version="1.0" encoding="UTF-8" ?>
<?xml-stylesheet type="text/xsl" href="junit_xsl.xslt"?>
<list>' >> lcov/index.xml

for f in lcov/results/*.xml
do
name=${f##*/}
echo '<entry name="results/'"$name"'" />'>> lcov/index.xml
done

echo '</list>' >> lcov/index.xml

echo
echo "Report created to lcov/index.xml (outputs html)"
echo