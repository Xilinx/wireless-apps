# This syntax is invalid in new sh version
# use writeDevmem () {
# http://stackoverflow.com/questions/12468889/bash-script-error-function-not-found-why-would-this-appear
# function writeDevmem {
writeDevmem () {
   addr=$(($1 + $2))
   printf "DEVMEM WRITE: BaseAddr=0x%08x Offset=0x%04x Value=0x%04x to 0x%08x %s\n" $1 $2 $3 $addr $4
   devmem $addr 32 $3
}

readDevmem () {
   addr=$(($1 + $2))
   local=$(devmem $addr 32)
   printf "DEVMEM READ : BaseAddr=0x%08x Offset=0x%04x Value=%s from 0x%08x %s\n" $1 $2 $local $addr
}

readRxDemuxFilter () {
   readDevmem $1 $((0x00 + $2))
   readDevmem $1 $((0x04 + $2))
   readDevmem $1 $((0x0C + $2))
}

writeRxDemuxFilter () {
   writeDevmem $1 $((0x00 + $2)) $3
   writeDevmem $1 $((0x04 + $2)) $4
   writeDevmem $1 $((0x0C + $2)) $5
}

readRxDemuxFilters () {
   readRxDemuxFilter $1 0x00
   readRxDemuxFilter $1 0x10
   readRxDemuxFilter $1 0x20
}

setRxDemuxAllToCpri () {
   writeRxDemuxFilter $1 0x00 0x0 0x0 0x0
   writeRxDemuxFilter $1 0x10 0x0 0x0 0x0
   writeRxDemuxFilter $1 0x20 0x0 0x0 0x0
}

setRxDemuxDefault () {
	writeRxDemuxFilter $1 0x00 0x010190E2 0x02005E01 0x000000FF
	writeRxDemuxFilter $1 0x10 0x080A0020 0xBA68ABEA 0x000000FF
	writeRxDemuxFilter $1 0x20 0xFFFFFFFF 0xFFFFFFFF 0x00000000
}

readMultiRegs () {
number=0
while [ "$number" -lt "$2" ]
do
   readDevmem $1 $((0x04 * $number))
        number=`expr $number + 1 `
done
}

readRxDemuxStats () {
	readMultiRegs 0x83C50200 10
}

showRxDemuxStats () {
    l1=$(devmem $(($1 + 0x0)) 32)
    l2=$(devmem $(($1 + 0x4)) 32)
    l3=$(devmem $(($1 + 0x8)) 32)
    l4=$(devmem $(($1 + 0xC)) 32)
    printf "RXDEMUX READ : MR=%8d AR=%8d BM=%8d BA=%8d\n" $l1
}
