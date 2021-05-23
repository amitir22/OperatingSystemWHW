#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/list.h>


asmlinkage long sys_hello(void) {
printk("Hello, World!\n");
return 0;
}

asmlinkage long sys_set_weight(int weight) {
if(weight >= 0){
	
printk("Weight before: %d\n", current->weight);

current->weight = weight;

printk("Set weight %d of current process\n", current->weight);

return 0;
} else {
	return -EINVAL;
	}

}


static long do_sys_get_total_weight(struct task_struct * father){
	long weight = father->weight;
	struct list_head* pos;
		list_for_each(pos, &father->children){
			struct task_struct * task = list_entry(pos, struct task_struct, sibling);
			weight += do_sys_get_total_weight(task);
	}
	
	return weight;
}


asmlinkage long sys_get_total_weight(void){
	return do_sys_get_total_weight(current);
}

asmlinkage long sys_get_heaviest_child(void){
	int max = -1;
	int maxPid = 0;
	
	struct list_head* pos;
		list_for_each(pos, &current->children){
			struct task_struct * task = list_entry(pos, struct task_struct, sibling);
			
			if(do_sys_get_total_weight(task) > max){
				max = do_sys_get_total_weight(task);
				maxPid = task->pid;
			}
	}
	
	if(max == -1){
		return -ECHILD;
		} else{
	
	return maxPid;
}
}

