# SDOS API
This file contains all suggested HTTP requests
and some examples using the provided libraries as well.

### Data format
In general, all data is transferred using JSON (JavaScript Object Notation). 
Therefore you may transfer lists (arrays), integers, floats, strings and nested objects. 
You cannot add any additional, custom datatypes. 
If you want to use anything else (e.g. Python tuples or timezone aware java timestamps), it has to be converted to a string and parsed.

### Required parameters
Whether you want to persist or query data, you have to specify the containing collection.
Think of it as the directory containing all the files you want to query.
The terminology is borrowed from document storage databases, such as MongoDB.

* Collection name `collection` either as URL param or as part of the json body.
If it is part of the json body, it will be treated as a part of the data itself and redundantly persisted.
Beware of naming collisions, if your system logic also uses the attribute `collection`.
If both the URL parameter an the json attribute are present, the URL param takes precedence.

### Persisting

If you want to persist data, it has to be within the HTTP requests body as a string in JSON format.
The header has to specify the `content-type` to be `application\json`. 
The request method has to be `POST`.
After persisting the data successfully, the response will be the data's generated `UUID` string.
By default it has to be sent to `localhost:5006`.
If the collection does not exist yet, it will be automatically created.

This exemplary accepted JSON could be posted to `localhost:5006?collection=demo`:

    {
        "demo_number": 42,
        "demo_array": [1.2, 3.4, 5.6],
        "demo_string": "content"
    }

And the returned ID could be:

`c5139ab1-ec1d-4b2e-b9fa-a46caf1e080e` 

### Updating

Similarly to persisting data, you can update an existing entry by also providing `data_id`either as request param or as part of the json body.

### Querying

TODO: searching / filtering data

You may query either a list of entry IDs within a collection, or a specific entry with all its data.
In both cases, send a `GET` request to `localhost:5006`. 
When providing `data_id`, either as request param or as part of the json body, the endpoint will return the entry's data as JSON.
Otherwise, the list of IDs within the collection will be returned.

For example, when sending a `GET` request to `localhost:5006?collection=demo`, the returned IDs could be:

`{"ids": ["c5139ab1-ec1d-4b2e-b9fa-a46caf1e080e", "99797d68-ce6b-45bf-90ce-fa8c5979a8cd"]}`

And querying `localhost:5006?collection=demo&data_id=c5139ab1-ec1d-4b2e-b9fa-a46caf1e080e` yields:
    
    {
        "demo_number": 42,
        "demo_array": [1.2, 3.4, 5.6],
        "demo_string": "content"
    }

### Using the C library "sdos"

Since sending HTTP requests using C can be tedious, we also provide a library, which offers functions for the most common API calls.
The following code snippet illustrates its usage for both writing and reading data.
    
    #include <sdos.h>
    #include <stdio.h>
    
    int main() {
        write_int("demo_value", 69);
        int demo_arr[] = {1, 2, 3, 4, 5};
        write_int_array("demo_arr", 5, demo_arr);
        
        int id_length;
        char* id;
        persist_entry("writing_demo", &id, &id_length);
        
        int n_items;
        char **items;
        collection_entries("writing_demo", &n_items, &items);
        for (int i = 0; i < n_items; ++i) {
            load_entry("writing_demo", items[i]);
            printf("%sn", items[i]);
        }
        return 0;
    }

A call to the `load_entry` function, queries the given entry id and keeps a copy in memory.

Given the following struct (from our research group's mapping algorithm)

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
    
The following code illustrates how it could be populated using sdos

    load_entry("collection_name", data_id /*populate with your id*/);
    
    struct input in;
    extract_int_array("dims", &(in.n_dims), &(in.dims));
    extract_int_array("stencil", &(in.stencil_size), &(in.stencil));
    extract_int_array("partition_sizes", &(in.num_part), &(in.part_sizes));
    in.stencil_size /= in.n_dims;
    in.num_nodes = extract_int("num_nodes");
    in.node_size = extract_int("node_size");

(Who would have thought that an array could be read without having to explicitly specify its length first.
Besides Python, C++, Java and all other languages other than C, of course.)