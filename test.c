#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "cache.h"
#include "helper.h"

#define num_vals (11ULL)

key_t insert_random(cache_t cache,uint32_t size);
key_t keys[] = {"one","two","three","argvar","52","ten thousand","asdasd","asdasdasdasdasd,","!@#!@$!$~`","1","a verryy longgggg stringggggggggg...................................................................................................................................................!"};
val_t vals[num_vals];
size_t val_sizes[] = {0,1,2,26,233,1020,15221,41231,300212,1312312,6362363};
void generate_vals();
void delete_vals();
void test_cache_overflow(uint64_t max_mem);
void test_no_eviction(uint64_t max_mem);

int main(int argc,char ** argv){
    srand(23);
    generate_vals();
    const size_t num_tests = 5;
    uint64_t test_sizes[] = {10ULL,1000ULL,100000ULL,1000000ULL,100000000ULL};

    for(size_t t = 0; t < num_tests; t++){
        test_no_eviction(test_sizes[t]);
    }
    for(size_t t = 0; t < num_tests; t++){
        test_cache_overflow(test_sizes[t]);
    }
    delete_vals();
    return 0;
}
void test_no_eviction(uint64_t max_mem){
    cache_t cache = create_cache(max_mem,NULL);
    //should not evict things before memory exceeded test
    uint32_t size_sum = 0;
    size_t num_before_evict = 0;
    for(uint32_t i = 0;i < num_vals && size_sum + val_sizes[i] < max_mem;i++){
        num_before_evict += 1;
        size_sum += val_sizes[i];
    }
    for(size_t i = 0; i < num_before_evict; i++){
        cache_set(cache,keys[i],vals[i],val_sizes[i]);
    }
    if(cache_space_used(cache) != size_sum){
        printf("cache space not what expected, %u expected, %u actual\n",size_sum,cache_space_used(cache));
    }
    //makes sure all values are in the cache
    for(size_t i = 0; i < num_before_evict; i++){
        uint32_t val_size = 0;
        val_t val = cache_get(cache,keys[i],&val_size);
        if(val == NULL){
            printf("value added that did not exceed max_mem is not in cache\n");
        }
        if(val_size != val_sizes[i]){
            printf("size of value not what expected: %u expected, %u actual\n",val_sizes[i],val_size);
        }
        if(memcmp(vals[i],val,val_size)){
            printf("value not what expected, first byte, expected = %u, actual = %u\n",*(uint8_t*)(vals[i]),*(uint8_t*)(val));
        }
    }
    //deletes items in the cache
    const size_t d_i = 4;
    uint32_t d_val_size;
    cache_delete(cache,keys[d_i]);

    if(cache_get(cache,keys[d_i],&d_val_size) != NULL){
        printf("cache_get returned non-null for deleted key\n");
    }
    //checks to see if it crashes if an item is deleted twice
    cache_delete(cache,keys[d_i]);

    for(size_t i = 0; i < num_before_evict; i++){
        if(i == d_i)
            continue;
        uint32_t val_size = 0;
        val_t val = cache_get(cache,keys[i],&val_size);
        if(val == NULL){
            printf("value not deleted is not in cache after another value was deleted\n");
        }
    }

    destroy_cache(cache);
}

uint64_t get_mem_in_cache(cache_t cache){
    //gets memory in the cache without relying on the cache to actually keep track of its own memory
    uint64_t tot_val_size = 0;
    for(size_t i = 0; i < num_vals; i++){
        uint32_t val_size = 0;
        val_t val = cache_get(cache,keys[i],&val_size);
        if(val != NULL){
            tot_val_size += val_size;
        }
    }
    return tot_val_size;
}

void test_cache_overflow(uint64_t max_mem){
    cache_t cache = create_cache(max_mem,NULL);

    for(size_t item_n = 0; item_n < num_vals; item_n++){
        cache_set(cache,keys[item_n],vals[item_n],val_sizes[item_n]);

        uint64_t cache_mem = get_mem_in_cache(cache);
        if(cache_mem != cache_space_used(cache)){
            printf("cache_space_used returned incorrect value\n");
        }
        if(cache_mem > max_mem){
            printf("cache uses more memory than specified\n");
        }
    }

    destroy_cache(cache);
}

void generate_vals(){
    for(size_t v = 0; v < num_vals;v++){
        vals[v] = calloc(val_sizes[v],1);
        uint8_t * valbytes = (uint8_t *)(vals[v]);
        for(size_t i = 0; i < val_sizes[v]; i++){
            valbytes[i] = (uint8_t)(rand());
        }
    }
}
void delete_vals(){
    for(size_t v = 0; v < num_vals;v++){
        c_delete(&vals[v]);
    }
}
