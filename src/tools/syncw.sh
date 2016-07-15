#!/bin/bash

#/home/cfrankb/toolkit/qt/lgck/techdocs/documents/manual5c.edoc -o /home/cfrankb/toolkit/qt/lgck/tools/wiki/ -q
#cp ../build-easyDoc-Desktop-Release/easyDoc bin/easyDoc
bin/easydoc ../../techdocs/documents/manual5c.edoc -o wiki/ -q
scp wiki/*.txt root@cfrankb.fb:/var/www/cfrankb/lgck/wiki/data/pages
ssh root@cfrankb.fb 'rm -r /var/www/cfrankb/lgck/wiki/data/cache/*'
ssh root@cfrankb.fb 'chown www-data:www-data -R /var/www/cfrankb/lgck/wiki/data/pages/'

#/pink/extra/www/cfrankb2/lgck/wiki/sync.sh
