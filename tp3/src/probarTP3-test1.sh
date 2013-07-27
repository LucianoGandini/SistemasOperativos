# /bin/bash
mpiexec -np 2 ./tp3 >/dev/null 2>> output2
./test "cat ./output2"
mpiexec -np 4 ./tp3 >/dev/null 2>> output4
./test "cat ./output4"
mpiexec -np 6 ./tp3 >/dev/null 2>> output6
./test "cat ./output6"
mpiexec -np 8 ./tp3 >/dev/null 2>> output8
./test "cat ./output8"
mpiexec -np 10 ./tp3 >/dev/null 2>> output10
./test "cat ./output10"
mpiexec -np 12 ./tp3 >/dev/null 2>> output12
./test "cat ./output12"
mpiexec -np 14 ./tp3 >/dev/null 2>> output14
./test "cat ./output14"
mpiexec -np 16 ./tp3 >/dev/null 2>> output16
./test "cat ./output16"
mpiexec -np 18 ./tp3 >/dev/null 2>> output18
./test "cat ./output18"
mpiexec -np 20 ./tp3 >/dev/null 2>> output20
./test "cat ./output20"
mpiexec -np 22 ./tp3 >/dev/null 2>> output22
./test "cat ./output22"
mpiexec -np 24 ./tp3 >/dev/null 2>> output24
./test "cat ./output24"
mpiexec -np 26 ./tp3 >/dev/null 2>> output26
./test "cat ./output26"
mpiexec -np 28 ./tp3 >/dev/null 2>> output28
./test "cat ./output28"
mpiexec -np 30 ./tp3 >/dev/null 2>> output30
./test "cat ./output30"
rm -f output*
