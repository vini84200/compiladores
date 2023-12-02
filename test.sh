./etapa3 > /dev/null 2>&1
if [ $? -eq 1 ]; then
    echo "Teste 1: OK"
else
    echo "Teste 1: ERRO"
    exit 1
fi

./etapa3 notExistsFiledfkasdfg asdasdf> /dev/null 2>&1
if [ $? -eq 2 ]; then
    echo "Teste 2: OK"
else
    echo "Teste 2: ERRO"
    exit 1
fi
touch inputEmpty

./etapa3 inputEmpty o> /dev/null 2>&1
if [ $? -eq 0 ]; then
    echo "Teste 3: OK"
else
    echo "Teste 3: ERRO"
    exit 1
fi

rm inputEmpty

for file in inputs/*; do
  echo "Testando $file";
  ./etapa3 $file outTest > /dev/null 2>&1
  if [ $? -eq 0 ]; then
    echo "Teste $file-A: OK"
  else
    echo "Teste $file-A: ERRO"
    echo "File $file failed to compile"
    exit 1
  fi

  ./etapa3 outTest outTest2 > /dev/null 2>&1
  if [ $? -eq 0 ]; then
    echo "Teste $file-B: OK"
  else
    echo "Teste $file-B: ERRO"
    echo "File $file failed to compile from outTest"
    exit 1
  fi

  diff outTest outTest2 > /dev/null 2>&1
  if [ $? -eq 0 ]; then
    echo "Teste $file-C: OK"
  else
    echo "Teste $file-C: ERRO"
    echo "Diff:"
    diff outTest outTest2
    exit 1
  fi

  rm outTest outTest2

done