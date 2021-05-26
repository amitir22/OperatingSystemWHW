#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/list.h>


asmlinkage long sys_hello(void) {
    printk("Hello, World!\n");
    return 0;
}

asmlinkage long sys_set_weight(int weight) {
    if (weight >= 0) {
        printk("Weight before: %d\n", current->weight);
        current->weight = weight;
        printk("Set weight %d of current process\n", current->weight);

        return 0;
    } else {
        return -EINVAL;
    }
}

asmlinkage long sys_get_weight(void) {
    printk("Weight: %d\n", current->weight);
    return current->weight;
}

static long do_sys_get_children_sum(struct task_struct *father) {
    long weight = 0;
    struct list_head *pos;
    struct task_struct *task;

    printk("get_children_sum was called\n");

    list_for_each(pos, &father->children)
    {
        task = list_entry(pos, struct task_struct, sibling);
        weight += do_sys_get_children_sum(task) + task->weight;
    }

    printk("get_children_sum returns: %d\n", weight);

    return weight;
}

asmlinkage long sys_get_children_sum(void) {
    printk("get_children_sum was called\n");

    if (list_empty(&(current->children))) {
        printk("get_children_sum: failed: current->children list is empty\n");
        return -ECHILD;
    }

    return do_sys_get_children_sum(current);
}

asmlinkage long sys_get_heaviest_ancestor(void) {
    const long init_pid = 1;

    struct task_struct *current_task = current;
    long max_weight = current_task->weight;
    long max_pid = current_task->pid;
    short did_not_find_init_process = 1;

    long current_weight;

    while (did_not_find_init_process) {
        printk("get_heaviest_ancestor: current max pid: %d\n", max_pid);

        current_task = current_task->real_parent; // upward traversal in the process tree
        current_weight = current_task->weight; // for readability

        if (current_weight > max_weight) {
            printk("get_heaviest_ancestor: previous max_weight: %d\n", max_weight);
            max_weight = current_weight;
            max_pid = current_task->pid;
            printk("get_heaviest_ancestor: current max_weight: %d\n", max_weight);
        }

        // because init is the father of all processes after boot is completed
        if (current_task->pid == init_pid) {
            printk("get_heaviest_ancestor: prime ancestor process found (init process)\n");
            did_not_find_init_process = 0; // init process found
        }
    }

    printk("get_heaviest_ancestor: heaviest process' pid: %d\n", max_pid);

    return max_pid;
}
