#!/bin/bash
BASE_DIR=$(dirname "$(readlink -f "$0")")
DATE=$(date +%F)
ZIP_FILE=../setup/lgck-builder-portable-0.6.0.9[linux].${DATE}.zip

# https://github.com/QuasarApp/CQtDeployer/wiki/DeployConfigFileEn
# https://github.com/QuasarApp/CQtDeployer/wiki/Options
# https://github.com/QuasarApp/CQtDeployer

rm -rf Distro
cqtdeployer -confFile lgck-builder.json noTranslations force-clear

mkdir -p setup
cp -r resources/* Distro
cd Distro
zip ${ZIP_FILE} -r *
