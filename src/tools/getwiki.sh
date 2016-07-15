#!/bin/bash
ssh root@cfrankb.fb 'zip export.zip /var/www/cfrankb/lgck/wiki/ -r'
scp root@cfrankb.fb:~/export.zip .
unzip export -d tmp/
cd tmp/var/
find .
#rm -rf tmp/
