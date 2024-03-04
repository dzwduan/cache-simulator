make clean
make


for file in traces:
do
    echo "Running $benchmark benchmark.."
    output=`unzip2 -kc traces/$file | ./cache  --icache=128:2:128:2 --dcache=64:4:128:2 --l2cache=128:8:128:50 --memspeed=100`
    echo -n "MIPS-R10k:     ";echo $output

    output=`unzip2 -kc traces/$file | ./cache  --icache=512:2:64:2 --dcache=256:4:64:2 --l2cache=16384:8:64:50 --memspeed=100`
    echo -n "Alpha-21264:   ";echo $output

    echo "=============================================="
done



