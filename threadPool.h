#include <bits/stdc++.h>

#ifndef INC_THREADPOOL_H
#define INC_THREADPOOL_H


#define MAX_THREADS 100



typedef void* threadpool;


// "dispatch_fn" declares a typed function pointer.
// A variable of type "dispatch_fn" points to a function with the following signature:
//     void dispatch_function(void *arg);
typedef void (* dispatch_fn)(void*);


//create a given no of threads return a threadpool , else return NULL
threadpool create_threadpool(int num_threads_in_pool);



//takes a threadpool as well as function pointer which to call  and a (void*)arg to pass it to the function
void dispatch(threadpool from_me,dispatch_fn dispatch_to_here,void* arg);



//this will cause all threads to destroy and memory will be cleaned up after that.
void destroy_threadpool(threadpool destroyme);


#endif