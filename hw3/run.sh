rmmod sys_xjob
insmod sys_xjob.ko

./demo -c /usr/src/hw3-cse506g12/hw3/amin1.chk /usr/src/hw3-cse506g12/hw3/big.pdf
./demo -c /usr/src/hw3-cse506g12/hw3/amin2.chk /usr/src/hw3-cse506g12/hw3/big.pdf
./demo -c /usr/src/hw3-cse506g12/hw3/amin3.chk /usr/src/hw3-cse506g12/hw3/big.pdf
./demo -c /usr/src/hw3-cse506g12/hw3/amin4.chk /usr/src/hw3-cse506g12/hw3/big.pdf
./demo -c /usr/src/hw3-cse506g12/hw3/amin5.chk /usr/src/hw3-cse506g12/hw3/big.pdf
./demo -c /usr/src/hw3-cse506g12/hw3/amin6.chk /usr/src/hw3-cse506g12/hw3/big.pdf
./demo -c /usr/src/hw3-cse506g12/hw3/amin7.chk /usr/src/hw3-cse506g12/hw3/big.pdf
./demo -c /usr/src/hw3-cse506g12/hw3/amin8.chk /usr/src/hw3-cse506g12/hw3/big.pdf

./demo -e /usr/src/hw3-cse506g12/hw3/amin1.enc /usr/src/hw3-cse506g12/hw3/big.pdf 'abc'
./demo -e /usr/src/hw3-cse506g12/hw3/amin2.enc /usr/src/hw3-cse506g12/hw3/big.pdf 'abc'
./demo -e /usr/src/hw3-cse506g12/hw3/amin3.enc /usr/src/hw3-cse506g12/hw3/big.pdf 'abc'
./demo -e /usr/src/hw3-cse506g12/hw3/amin4.enc /usr/src/hw3-cse506g12/hw3/big.pdf 'abc'
./demo -z /usr/src/hw3-cse506g12/hw3/amin1.zip /usr/src/hw3-cse506g12/hw3/big.pdf
./demo -c /usr/src/hw3-cse506g12/hw3/amin9.chk /usr/src/hw3-cse506g12/hw3/big.pdf

./demo -e /usr/src/hw3-cse506g12/hw3/syed1.enc /usr/src/hw3-cse506g12/hw3/big.pdf 'abc'
./demo -e /usr/src/hw3-cse506g12/hw3/syed2.enc /usr/src/hw3-cse506g12/hw3/big.pdf 'abc'
./demo -e /usr/src/hw3-cse506g12/hw3/syed3.enc /usr/src/hw3-cse506g12/hw3/big.pdf 'abc'
./demo -e /usr/src/hw3-cse506g12/hw3/syed4.enc /usr/src/hw3-cse506g12/hw3/big.pdf 'abc'
./demo -z /usr/src/hw3-cse506g12/hw3/syed1.zip /usr/src/hw3-cse506g12/hw3/big.pdf
./demo -c /usr/src/hw3-cse506g12/hw3/syed9.chk /usr/src/hw3-cse506g12/hw3/big.pdf


./demo -N
./demo -T 11
./demo -T 12
./demo -T 13
./demo -N
