#include <library.h>
#include <stdio.h>

struct input {
    int n_dims;
    int* dims;
    int stencil_size;
    int* stencil;
    int num_part;
    int* part_sizes;
    int num_nodes;
    int node_size;
};

int main() {
    int n_items;
    char **items;
    collection_entries("mapping_instances", &n_items, &items);

    for (int i = 0; i < n_items; ++i) {
        load_entry("mapping_instances", items[i], true);
        printf("%s\n", items[i]);

        struct input in;
        extract_int_array("dims", &(in.n_dims), &(in.dims));
        extract_int_array("stencil", &(in.stencil_size), &(in.stencil));
        extract_int_array("partition_sizes", &(in.num_part), &(in.part_sizes));
        in.stencil_size /= in.n_dims;
        in.num_nodes = extract_int("num_nodes");
        in.node_size = extract_int("node_size");
        break;
    }

    return 0;
}
