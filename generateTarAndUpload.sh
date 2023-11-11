name=$1
key='wikxwqtnyxlghroe'
host='htmlInf'
tar cvzf $name.tgz -X .tarignore .
sftp  $host:public_html/$key/ <<EOF
put $name.tgz
EOF
rm $name.tgz