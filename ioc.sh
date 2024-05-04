#!/usr/bin/env bash

cd "$(dirname "${BASH_SOURCE[0]}")"

cd iocBoot/sioc-tst-sys0

mkdir -p autosave
$DEBUGGER ../../bin/$EPICS_HOST_ARCH/atlas ./st.cmd

