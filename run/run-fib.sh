#defaults

DEF_INPUTS=10

#don't modify from here

BASE_DIR=$(dirname $0)/..
BIN=$BASE_DIR/bin
RUN=$BASE_DIR/run
	
source $RUN/run.common 

parse_args $*
set_values
exec_all_sizes

