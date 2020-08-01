#!/bin/bash
BASE_DIR=$(dirname "$(readlink -f "$0")")
DATE=$(date +%F)
ZIP_FILE=../setup/lgck-builder-portable-0.6.0.9[linux].${DATE}.zip

# https://github.com/QuasarApp/CQtDeployer/wiki/DeployConfigFileEn
# https://github.com/QuasarApp/CQtDeployer/wiki/Options
# https://github.com/QuasarApp/CQtDeployer

#cqtdeployer -bin bin/lgck-builder qif -qifStyle quasar noTranslations clear
#cqtdeployer -binDir bin/ qif -qifStyle quasar noTranslations force-clear
#cqtdeployer -confFile lgck-builder.json qif -qifStyle quasar noTranslations force-clear
#cqtdeployer -confFile lgck-builder.json -extraLibs libqscintilla2_qt5,liblua5.2 noTranslations force-clear
rm -rf Distro
cqtdeployer -confFile lgck-builder.json noTranslations force-clear

cp -r resources/* Distro
cd Distro
zip ${ZIP_FILE} -r *
