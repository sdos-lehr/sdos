#ifndef SDOS_LIB_LIBRARY_H
#define SDOS_LIB_LIBRARY_H

#include <stdbool.h>
#include <json-c/json.h>

/**
 * Queries the ids contained in the given collection
 * @param collection: IN name of the collection to query
 * @param n_items: OUT number of ids within the collection
 * @param ids: OUT list of strings containing all the ids within the collection
 * @return status code TODO
 */
int collection_entries(const char *collection, int *n_items, char ***ids);

/**
 * TODO same as collection_entries(...), but instead of explicitly passing the collection name,
 * it is read from the environment variables
 */
int collection_entries_from_environment(int *n_items, char ***ids);

/**
 * Query a single entry and cache it for global access
 * @param collection name
 * @param id of the element to load
 * @param print_debug: set to true if the received GET response should be printed
 * @return a handle to the received json_object, which may be used optionally
 */
json_object *load_entry(const char *collection, const char *id, bool print_debug);

/**
 * TODO same as load_entry(...), but reads all parameters from the environment variables
 */
json_object *load_entry_from_environment();

/**
 * Persisted the previously modified json object
 * @param collection name
 * @param print_debug: set to true if the received GET response should be printed
 * @return a handle to the received json_object, which may be used optionally
 */
void persist_entry(const char *collection, bool print_debug, char **generated_id, int *id_length);

// ################################################
// # From here on, the data access methods follow #
// # Load an entry e.g. with load_entry(...) first#
// ################################################

int extract_int(const char *param);

void extract_int_array(const char *param, int *arr_length, int **arr);

int64_t extract_long(const char *param);

void extract_long_array(const char *param, int *arr_length, int64_t **arr);

double extract_double(const char *param);

void extract_double_array(const char *param, int *arr_length, double **arr);

char *extract_string(const char *param);

void extract_string_array(const char *param, int *arr_length, char ***arr);


// ###############################################
// # From here on, the data write methods follow #
// ###############################################

void write_int(const char *param, int value);
void write_int_array(const char *param, int arr_length, int *arr);


#endif //SDOS_LIB_LIBRARY_H
