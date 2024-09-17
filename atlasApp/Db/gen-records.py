#!/usr/bin/env python3

import argparse

parser = argparse.ArgumentParser()  
parser.add_argument('-o', type=str, required=True)
parser.add_argument('-n', type=int, required=True)
args = parser.parse_args()

templ = '''
record(longin, "$(P)Random$$NUM$$")
{
        field(VAL, "2")
        field(DTYP, "devLongRandom")
        field(SCAN, "1 second")
        field(INP, "@$$HIGH$$,$$LOW$$")
        info(autosaveFields, "VAL EGU DESC")
}
'''

with open(args.o, 'w') as fp:
    for i in range(0, args.n):
        fp.write(templ.replace('$$NUM$$', str(i)).replace('$$HIGH$$', '100').replace('$$LOW$$', '-100'))

with open('info_positions.req', 'w') as fp:
    for i in range(0, args.n):
        fp.write(f'$(P)Random{i}\n')

