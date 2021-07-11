#!/bin/bash

echo "Run gradle build.."
echo "Root directory: $1"

pushd $(pwd)

cd $1
gradle build

popd
