#include "sdos.h"

#include <stdio.h>

/**
 * example C code using libcurl and json-c
 * to post and return a payload using
 * http://jsonplaceholder.typicode.com
 *
 * License:
 *
 * This code is licensed under MIT license
 * https://opensource.org/licenses/MIT
 *
 * Requirements:
 *
 * json-c - https://github.com/json-c/json-c
 * libcurl - http://curl.haxx.se/libcurl/c
 *
 * Build:
 *
 * cc curltest.c -lcurl -ljson-c -o curltest
 *
 * Run:
 *
 * ./curltest
 *
 */

/* standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

/* json-c (https://github.com/json-c/json-c) */
#include <json-c/json.h>

/* libcurl (http://curl.haxx.se/libcurl/c) */
#include <curl/curl.h>

json_object *cached_object = NULL;
json_object *object_to_persist = NULL;

/* holder for curl fetch */
struct curl_fetch_st {
    char *payload;
    size_t size;
};

/* callback for curl fetch */
size_t curl_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;                             /* calculate buffer size */
    struct curl_fetch_st *p = (struct curl_fetch_st *) userp;   /* cast pointer to fetch struct */

    /* expand buffer */
    p->payload = (char *) realloc(p->payload, p->size + realsize + 1);

    /* check buffer */
    if (p->payload == NULL) {
        /* this isn't good */
        fprintf(stderr, "ERROR: Failed to expand buffer in curl_callback");
        /* free buffer */
        free(p->payload);
        /* return */
        return 1;
    }

    /* copy contents to buffer */
    memcpy(&(p->payload[p->size]), contents, realsize);

    /* set new buffer size */
    p->size += realsize;

    /* ensure null termination */
    p->payload[p->size] = 0;

    /* return size */
    return realsize;
}

/* fetch and return url body via curl */
CURLcode curl_fetch_url(CURL *ch, const char *url, struct curl_fetch_st *fetch) {
    CURLcode rcode;                   /* curl result code */

    /* init payload */
    fetch->payload = (char *) calloc(1, sizeof(fetch->payload));

    /* check payload */
    if (fetch->payload == NULL) {
        /* log error */
        fprintf(stderr, "ERROR: Failed to allocate payload in curl_fetch_url");
        /* return error */
        return CURLE_FAILED_INIT;
    }

    /* init size */
    fetch->size = 0;

    /* set url to fetch */
    curl_easy_setopt(ch, CURLOPT_URL, url);

    /* set calback function */
    curl_easy_setopt(ch, CURLOPT_WRITEFUNCTION, curl_callback);

    /* pass fetch struct pointer */
    curl_easy_setopt(ch, CURLOPT_WRITEDATA, (void *) fetch);

    /* set default user agent */
    curl_easy_setopt(ch, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    /* set timeout */
    curl_easy_setopt(ch, CURLOPT_TIMEOUT, 15);

    /* enable location redirects */
    curl_easy_setopt(ch, CURLOPT_FOLLOWLOCATION, 1);

    /* set maximum allowed redirects */
    curl_easy_setopt(ch, CURLOPT_MAXREDIRS, 1);

    /* fetch the url */
    rcode = curl_easy_perform(ch);

    /* return */
    return rcode;
}

json_object *
send_request(const char *method, const char *data, bool print_debug, const char *collection, const char *id) {
    CURL *ch;                                               /* curl handle */
    CURLcode rcode;                                         /* curl result code */

    json_object *json;                                      /* json post body */
    enum json_tokener_error jerr = json_tokener_success;    /* json parse error */

    struct curl_fetch_st curl_fetch;                        /* curl fetch struct */
    struct curl_fetch_st *cf = &curl_fetch;                 /* pointer to fetch struct */
    struct curl_slist *headers = NULL;                      /* http headers to send with request */
    const char *url = "http://localhost";
    int port = 5006;
    const int size = sizeof(char) * 500;
    char *concat_url = malloc(size);
    /* init curl handle */
    if ((ch = curl_easy_init()) == NULL) {
        /* log error */
        fprintf(stderr, "ERROR: Failed to create curl handle in fetch_session");
        /* return error */
        return NULL;
    }

    char *encoded_collection = curl_easy_escape(ch, collection, strlen(collection));
    char *encoded_id;
    concat_url[0] = '\0';
    strcat(concat_url, url);
    strcat(concat_url, "?collection=");
    strcat(concat_url, encoded_collection);
    if (id != NULL) {
        encoded_id = curl_easy_escape(ch, id, strlen(id));
        strcat(concat_url, "&id=");
        strcat(concat_url, encoded_id);
    }
    printf("url:\n");
    printf("%s\n", concat_url);

    /* set content type */
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, "Content-Type: application/json");

    /* set curl options */
    curl_easy_setopt(ch, CURLOPT_CUSTOMREQUEST, method);
    curl_easy_setopt(ch, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(ch, CURLOPT_POSTFIELDS, data);
    curl_easy_setopt(ch, CURLOPT_PORT, port);

    /* fetch page and capture return code */
    rcode = curl_fetch_url(ch, concat_url, cf);

    /* cleanup curl handle */
    curl_easy_cleanup(ch);

    /* free headers */
    curl_slist_free_all(headers);

    /* check return code */
    if (rcode != CURLE_OK || cf->size < 1) {
        /* log error */
        fprintf(stderr, "ERROR: Failed to fetch url (%s) - curl said: %s",
                concat_url, curl_easy_strerror(rcode));
        /* return error */
        return NULL;
    }

    /* check payload */
    if (cf->payload != NULL) {
        if (print_debug) {
            /* print result */
            printf("CURL Returned: \n%s\n", cf->payload);
        }
        /* parse return */
        json = json_tokener_parse_verbose(cf->payload, &jerr);
        /* free payload */
        free(cf->payload);
    } else {
        /* error */
        fprintf(stderr, "ERROR: Failed to populate payload");
        /* free payload */
        free(cf->payload);
        /* return */
        return NULL;
    }

    /* check error */
    if (jerr != json_tokener_success) {
        /* error */
        fprintf(stderr, "ERROR: Failed to parse json string, jerr: %d", jerr);
        /* free json object */
        json_object_put(json);
        /* return */
        return NULL;
    }


    curl_free(encoded_collection);
    if (id != NULL) {
        curl_free(encoded_id);
    }

    return json;
}

int collection_entries(const char *collection, int *n_items, char ***ids) {
    json_object *json, *id_arr, *id_str;                                      /* json post body */
    /* create json object for post */
    json = json_object_new_object();

    /* build get data */
    json = send_request("GET", json_object_to_json_string(json), false, collection, NULL);

    id_arr = json_object_object_get(json, "ids");
    n_items[0] = json_object_array_length(id_arr);
    ids[0] = malloc(sizeof(char *) * n_items[0]);

    for (int i = 0; i < n_items[0]; ++i) {
        id_str = json_object_array_get_idx(id_arr, i);
        ids[0][i] = strdup(json_object_get_string(id_str));
    }

    /* free json object before return */
    json_object_put(json);
    /* exit */
    return 0;
}

int collection_entries_from_environment(int *n_items, char ***ids) {
    return collection_entries(getenv("SDOS_COLLECTION"), n_items, ids);
}

json_object *load_entry(const char *collection, const char *id, bool print_debug) {
    json_object_put(cached_object);
    cached_object = json_object_new_object();

    /* build post data */
    cached_object = send_request("GET", json_object_to_json_string(cached_object), print_debug, collection, id);
    return cached_object;
}

json_object *load_entry_from_environment() {
    return load_entry(
            getenv("SDOS_COLLECTION"),
            getenv("SDOS_ID"),
            false);
}

int extract_int(const char *param) {
    json_object *int_obj = json_object_object_get(cached_object, param);
    return json_object_get_int(int_obj);
}

void persist_entry(const char *collection, bool print_debug, char **generated_id, int *id_length) {
    if (object_to_persist == NULL) {
        /* error */
        fprintf(stderr, "ERROR: No object to persist has been created yet.");
    }

    /* build post data */
    cached_object = send_request("POST", json_object_to_json_string(object_to_persist), print_debug, collection, NULL);
}

// ################################################
// # From here on, the data access methods follow #
// # Load an entry e.g. with load_entry(...) first#
// ################################################

void extract_int_array(const char *param, int *arr_length, int **arr) {
    json_object *arr_obj = json_object_object_get(cached_object, param);
    json_object *tmp_elem;
    arr_length[0] = json_object_array_length(arr_obj);
    arr[0] = malloc(sizeof(int) * arr_length[0]);

    for (int i = 0; i < arr_length[0]; ++i) {
        tmp_elem = json_object_array_get_idx(arr_obj, i);
        arr[0][i] = json_object_get_int(tmp_elem);
    }
}

int64_t extract_long(const char *param) {
    json_object *int_obj = json_object_object_get(cached_object, param);
    return json_object_get_int64(int_obj);
}

void extract_long_array(const char *param, int *arr_length, int64_t **arr) {
    json_object *arr_obj = json_object_object_get(cached_object, param);
    json_object *tmp_elem;
    arr_length[0] = json_object_array_length(arr_obj);
    arr[0] = malloc(sizeof(int64_t) * arr_length[0]);

    for (int i = 0; i < arr_length[0]; ++i) {
        tmp_elem = json_object_array_get_idx(arr_obj, i);
        arr[0][i] = json_object_get_int64(tmp_elem);
    }
}

double extract_double(const char *param) {
    json_object *double_obj = json_object_object_get(cached_object, param);
    return json_object_get_double(double_obj);
}

void extract_double_array(const char *param, int *arr_length, double **arr) {
    json_object *arr_obj = json_object_object_get(cached_object, param);
    json_object *tmp_elem;
    arr_length[0] = json_object_array_length(arr_obj);
    arr[0] = malloc(sizeof(double) * arr_length[0]);

    for (int i = 0; i < arr_length[0]; ++i) {
        tmp_elem = json_object_array_get_idx(arr_obj, i);
        arr[0][i] = json_object_get_double(tmp_elem);
    }
}

char *extract_string(const char *param) {
    json_object *double_obj = json_object_object_get(cached_object, param);
    return strdup(json_object_get_string(double_obj));
}

void extract_string_array(const char *param, int *arr_length, char ***arr) {
    json_object *arr_obj = json_object_object_get(cached_object, param);
    json_object *tmp_elem;
    arr_length[0] = json_object_array_length(arr_obj);
    arr[0] = malloc(sizeof(char *) * arr_length[0]);

    for (int i = 0; i < arr_length[0]; ++i) {
        tmp_elem = json_object_array_get_idx(arr_obj, i);
        arr[0][i] = strdup(json_object_get_string(tmp_elem));
    }
}


// ###############################################
// # From here on, the data write methods follow #
// ###############################################

void init_object() {
    if (object_to_persist == NULL) {
        object_to_persist = json_object_new_object();
    }
}

void write_int(const char *key, int value) {
    init_object();
    json_object_object_add(object_to_persist, key, json_object_new_int(value));
}

void write_int_array(const char *key, int arr_length, int *arr) {
    init_object();
    json_object *arr_obj = json_object_new_array_ext(arr_length);
    for (int i = 0; i < arr_length; ++i) {
        json_object *elem_obj = json_object_new_int(arr[i]);
        json_object_array_put_idx(arr_obj, i, elem_obj);
    }
    json_object_object_add(object_to_persist, key, arr_obj);
}

void write_long(const char *key, int64_t value) {
    init_object();
    json_object_object_add(object_to_persist, key, json_object_new_int64(value));
}

void write_long_array(const char *key, int arr_length, int64_t *arr) {
    init_object();
    json_object *arr_obj = json_object_new_array_ext(arr_length);
    for (int i = 0; i < arr_length; ++i) {
        json_object *elem_obj = json_object_new_int64(arr[i]);
        json_object_array_put_idx(arr_obj, i, elem_obj);
    }
    json_object_object_add(object_to_persist, key, arr_obj);
}

void write_double(const char *key, double value) {
    init_object();
    json_object_object_add(object_to_persist, key, json_object_new_double(value));
}

void write_double_array(const char *key, int arr_length, double *arr) {
    init_object();
    json_object *arr_obj = json_object_new_array_ext(arr_length);
    for (int i = 0; i < arr_length; ++i) {
        json_object *elem_obj = json_object_new_double(arr[i]);
        json_object_array_put_idx(arr_obj, i, elem_obj);
    }
    json_object_object_add(object_to_persist, key, arr_obj);
}

void write_string(const char *key, char *value) {
    init_object();
    json_object_object_add(object_to_persist, key, json_object_new_string(value));
}

void write_string_array(const char *key, int arr_length, char **arr) {
    init_object();
    json_object *arr_obj = json_object_new_array_ext(arr_length);
    for (int i = 0; i < arr_length; ++i) {
        json_object *elem_obj = json_object_new_string(arr[i]);
        json_object_array_put_idx(arr_obj, i, elem_obj);
    }
    json_object_object_add(object_to_persist, key, arr_obj);
}

void write_custom_json(const char *key, json_object *obj) {
    init_object();
    json_object_object_add(object_to_persist, key, obj);
}