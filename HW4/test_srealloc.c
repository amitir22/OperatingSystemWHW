#include <cstdio>

#include <assert.h>



#define META_SIZE         sizeof(meta_data) // put your meta data name here.

#include "smalloc_3.cpp"



int main() {



    //   global_list_init = NULL; init of global list.

    //   global_list = NULL;



    // allocate blocks

    void *b1, *b2, *b3, *b4, *b5, *b6, *b7, *b8, *b9, *b10, *b11, *b12, *b13, *b14 ,*b15;



    // allocate first block, heap: b1(1000)

    b1 = smalloc(1000);

    assert(_num_free_blocks() == 0);

    assert(_num_free_bytes() == 0);

    assert(_num_allocated_blocks() == 1);

    assert(_num_allocated_bytes() == 1000);

    assert(_num_meta_data_bytes() == META_SIZE);



    // check srealloc with NULL, heap: 1000(no_sfree), 2000(not_sfree)

    b2 = srealloc(NULL, 2000);

    assert(_num_free_blocks() == 0);

    assert(_num_free_bytes() == 0);

    assert(_num_allocated_blocks() == 2);

    assert(_num_allocated_bytes() == 3000);

    assert(_num_meta_data_bytes() == 2 * META_SIZE);



    // check srealloc with same size, should return the same block, heap: 1000(no_sfree), 2000(not_sfree)

    b3 = srealloc(b2, 2000);

    assert(b2 == b3);

    assert(_num_free_blocks() == 0);

    assert(_num_free_bytes() == 0);

    assert(_num_allocated_blocks() == 2);

    assert(_num_allocated_bytes() == 3000);

    assert(_num_meta_data_bytes() == 2 * META_SIZE);



    // check srealloc with bigger size on wilderness block, heap: 1000(no_sfree), 3000(not_sfree)

    b4 = srealloc(b2, 3000);

    assert(b2 == b4);

    assert(_num_free_blocks() == 0);

    assert(_num_free_bytes() == 0);

    assert(_num_allocated_blocks() == 2);

    assert(_num_allocated_bytes() == 4000);

    assert(_num_meta_data_bytes() == 2 * META_SIZE);



    // check srealloc for smaller size and not enough space for split, heap: 1000(no_sfree), 3000(not_sfree)

    b5 = srealloc(b1, 900);

    assert(b1 == b5);

    assert(_num_free_blocks() == 0);

    assert(_num_free_bytes() == 0);

    assert(_num_allocated_blocks() == 2);

    assert(_num_allocated_bytes() == 4000);

    assert(_num_meta_data_bytes() == 2 * META_SIZE);



    // heap: 1000(no_sfree), 3000(not_sfree), 2000(not_sfree)

    b6 = smalloc(2000);

    assert(_num_free_blocks() == 0);

    assert(_num_free_bytes() == 0);

    assert(_num_allocated_blocks() == 3);

    assert(_num_allocated_bytes() == 6000);

    assert(_num_meta_data_bytes() == 3 * META_SIZE);

    

    // check srealloc for smaller size and enough space for split

    // heap: 1000(no_sfree), 1000(not_sfree), 2000 - META_SIZE(sfree), 2000(not_sfree)

    b7 = srealloc(b2, 1000);

    assert(_num_free_blocks() == 1);

    assert(_num_free_bytes() == 2000 - META_SIZE);

    assert(_num_allocated_blocks() == 4);

    assert(_num_allocated_bytes() == 6000 - META_SIZE);

    assert(_num_meta_data_bytes() == 4 * META_SIZE);

/*

    // check srealloc for smaller size and enough space for split, after that merge with upper sfree block

    // heap: 1000(no_sfree), 500(not_sfree), 2500 - META_SIZE(sfree), 2000(not_sfree)

    b8 = srealloc(b7, 500);

    assert(_num_free_blocks() == 1);

    assert(_num_free_bytes() == 2500 - META_SIZE);

    assert(_num_allocated_blocks() == 4);

    assert(_num_allocated_bytes() == 6000 - META_SIZE);

    assert(_num_meta_data_bytes() == 4 * META_SIZE);



    // check srealloc for bigger size, upper block is sfree and enough space for split

    // heap: 1000(no_sfree), 1000(not_sfree), 2000 - META_SIZE(sfree), 2000(not_sfree)

    b9 = srealloc(b8, 1000);

    assert(_num_free_blocks() == 1);

    assert(_num_free_bytes() == 2000 - META_SIZE);

    assert(_num_allocated_blocks() == 4);

    assert(_num_allocated_bytes() == 6000 - META_SIZE);

    assert(_num_meta_data_bytes() == 4 * META_SIZE);



    // check srealloc for bigger size, upper block is sfree and not enough space for split

    // heap: 1000(no_sfree), 3000(not_sfree), 2000(not_sfree)

    b10 = srealloc(b9, 3000);

    assert(b10 == b9);

    assert(_num_free_blocks() == 0);

    assert(_num_free_bytes() == 0);

    assert(_num_allocated_blocks() == 3);

    assert(_num_allocated_bytes() == 6000);

    assert(_num_meta_data_bytes() == 3 * META_SIZE);



    // check srealloc for bigger size, upper block is sfree and lower block is sfree and size requested fits the 3 blocks merged

    // heap: 1000(no_sfree), 1000(not_sfree), 2000 - META_SIZE(sfree), 2000(not_sfree)

    b11 = srealloc(b10, 1000);

    assert(_num_free_blocks() == 1);

    assert(_num_free_bytes() == 2000 - META_SIZE);

    assert(_num_allocated_blocks() == 4);

    assert(_num_allocated_bytes() == 6000 - META_SIZE);

    assert(_num_meta_data_bytes() == 4 * META_SIZE);



    // heap: 1000(sfree), 1000(not_sfree), 2000 - META_SIZE(sfree), 2000(not_sfree)

    sfree(b1);

    assert(_num_free_blocks() == 2);

    assert(_num_free_bytes() == 3000 - META_SIZE);

    assert(_num_allocated_blocks() == 4);

    assert(_num_allocated_bytes() == 6000 - META_SIZE);

    assert(_num_meta_data_bytes() == 4 * META_SIZE);



    // heap: 4000 + META_SIZE(not_sfree), 2000(not_sfree)

    b12 = srealloc(b10, 4000);

    assert(b12 == b1);

    assert(_num_free_blocks() == 0);

    assert(_num_free_bytes() == 0);

    assert(_num_allocated_blocks() == 2);

    assert(_num_allocated_bytes() == 6000 + META_SIZE);

    assert(_num_meta_data_bytes() == 2 * META_SIZE);

    */

    printf("GOOD JOB!\n");



    return 0;

}