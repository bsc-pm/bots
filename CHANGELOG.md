# BOTS ChangeLog

## _BOTS current version_ (unreleased)

### The following new features/information were added:

 * Strassen's OmpSs initial version (#147)
 * UTS's OmpSs initial version (#148)
 * N-Queens's OmpSs initial version (#144)
 * Health's OmpSs initial version (#142)
 * Multisort's OmpSs initial version (#145)
 * Floorplan's OmpSs initial version (#141)
 * Fibonacci's OmpSs initial version (#140)
 * OmpSs support (enabling to compile OmpSs applications) (#137)

### In addition, the following items were fixed and/or added:

 * Distribution files: CHANGELOG, README fixed (as markdown)
 * Script configure syntax error when using --help
 * UTS compilation problem, no 'omp.h' header
 * Common 'final' version compilation problem

## _BOTS version 1.1.2_ (released 2011-11-23)

### The following new features/information were added:

 * Choose output format option from run scripts (#132)
 * New Health's large and test inputs (#123)
 * Tied version for UTS benchmark (#131)

### In addition, the following items were fixed and/or added:

 * Health results in unsuccessful when compiled with -m 32 (#124)
 * UTS benchmark seg faults with medium data set (#128)
 * Fibonacci result overflows when input is bigger than 47 (#130)
 * Fibonacci was always returning successful (also when failing) (#129)
 * Wrong use of arrays in alignment benchmark (#126 an #127)
 * String overflows problem in common files (#122)

## _BOTS version 1.1.1_ (released 2011-2-11)

### The following new features/information were added:

 * Several benchmarks using the final OpenMP 3.1 final clause (#96)
 * Alignment benchmark has been adapted to clustalw implementation (#119)
 * get_architecture() has been implemented in all Linux-like systems (#44)
 * configure.sh script allows to specify compiler in the command line (#90)
 * A new version with single has been implemented for alignment benchmark (#63)
 * Verbosity in "run-health.sh -check" has been reduced (#112)

### In addition, the following items were fixed and/or added:

 * Script run-nqueens.sh no-shell bug fixed (#115)
 * sparselu: divison by zero in checkmat() fixed (#118)
 * CFLAGS not big enough bug has been fixed (#117)
 * Benchmark omp-tasks/nqueens.c out-of-bounds access bug has been fixed (#116)
 * UTS verification bug fixed (#113)
 * Script run-all.sh is not running nonexistent knapsack test anymore (#111)
 * Sort benchmark when seqmerge() is running for just one element bug fixed(#65)
 * All warnings has been removed from compilation (#114)

## _BOTS version 1.1_ (released 2010-5-17)

### The following new features/information were added:

 * A new benchmark has been added: Unbalanceed Tree Search (UTS) (#3, #98, #99, #102, #103, #105 & #106)
 * A README file has been added (#55)
 * Environment variables are allowed through running scripts (#109)
 * Task/Sec measurement variable has been changed by Nodes/Sec (#97)
 * Final clause versions have been added (#94)
 * Make incorpores a new wipe option which clean binnaries (#93)
 * Bots configure script has been improved (#92)
 * Improving Alignment parallel code, moving task generation inside 'if' stament (#62)
 * Additional argument specification in suite files (#57)
 * An abrigded list format has been included (#42)
 * Fibonacci uses pre-computed values for testing purposes (#24)

### In addition, the following items were fixed and/or added:

 * Verbose modes have been reviewed and synchronized with configure '--debug' option (#79, #104, #107 & #108)
 * Common compiler warning have been solved (using icc #100 and xlc #101 compilers)
 * Program name was not appearing in AIX machines (#76)
 * Package sources permissions has been changed (#71)
 * Input file reading sanity checking improvement (#91)
 * Run-script cutoff value int tied versions has been fixed (#86)
 * Message error when running outside a cpuset has been solved (#85)
 * Using a relative input file name in run-scripts (#83)
 * Support to associate specific parameters to different classes (#61)
 * Serial version only run once with multiple classes/inputs (#58)
 * Alignment non valid input file error has been solved (#82)
 * FFT checking problems has been solved (#66)
 * Fibonacci pre-computed values error has been solved (#95)
 * Fibonacci self-verification showed N/A when unsuccessful (#77)
 * Floorplan task count has been fixed (#87)
 * Cropped area in Floorplan benchmark has been fixed (#67)
 * NQueens serial execution returned a wrong result (#89)
 * Removing NQueens unneeded taskwaits (#88)
 * Avoiding to allocate temporary version in NQueens serial path (#88)
 * Sort algorihtm has been parametrized (#81)
 * Sort was not scrambling vector before ordering it (#64)
 * Sort constants have been changed (#70)
 * SparseLU checking were too strict and has been relaxed (#59)

## _BOTS version 1.0_ (released 2009-5-29)
