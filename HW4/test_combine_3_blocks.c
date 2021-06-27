
/* TEST */
int main() {
    // test combine of 3 blocks
    void* p1;
    void* p2;
    void* p3;

    p1 = smalloc(100);

    std::cout << "#" << _num_allocated_blocks() << " allocated blocks of size " << _num_allocated_bytes() << std::endl;
    std::cout << "EXP: #1 allocated blocks of size 100" << std::endl;
    std::cout << "#" << _num_free_blocks() << " free blocks of size " << _num_free_bytes()<< std::endl;
    std::cout << "EXP: #0 free blocks of size 0" << std::endl;
    std::cout << "#" << _num_meta_data_bytes() << " num metadata bytes"<< std::endl;
    std::cout << "EXP: #64 metadata bytes" << std::endl;
    std::cout << std::endl;

    p2 = smalloc(100);

    std::cout << "#" << _num_allocated_blocks() << " allocated blocks of size " << _num_allocated_bytes() << std::endl;
    std::cout << "EXP: #2 allocated blocks of size 200" << std::endl;
    std::cout << "#" << _num_free_blocks() << " free blocks of size " << _num_free_bytes()<< std::endl;
    std::cout << "EXP: #0 free blocks of size 0" << std::endl;
    std::cout << "#" << _num_meta_data_bytes() << " num metadata bytes"<< std::endl;
    std::cout << "EXP: #128 metadata bytes" << std::endl;
    std::cout << std::endl;

    p3 = smalloc(100);

    std::cout << "#" << _num_allocated_blocks() << " allocated blocks of size " << _num_allocated_bytes() << std::endl;
    std::cout << "EXP: #3 allocated blocks of size 300" << std::endl;
    std::cout << "#" << _num_free_blocks() << " free blocks of size " << _num_free_bytes()<< std::endl;
    std::cout << "EXP: #0 free blocks of size 0" << std::endl;
    std::cout << "#" << _num_meta_data_bytes() << " num metadata bytes"<< std::endl;
    std::cout << "EXP: #192 metadata bytes" << std::endl;
    std::cout << std::endl;


    sfree(p1);

    std::cout << "#" << _num_allocated_blocks() << " allocated blocks of size " << _num_allocated_bytes() << std::endl;
    std::cout << "EXP: #3 allocated blocks of size 300" << std::endl;
    std::cout << "#" << _num_free_blocks() << " free blocks of size " << _num_free_bytes()<< std::endl;
    std::cout << "EXP: #1 free blocks of size 100" << std::endl;
    std::cout << "#" << _num_meta_data_bytes() << " num metadata bytes"<< std::endl;
    std::cout << "EXP: #192 metadata bytes" << std::endl;
    std::cout << std::endl;

    sfree(p3);

    std::cout << "#" << _num_allocated_blocks() << " allocated blocks of size " << _num_allocated_bytes() << std::endl;
    std::cout << "EXP: #3 allocated blocks of size 300" << std::endl;
    std::cout << "#" << _num_free_blocks() << " free blocks of size " << _num_free_bytes()<< std::endl;
    std::cout << "EXP: #2 free blocks of size 200" << std::endl;
    std::cout << "#" << _num_meta_data_bytes() << " num metadata bytes"<< std::endl;
    std::cout << "EXP: #192 metadata bytes" << std::endl;
    std::cout << std::endl;

    sfree(p2);

    std::cout << "#" << _num_allocated_blocks() << " allocated blocks of size " << _num_allocated_bytes() << std::endl;
    std::cout << "EXP: #3 allocated blocks of size 300" << std::endl;
    std::cout << "#" << _num_free_blocks() << " free blocks of size " << _num_free_bytes()<< std::endl;
    std::cout << "EXP: #1 free blocks of size 300" << std::endl;
    std::cout << "#" << _num_meta_data_bytes() << " num metadata bytes"<< std::endl;
    std::cout << "EXP: #144 metadata bytes" << std::endl;
    std::cout << std::endl;
    return 0;
}