#!/bin/bash

pushd .
mkdir /tmp/secret-build
mkdir /tmp/secret-build/secret-1.2.0
cp ~/git/php-secret/* /tmp/secret-build/secret-1.2.0/
mv /tmp/secret-build/secret-1.2.0/package.xml /tmp/secret-build/
cd /tmp/secret-build
tar -zcvf ~/ubuntu/secret-1.2.0.tgz *
popd
