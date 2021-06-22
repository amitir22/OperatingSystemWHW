all: hw1 hw2 hw3 hw4

hw1:
	$(MAKE) -C HW1/

hw2:
	$(MAKE) -C HW2/

hw3:
	$(MAKE) -C HW3/

hw4:
	$(MAKE) -C HW4/

clean:
	$(MAKE) -C HW1/ clean
	$(MAKE) -C HW2/ clean
	$(MAKE) -C HW3/ clean
	$(MAKE) -C HW4/ clean
