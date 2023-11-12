name=$1
key='wikxwqtnyxlghroe'
host='htmlInf'
username='vjfritzen'
testFile='test.sh'

# Compress file
echo "Generating tar file $name.tgz"
tar czf "$name".tgz -X .tarignore .

# Check if test file exists
tests=false
if [ -f $testFile ]; then
    tests=true
else
    echo "WARN: File $testFile does not exist."
    echo "Skipping tests."
fi

# First test

echo "Testing tar file $name.tgz compiles"
mkdir testRun
cp "$name".tgz testRun
if [ $tests = true ]; then
    cp $testFile testRun
fi
(
cd testRun || exit
tar xzf "$name".tgz
make clean > /dev/null 2>&1
make > /dev/null 2>&1
if [ -f "$name" ]; then
    echo "File $name exists."
    echo "Compiles correctly."
else
    echo "File $name does not exist."
    echo "Make sure makefile is correct."
    exit 1
fi
if [ $tests = true ]; then
    bash $testFile || (
        echo "Tests failed."
        exit 1
    )
fi
) || exit 1
echo "Removing testRun"
rm -rf testRun

echo "Uploading tar file $name.tgz to $host"
sftp $host:public_html/$key/ <<EOF || exit 1
put $name.tgz
exit
EOF



echo "Testing tar file $name.tgz downloads and compiles"
mkdir testRun
if [ $tests = true ]; then
    cp $testFile testRun
fi
(
cd testRun || exit 1
wget https://inf.ufrgs.br/~$username/$key/"$name".tgz -q || (
    echo "File $name.tgz does not exist."
    exit 1
)
if [ ! -f "$name".tgz ]; then
    echo "File $name.tgz does not exist after download."
    exit 1
fi
tar xzf "$name".tgz
make > /dev/null 2>&1
if [ -f "$name" ]; then
    echo "File $name exists."
    echo "Compiles correctly."
else
    echo "After download, file $name does not exist."
    exit 1
fi
if [ $tests = true ]; then
    bash $testFile || (
        echo "Tests failed after download."
        exit 1
    )
fi
) || exit 1
rm -rf testRun
echo "Everything is OK."


rm "$name".tgz