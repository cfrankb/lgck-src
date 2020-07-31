#!/bin/bash
BASE_DIR=$(dirname "$(readlink -f "$0")")

# https://github.com/QuasarApp/CQtDeployer/wiki/DeployConfigFileEn
# https://github.com/QuasarApp/CQtDeployer/wiki/Options
# https://github.com/QuasarApp/CQtDeployer

#cqtdeployer -bin bin/lgck-builder qif -qifStyle quasar noTranslations clear
#cqtdeployer -binDir bin/ qif -qifStyle quasar noTranslations force-clear
#cqtdeployer -confFile lgck-builder.json qif -qifStyle quasar noTranslations force-clear
cqtdeployer -confFile lgck-builder.json noTranslations force-clear

cd Distro
zip ../setup/lgck-builder.zip -r *

