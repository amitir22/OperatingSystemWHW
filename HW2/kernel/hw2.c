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

//// TODO: delete
//static long do_sys_get_total_weight(struct task_struct *father) {
//    long weight = father->weight;
//    struct list_head *pos;
//    list_for_each(pos, &father->children)
//    {
//        struct task_struct *task = list_entry(pos,
//        struct task_struct, sibling);
//        weight += do_sys_get_total_weight(task);
//    }
//
//    return weight;
//}
//
//
//// TODO: delete
//asmlinkage long sys_get_total_weight(void) {
//    return do_sys_get_total_weight(current);
//}
//
//// TODO: delete
//asmlinkage long sys_get_heaviest_child(void) {
//    int max = -1;
//    int maxPid = 0;
//
//    struct list_head *pos;
//    list_for_each(pos, &current->children)
//    {
//        struct task_struct *task = list_entry(pos,
//        struct task_struct, sibling);
//
//        if (do_sys_get_total_weight(task) > max) {
//            max = do_sys_get_total_weight(task);
//            maxPid = task->pid;
//        }
//    }
//
//    if (max == -1) {
//        return -ECHILD;
//    } else {
//        return maxPid;
//    }
//}

// TODO: test and check printk()'s are needed
asmlinkage long sys_get_weight(void) {
    return current->weight;
}

// TODO: test dry + wet
static long do_sys_get_children_sum(struct task_struct *father) {
    long weight = 0;
    struct list_head *pos;
    struct task_struct *task;

    list_for_each(pos, &father->children)
    {
        task = list_entry(pos, struct task_struct, sibling);
        weight += do_sys_get_children_sum(task) + task->weight;
    }

    return weight;
}

// TODO: test
asmlinkage long sys_get_children_sum(void) {
    // todo: make sure this is the right way to test
    if (list_empty(&(current->children))) {
        return -ECHILD;
    }

    return do_sys_get_children_sum(current);
}

// TODO: test
asmlinkage long sys_get_heaviest_ancestor(void) {
    const long init_pid = 1;

    struct task_struct *current_task = current;
    long max_weight = current_task->weight;
    long max_pid = current_task->pid;
    short is_over = 0;

    long current_weight;

    while (!is_over) {
        current_task = current_task->real_parent;
        current_weight = current_task->weight;

        if (current_weight > max_weight) {
            max_weight = current_weight;
            max_pid = current_task->pid;
        }

        // TODO: maybe should be 0?
        if (current_task->pid == init_pid) {
            is_over = 1;
        }
    }

    return max_pid;
}
