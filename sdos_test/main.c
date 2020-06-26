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

    write_int("demo_value", 69);
    int demo_arr[] = {1, 2, 3, 4, 5};
    write_int_array("demo_arr", 5, demo_arr);

    int id_length;
    char* id;
    persist_entry("writing_demo", true, &id, &id_length);

    collection_entries("writing_demo", &n_items, &items);

    for (int i = 0; i < n_items; ++i) {
        load_entry("writing_demo", items[i], true);
        printf("%s\n", items[i]);
    }

    return 0;
}

int main_old() {
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
