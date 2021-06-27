
int main() {
    size_t s = 128*1024+1;
    void *p1, *p2, *p3, *p4;

    p1 = smalloc(s);

    std::cout << _num_allocated_blocks() << " " << _num_allocated_bytes() << " " << _num_free_blocks() << " " << _num_free_bytes() << " " << _num_meta_data_bytes() << std::endl;

    sfree(p1);

    std::cout << _num_allocated_blocks() << " " << _num_allocated_bytes() << " " << _num_free_blocks() << " " << _num_free_bytes() << " " << _num_meta_data_bytes() << std::endl;

    p2 = smalloc(s);

    std::cout << _num_allocated_blocks() << " " << _num_allocated_bytes() << " " << _num_free_blocks() << " " << _num_free_bytes() << " " << _num_meta_data_bytes() << std::endl;

    p3 = smalloc(s);

    std::cout << _num_allocated_blocks() << " " << _num_allocated_bytes() << " " << _num_free_blocks() << " " << _num_free_bytes() << " " << _num_meta_data_bytes() << std::endl;

    p4 = smalloc(s);

    std::cout << _num_allocated_blocks() << " " << _num_allocated_bytes() << " " << _num_free_blocks() << " " << _num_free_bytes() << " " << _num_meta_data_bytes() << std::endl;

    sfree(p3);

    std::cout << _num_allocated_blocks() << " " << _num_allocated_bytes() << " " << _num_free_blocks() << " " << _num_free_bytes() << " " << _num_meta_data_bytes() << std::endl;
}