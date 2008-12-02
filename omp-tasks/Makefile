DIRS=fib alignment nqueens sort strassen sparselu fft knapsack concom floorplan 

RECURSIVE=all-recursive clean-recursive dist-clean-recursive

all: all-recursive
clean: clean-recursive
dist-clean: dist-clean-recursive

$(RECURSIVE): 
	@failcom='exit 1';\
        target=`echo $@ | sed s/-recursive//`; \
        for subdir in $(DIRS); do \
                (cd $$subdir && $(MAKE) $(AM_MAKEFLAGS) $$target) \
          || eval $$failcom; \
        done;

dist: dist-clean
	echo "TODO"

