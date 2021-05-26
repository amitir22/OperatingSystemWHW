#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <math.h>
#include <sys/prctl.h>

#define RESET -800
#define MAX_RANDOM_NUM 50

//BE CAREFUL WITH THESE CUTE ONES BELOW, forked processes amount would
//grow exponentially by WANTED_FORKS^WANTED_GENERATIONS!
#define WANTED_FORKS 4
#define WANTED_GENERATIONS 2
#define WANTED_ANCESTORS 8

//consider to raise this number in case of higher fork and generations 
//value and TEST 11 fails with rather large positive number:

#define MAX_ALLOWED_TIME 2



void createFamily(pid_t* fork_array,int generation, int* requested_weights, int last_location){
	if(generation<1){
		return; 
	}
	
	for(int i=0; i<WANTED_FORKS;i++)
	{
		pid_t child=fork();
		if(child<0){
			std::cout << "10.5 FAIL: fork failed, too much forks dearie" << std::endl;
			exit(1);
		}
		if(child==0){
			prctl(PR_SET_PDEATHSIG, SIGHUP);
			int weight_location=0;
			if(generation==WANTED_GENERATIONS){
				weight_location=i * ( (pow(WANTED_FORKS,WANTED_GENERATIONS) - 1) / (WANTED_FORKS-1));
			}
			else
			{
				weight_location=last_location + 1 +  (i * ( (pow(WANTED_FORKS,generation) - 1) / (WANTED_FORKS-1)));
			}
			//std::cout<< "my location is:" << weight_location << std::endl;
			syscall(334,requested_weights[weight_location]);
			generation-=1;
			//std::cout<< "my generation is:" << generation << std::endl;
			createFamily(nullptr,generation,requested_weights,weight_location);
			while(1);
			exit(0);
		}
		if(fork_array!=nullptr){
			fork_array[i]=child;
		}
	}
}

void createDynasty(int generation, int* requested_weights, int expected_max_weight_place, int *ancestors_pids){
	if(generation<1){
		return;
	}
	int wstatus;
	pid_t child=fork();
	if(child<0){
		std::cout << "12.5 FAIL: fork failed, too much forks dearie" << std::endl;
		exit(1);
	}
	if(child==0){
			prctl(PR_SET_PDEATHSIG, SIGHUP);
			syscall(334,requested_weights[generation-1]);
			ancestors_pids[generation-1]=getpid();
			if(generation>1){
				generation-=1;
				createDynasty(generation,requested_weights,expected_max_weight_place,ancestors_pids);
			}
			else{
				long r = syscall(337);
				pid_t expected_pid=ancestors_pids[expected_max_weight_place];
				if(r==expected_pid){ std::cout << "13 SUCCESS: sys_get_heaviest_ancestor successfully returned with: "<< r << std::endl; }
				else{
					std::cout << "13 FAIL: sys_get_heaviest_ancestor didn't return the expected weight value. current value is: " << r << std::endl;
					std::cout << "expected_max_weight_place: " << expected_max_weight_place << std::endl;
					for(int i=0;i<WANTED_ANCESTORS;i++){
						std::cout << "place: "<< i << " value: " << ancestors_pids[i] << " weight: " << requested_weights[i] << std::endl;
					}
				}
				exit(0);
			}
			wait(&wstatus);
			exit(0);
	}
	wait(&wstatus);
}

int get_rand(){
	static int seed=4654531;
	srand((unsigned int)time(NULL) + ++seed);
	return 1 + (rand()%MAX_RANDOM_NUM);
	
}

void basicTest(){
	long r=RESET;
	// TEST get_weight default value
	r = syscall(333);
	if(r==0){ std::cout << "1 SUCCESS: sys_get_weight default value is correct and is: " << r << std::endl; }
	else{ std::cout << "1 FAIL: sys_get_weight should not fail! r value is:" << r << std::endl; }
	
	
	// TEST set_weight error value
	
	r = syscall(334,-5);
	if(errno==EINVAL){ std::cout << "2 SUCCESS: sys_set_weight failed successfully." << std::endl; }
	else{ std::cout << "2 FAIL: sys_set_weight didn't return the correct failed value or didn't fail successfully "<< std::endl; }
	
	
	// Test set_weight valid value
	r=RESET;
	r = syscall(334,8);
	if(r<0){std::cout << "2.5 FAIL: sys_set_weight with positive value shouldn't fail! returned value is: " << r << std::endl;}
	r=RESET;
	r = syscall(335);
	if(r==8){ std::cout << "3 SUCCESS: sys_set_weight successfully returned with: "<< r << std::endl; }
	else{ std::cout << "3 FAIL: sys_get_weight didn't return the expected weight value. current value is: " << r << std::endl; }
}

void familyTest(){
	int wstatus;
	long r=RESET;
	// Test get_children_sum with no kids
	r=RESET;
	r = syscall(336);
	if(errno==ECHILD){ std::cout << "4 SUCCESS: sys_get_children_sum failed successfully." << std::endl; }
	else{ std::cout << "4 FAIL: sys_get_children_sum didn't return the correct failed value or didn't fail successfully "<< std::endl; }
	
	
	// Test get_children_sum with kids
	pid_t child=fork();
	if(child<0){
		std::cout << "4.5 FAIL: fork failed" << std::endl;
		exit(1);
	}
	if(child==0){//child process
		//Test if child inherit parent's wieght
		r=RESET;
		r = syscall(335);
		if(r==8){ std::cout << "5 SUCCESS: child process inherited parent's weight. successfully returned with: "<< r << std::endl; }
		else{ std::cout << "5 FAIL: child weight isn't equal parent's. current value is: " << r << std::endl; }
		r = syscall(334,4);
		if(r<0){ std::cout << "5.5 FAIL: sys_set_weight for child with positive value shoudn't fail! returned value is: " << r << std::endl; }
		
		r=RESET;
		r = syscall(336);
		if(errno==ECHILD){ std::cout << "6 SUCCESS: sys_get_children_sum failed successfully." << std::endl; }
		else{ std::cout << "6 FAIL: sys_get_children_sum didn't return the correct failed value or didn't fail successfully "<< std::endl; }
	
		while(1);
		exit(0);
	}
	
	//Test if get_children_sum works
	sleep(MAX_ALLOWED_TIME);
	r=RESET;
	r = syscall(336);
	if(r==4){ std::cout << "7 SUCCESS: sys_get_children_sum successfully return with: " << r << std::endl; }
	else{ std::cout << "7 FAIL: sys_get_children_sum didn't return the correct value. returned value is: " << r << std::endl; }
	kill(child,9);
	wait(&wstatus);
	// Test if get_children_sum work after child died:
	r = syscall(336);
	if(errno==ECHILD){ std::cout << "8 SUCCESS: sys_get_children_sum failed successfully." << std::endl; }
	else{ std::cout << "8 FAIL: sys_get_children_sum didn't return the correct failed value or didn't fail successfully "<< std::endl; }
	
	///FAMILY ONE GENERATION TIME///
	
	
	pid_t fork_array[WANTED_FORKS] = {};
	int expected_sum=0;
	for(int i=0; i<WANTED_FORKS;i++)
	{
		int new_weight=get_rand();
		expected_sum+=new_weight;
		child=fork();
		if(child<0){
			std::cout << "8.5 FAIL: fork failed, too much forks dearie" << std::endl;
			exit(1);
		}
		if(child==0){
			r = syscall(334,new_weight);
			while(1);
			exit(0);
		}
		fork_array[i]=child;
	}
	sleep(MAX_ALLOWED_TIME);
	r=RESET;
	r = syscall(336);
	//std::cout<< "expected value:" << expected_sum << std::endl;
	if(r==expected_sum){ std::cout << "9 SUCCESS: sys_get_children_sum successfully returned with: " << r << std::endl; }
	else{ std::cout << "9 FAIL: sys_get_children_sum didn't return the correct value. returned value is: " << r << std::endl; }
	for(int i=0;i<WANTED_FORKS;i++){
		kill(fork_array[i],9);
	}
	while(wait(NULL)!=-1);
	// Test if get_children_sum work after child died:
	r = syscall(336);
	if(errno==ECHILD){ std::cout << "10 SUCCESS: sys_get_children_sum failed successfully." << std::endl; }
	else{ std::cout << "10 FAIL: sys_get_children_sum didn't return the correct failed value or didn't fail successfully "<< std::endl; }
	
	///FAMILY MULTIPLE GENERATIONS TIME///
	
	int family_size=(WANTED_FORKS * (pow(WANTED_FORKS, WANTED_GENERATIONS) - 1)) / (WANTED_FORKS-1);
	int requested_weights[family_size] = {};
	expected_sum=0;
	//std::cout << "family size is: " << family_size<< std::endl;
	for(int i=0;i<family_size;i++){
		int new_weight=get_rand();
		requested_weights[i]=new_weight;
		expected_sum+=new_weight;
		//std::cout << "new weight is:: " << new_weight << std::endl;
	}
	createFamily(fork_array, WANTED_GENERATIONS,requested_weights,0);
	sleep(MAX_ALLOWED_TIME);
	r=RESET;
	r = syscall(336);
	//std::cout<< "expected value:" << expected_sum << std::endl;
	if(r==expected_sum){ std::cout << "11 SUCCESS: sys_get_children_sum successfully returned with: " << r << std::endl; }
	else{ std::cout << "11 FAIL: sys_get_children_sum didn't return the correct value. returned value is: " << r << std::endl; }
	for(int i=0;i<WANTED_FORKS;i++){
		kill(fork_array[i],9);
	}
	while(wait(NULL)!=-1);
	// Test if get_children_sum work after child died:
	r = syscall(336);
	if(errno==ECHILD){ std::cout << "12 SUCCESS: sys_get_children_sum failed successfully." << std::endl; }
	else{ std::cout << "12 FAIL: sys_get_children_sum didn't return the correct failed value or didn't fail successfully "<< std::endl; }
}

void dynastyTest(){
	long r=RESET;
		///ANCESTORS TIME///
	
	r = RESET;
	r = syscall(334,0);
	
	int ancestor_requested_weights[WANTED_ANCESTORS] = {};
	int expected_max_weight=0;
	int expected_max_weight_place=0;
	for(int i=0;i<WANTED_ANCESTORS;i++){
		int new_weight=get_rand();
		if(new_weight>=expected_max_weight){
			expected_max_weight = new_weight;
			expected_max_weight_place=i;
		}
		ancestor_requested_weights[i]=new_weight;
	}
	int ancestors_pids[WANTED_ANCESTORS] = {};
	//std::cout << "expected max weight is:: " << expected_max_weight << std::endl;
	createDynasty(WANTED_ANCESTORS, ancestor_requested_weights, expected_max_weight_place,ancestors_pids);
}

int main()
{
	basicTest();
	familyTest(); //IF YOU DECIDE TO ITERATRE FAMILY AND DYNASTY TESTS, IGNORE TEST 5 FAIL RESULT FROM THE SECOND TIME
	dynastyTest();
	
	return 0;
}


//waitpid(child,&wstatus,WUNTRACED);

/*time(&begin_time);
	while(r!=expected_sum){
		r=RESET;
		r = syscall(336);
		time_t time_passed=time(&current_time)-begin_time;
		if(time_passed>MAX_ALLOWED_TIME){
			break;
		}
	}*/

/*
	for(int i=0;i<1000;i++){
			std::cout<<"my milkshake brings all the boys to the yard. ";
		}
*/
