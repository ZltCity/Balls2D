#!/bin/bash

RUN_DIR=$(pwd)

echo "Building for Android.."
echo "run dir: ${RUN_DIR}"

if [[ -z ${ANDROID_SDK_ROOT} ]]; then
  echo "'ANDROID_SDK_ROOT' variable must be set."
  exit 1
fi


