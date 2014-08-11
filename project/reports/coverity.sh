#!/bin/sh

# Copyright (c) 2013-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#
# before to run this script you must set these sheel variable :
# $DKCoverityToken with token of digiKam project given by Coverity SCAN.
# $DKCoverityEmail with email adress to send SCAN result.
#
# See this url to see how to prepare your computer with Coverity SCAN tool:
# http://scan.coverity.com/projects/285/upload_form

cd ../..

# Manage build sub-dir
if [ -d "build" ]; then
    rm -rfv ./build
fi

if [[ "$OSTYPE" == "linux-gnu" ]]; then
    ./bootstrap.linux
elif [[ "$OSTYPE" == "darwin"* ]]; then
    ./bootstrap.macports
else
    echo "Unsupported platform..."
    exit -1
fi

# Get active git branches to create SCAN import description string
./gits branch | sed -e "s/*/#/g" | sed -e "s/On:/#On:/g" | grep "#" | sed -e "s/#On:/On:/g" | sed -e "s/#/BRANCH:/g" > ./build/git_branches.txt
desc=$(<build/git_branches.txt)

cd ./build

cov-build --dir cov-int --tmpdir ~/tmp make
tar czvf myproject.tgz cov-int

echo "-- SCAN Import description --"
echo $desc
echo "-----------------------------"

echo "Coverity Scan tarball 'myproject.tgz' uploading in progress..."

# To be sure that resolv domain is in cache
nslookup scan5.coverity.com

# To mesure uploading time
SECONDS=0

curl -# \
     --form project=digiKam \
     --form token=$DKCoverityToken \
     --form email=$DKCoverityEmail \
     --form file=@myproject.tgz \
     --form version=git-master \
     --form description="$desc" \
     http://scan5.coverity.com/cgi-bin/upload.py \
     > /dev/null

echo "Done. Coverity Scan tarball 'myproject.tgz' is uploaded and ready for analyse."
echo "That took approximately $SECONDS seconds to upload."