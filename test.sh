./etapa2 > /dev/null 2>&1
if [ $? -eq 1 ]; then
    echo "Teste 1: OK"
else
    echo "Teste 1: ERRO"
    exit 1
fi

./etapa2 notExistsFiledfkasdfg> /dev/null 2>&1
if [ $? -eq 2 ]; then
    echo "Teste 2: OK"
else
    echo "Teste 2: ERRO"
    exit 1
fi

cp ../in .
./etapa2 in> /dev/null 2>&1
if [ $? -eq 0 ]; then
    echo "Teste 3: OK"
else
    echo "Teste 3: ERRO"
    exit 1
fi
