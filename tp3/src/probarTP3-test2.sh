# /bin/bash
mpiexec -np 2 ./tp3  r 5 137 559  >/dev/null 2>> output2
./test "cat ./output2"
mpiexec -np 4 ./tp3  k 5 459 571  y 7 176 377  >/dev/null 2>> output4
./test "cat ./output4"
mpiexec -np 6 ./tp3  c 7 870 682  i 2 155 310  w 5 786 243  >/dev/null 2>> output6
./test "cat ./output6"
mpiexec -np 8 ./tp3  o 10 445 133  x 7 800 230  l 6 567 296  i 6 559 138  >/dev/null 2>> output8
./test "cat ./output8"
mpiexec -np 10 ./tp3  b 5 306 761  u 6 935 411  v 3 993 628  a 10 971 234  a 6 273 575  >/dev/null 2>> output10
./test "cat ./output10"
mpiexec -np 12 ./tp3  k 5 260 389  d 4 939 924  z 5 808 940  m 9 539 606  b 4 932 216  l 7 972 486  >/dev/null 2>> output12
./test "cat ./output12"
mpiexec -np 14 ./tp3  w 3 735 654  m 1 789 637  x 2 260 134  s 4 421 664  g 8 791 781  h 4 108 649  i 7 439 532  >/dev/null 2>> output14
./test "cat ./output14"
mpiexec -np 16 ./tp3  d 7 694 660  v 2 701 734  f 3 150 800  l 3 951 355  a 6 988 741  f 2 592 923  o 8 792 163  c 1 793 707  >/dev/null 2>> output16
./test "cat ./output16"
mpiexec -np 18 ./tp3  d 8 929 327  b 2 942 111  o 10 767 140  y 2 719 632  x 2 176 328  d 1 749 757  m 7 915 153  l 2 390 525  v 10 709 614  >/dev/null 2>> output18
./test "cat ./output18"
mpiexec -np 20 ./tp3  i 1 148 816  r 1 929 393  q 8 375 536  d 3 494 585  w 8 782 416  t 4 845 863  n 3 728 413  t 7 458 647  k 3 648 438  q 3 330 543  >/dev/null 2>> output20
./test "cat ./output20"
mpiexec -np 22 ./tp3  k 7 112 890  f 7 167 823  e 3 401 645  h 7 239 351  y 6 556 909  q 2 868 324  o 3 225 505  y 7 411 650  t 5 374 788  x 7 548 665  b 2 464 586  >/dev/null 2>> output22
./test "cat ./output22"
mpiexec -np 24 ./tp3  m 2 627 510  x 10 323 373  c 6 201 180  d 8 654 388  b 2 944 372  u 3 519 963  t 6 833 447  s 2 710 194  o 4 537 768  v 3 849 901  l 4 103 424  f 6 758 483  >/dev/null 2>> output24
./test "cat ./output24"
mpiexec -np 26 ./tp3  j 1 490 810  p 1 149 747  t 9 701 204  q 7 543 357  b 6 747 976  h 9 365 245  i 6 395 438  x 3 539 502  v 1 652 744  m 4 227 893  r 3 215 395  l 10 521 413  y 10 285 617  >/dev/null 2>> output26
./test "cat ./output26"
mpiexec -np 28 ./tp3  n 5 509 786  j 2 348 213  b 5 372 829  z 8 507 897  v 10 653 686  u 2 481 652  c 2 508 994  a 6 796 366  q 6 772 534  p 6 513 710  t 4 705 656  b 8 107 273  k 5 839 613  q 6 398 800  >/dev/null 2>> output28
./test "cat ./output28"
mpiexec -np 30 ./tp3  m 2 340 281  f 9 614 740  f 7 460 933  y 10 876 195  p 2 894 974  w 6 247 868  r 6 910 685  q 10 175 371  z 3 556 217  c 4 274 238  d 4 949 409  p 6 777 534  i 5 648 124  c 8 518 276  d 6 805 580  >/dev/null 2>> output30
./test "cat ./output30"
rm -f output*
