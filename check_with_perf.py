#!/usr/bin/python
import math
import subprocess as sp
import sys
import os
from random import *

# Parameters

# default size
SIZE = math.pow(2, 20)

metrics = "L1-dcache-load-misses"


# Executes the given string as a shell command.
# Returns the standard output and error.
def execute_command(command):
    return sp.Popen(command, stdout=sp.PIPE, stderr=sp.PIPE).communicate()


# Returns the shell command as a string for executing perf, given the
# metrics string and executable string.
# repeats = 20


def make_perf_command(metrics, executable, repeats=20, out='perf_out.txt'):
    return ['perf', 'stat', '-r', str(repeats), '-o', out, '-e', metrics] + executable


def elapsed_time(output):
    ans = None
    lines = output.splitlines()
    for line in lines:
        idx = line.find("seconds time elapsed")
        if idx != -1:
            val = line[:idx].strip()
            try:
                ans = float(val)
            except ValueError:
                raise RuntimeError("Value %s found for seconds time elapsed." % val)
    return ans


def L1_misses(output):
    ans = None
    lines = output.splitlines()
    for line in lines:
        idx = line.find("L1-dcache-load-misses")
        if idx != -1:
            val = line[:idx].strip()
            val = val.replace(',', '')
            try:
                ans = float(val)
            except ValueError:
                raise RuntimeError("Value %s found for L1-dcache-load-misses." % val)

    return ans


# Returns the performance metrics for one run.
def run_case(sort_option, nelements):
    execute_command(['make'])
    out = 'perf_out_' + str(randint(1,10000000)) + '.txt'
    funnel_command = make_perf_command(metrics, ["./sort", str(sort_option), '-n', str(nelements)], out=out)
    print ' '.join(funnel_command)
    execute_command(funnel_command)

    with open(out, 'r') as fd:
        output = fd.read()

    os.system('rm '+out)
    return {'elapsed_time': elapsed_time(output), 'L1_misses': L1_misses(output)}


# Main script routine.
def main():
    quick_metrics = run_case("-q", SIZE)
    print "Quick sort time: " + str(quick_metrics['elapsed_time']) + " s"
    print "Quick sort L1 data cache misses: " + str(quick_metrics['L1_misses'])

    funnel_metrics = run_case("-f", SIZE)
    print "Funnel sort time: " + str(funnel_metrics['elapsed_time']) + " s"
    print "Funnel sort L1 data cache misses: " + str(funnel_metrics['L1_misses'])

    print "Funnel sort / quick sort run time ratio: " + \
          str(funnel_metrics['elapsed_time'] / quick_metrics['elapsed_time'])
    print "Funnel sort / quick sort L1 data cache miss ratio: " + \
          str(float(funnel_metrics['L1_misses']) / float(quick_metrics['L1_misses']))


if __name__ == '__main__':
    if len(sys.argv) == 2:
        try:
            SIZE = int(sys.argv[-1])
        except:
            print 'Failed to use given size. Using default size: ', SIZE
    main()

