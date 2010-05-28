#!/bin/sh

OMP_NUM_THREADS=8
export OMP_NUM_THREADS
./NativeCppBlackScholesSolver solveND 3 2 BStest3DMC.bound BStest3DMC.stoch 1.0 std_euro_call 0.00 1.0 0.1 CrNic 16000 0.00001 > test3D_level_2_8CPU.out
./NativeCppBlackScholesSolver solveND 3 3 BStest3DMC.bound BStest3DMC.stoch 1.0 std_euro_call 0.00 1.0 0.1 CrNic 16000 0.00001 > test3D_level_3_8CPU.out
./NativeCppBlackScholesSolver solveND 3 4 BStest3DMC.bound BStest3DMC.stoch 1.0 std_euro_call 0.00 1.0 0.1 CrNic 16000 0.00001 > test3D_level_4_8CPU.out
./NativeCppBlackScholesSolver solveND 3 5 BStest3DMC.bound BStest3DMC.stoch 1.0 std_euro_call 0.00 1.0 0.1 CrNic 16000 0.00001 > test3D_level_5_8CPU.out
./NativeCppBlackScholesSolver solveND 3 6 BStest3DMC.bound BStest3DMC.stoch 1.0 std_euro_call 0.00 1.0 0.1 CrNic 16000 0.00001 > test3D_level_6_8CPU.out
./NativeCppBlackScholesSolver solveND 3 7 BStest3DMC.bound BStest3DMC.stoch 1.0 std_euro_call 0.00 1.0 0.1 CrNic 16000 0.00001 > test3D_level_7_8CPU.out
./NativeCppBlackScholesSolver solveND 3 8 BStest3DMC.bound BStest3DMC.stoch 1.0 std_euro_call 0.00 1.0 0.1 CrNic 16000 0.00001 > test3D_level_8_8CPU.out
OMP_NUM_THREADS=4
export OMP_NUM_THREADS
./NativeCppBlackScholesSolver solveND 3 2 BStest3DMC.bound BStest3DMC.stoch 1.0 std_euro_call 0.00 1.0 0.1 CrNic 16000 0.00001 > test3D_level_2_4CPU.out
./NativeCppBlackScholesSolver solveND 3 3 BStest3DMC.bound BStest3DMC.stoch 1.0 std_euro_call 0.00 1.0 0.1 CrNic 16000 0.00001 > test3D_level_3_4CPU.out
./NativeCppBlackScholesSolver solveND 3 4 BStest3DMC.bound BStest3DMC.stoch 1.0 std_euro_call 0.00 1.0 0.1 CrNic 16000 0.00001 > test3D_level_4_4CPU.out
./NativeCppBlackScholesSolver solveND 3 5 BStest3DMC.bound BStest3DMC.stoch 1.0 std_euro_call 0.00 1.0 0.1 CrNic 16000 0.00001 > test3D_level_5_4CPU.out
./NativeCppBlackScholesSolver solveND 3 6 BStest3DMC.bound BStest3DMC.stoch 1.0 std_euro_call 0.00 1.0 0.1 CrNic 16000 0.00001 > test3D_level_6_4CPU.out
./NativeCppBlackScholesSolver solveND 3 7 BStest3DMC.bound BStest3DMC.stoch 1.0 std_euro_call 0.00 1.0 0.1 CrNic 16000 0.00001 > test3D_level_7_4CPU.out
./NativeCppBlackScholesSolver solveND 3 8 BStest3DMC.bound BStest3DMC.stoch 1.0 std_euro_call 0.00 1.0 0.1 CrNic 16000 0.00001 > test3D_level_8_4CPU.out
OMP_NUM_THREADS=2
export OMP_NUM_THREADS
./NativeCppBlackScholesSolver solveND 3 2 BStest3DMC.bound BStest3DMC.stoch 1.0 std_euro_call 0.00 1.0 0.1 CrNic 16000 0.00001 > test3D_level_2_2CPU.out
./NativeCppBlackScholesSolver solveND 3 3 BStest3DMC.bound BStest3DMC.stoch 1.0 std_euro_call 0.00 1.0 0.1 CrNic 16000 0.00001 > test3D_level_3_2CPU.out
./NativeCppBlackScholesSolver solveND 3 4 BStest3DMC.bound BStest3DMC.stoch 1.0 std_euro_call 0.00 1.0 0.1 CrNic 16000 0.00001 > test3D_level_4_2CPU.out
./NativeCppBlackScholesSolver solveND 3 5 BStest3DMC.bound BStest3DMC.stoch 1.0 std_euro_call 0.00 1.0 0.1 CrNic 16000 0.00001 > test3D_level_5_2CPU.out
./NativeCppBlackScholesSolver solveND 3 6 BStest3DMC.bound BStest3DMC.stoch 1.0 std_euro_call 0.00 1.0 0.1 CrNic 16000 0.00001 > test3D_level_6_2CPU.out
./NativeCppBlackScholesSolver solveND 3 7 BStest3DMC.bound BStest3DMC.stoch 1.0 std_euro_call 0.00 1.0 0.1 CrNic 16000 0.00001 > test3D_level_7_2CPU.out
./NativeCppBlackScholesSolver solveND 3 8 BStest3DMC.bound BStest3DMC.stoch 1.0 std_euro_call 0.00 1.0 0.1 CrNic 16000 0.00001 > test3D_level_8_2CPU.out
OMP_NUM_THREADS=1
export OMP_NUM_THREADS
./NativeCppBlackScholesSolver solveND 3 2 BStest3DMC.bound BStest3DMC.stoch 1.0 std_euro_call 0.00 1.0 0.1 CrNic 16000 0.00001 > test3D_level_2_1CPU.out
./NativeCppBlackScholesSolver solveND 3 3 BStest3DMC.bound BStest3DMC.stoch 1.0 std_euro_call 0.00 1.0 0.1 CrNic 16000 0.00001 > test3D_level_3_1CPU.out
./NativeCppBlackScholesSolver solveND 3 4 BStest3DMC.bound BStest3DMC.stoch 1.0 std_euro_call 0.00 1.0 0.1 CrNic 16000 0.00001 > test3D_level_4_1CPU.out
./NativeCppBlackScholesSolver solveND 3 5 BStest3DMC.bound BStest3DMC.stoch 1.0 std_euro_call 0.00 1.0 0.1 CrNic 16000 0.00001 > test3D_level_5_1CPU.out
./NativeCppBlackScholesSolver solveND 3 6 BStest3DMC.bound BStest3DMC.stoch 1.0 std_euro_call 0.00 1.0 0.1 CrNic 16000 0.00001 > test3D_level_6_1CPU.out
./NativeCppBlackScholesSolver solveND 3 7 BStest3DMC.bound BStest3DMC.stoch 1.0 std_euro_call 0.00 1.0 0.1 CrNic 16000 0.00001 > test3D_level_7_1CPU.out
./NativeCppBlackScholesSolver solveND 3 8 BStest3DMC.bound BStest3DMC.stoch 1.0 std_euro_call 0.00 1.0 0.1 CrNic 16000 0.00001 > test3D_level_8_1CPU.out
