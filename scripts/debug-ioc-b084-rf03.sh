#!/usr/bin/env bash

TOP="$(dirname "${BASH_SOURCE[0]}")"

export PATH="/afs/slac/package/rtems/4.10.2/target/rtems_p4/ssrlApps_p3/powerpc-rtems/mvme3100/bin:$TOP/../bin/RTEMS-mvme3100:$PATH"

echo "Running gdb..."
powerpc-rtems-gdb -ex 'target rtems-remote ioc-b084-rf03:2159'
