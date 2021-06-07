all: HW1 HW2 HW3

HW1:
	$(MAKE) -C HW1/

HW2:
	$(MAKE) -C HW2/

HW3:
	$(MAKE) -C HW3/

clean:
	$(MAKE) -C HW1/ clean
	$(MAKE) -C HW2/ clean
	$(MAKE) -C HW3/ clean