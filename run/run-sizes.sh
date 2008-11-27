
BASE_DIR=$(dirname $0)/..
BIN=$BASE_DIR/bin
RUN=$BASE_DIR/run
	
source $RUN/run.common 

parse_args $*

CPUS=$ARG_CPUS
VERSIONS=$ARG_VERSIONS
SIZES=$ARG_INPUTS

exec_all_sizes

