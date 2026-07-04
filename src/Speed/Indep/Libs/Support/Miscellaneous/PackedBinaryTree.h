template<typename T, typename U>
unsigned int SearchPackedBinaryTree(unsigned int count, T *table, U key) {
    unsigned int index = 0;

    while (true) {
        if (index >= count) {
            return index;
        }

        if (table[index] != key){
            unsigned int branch = (table[index] < key) + 1;
            index = 2 * index + branch;
        } else {
            return index;
        }
    }
}
