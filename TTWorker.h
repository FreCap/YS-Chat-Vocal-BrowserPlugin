#ifndef TSWORKER_H
#define	TSWORKER_H

#include <stdio.h>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/function.hpp>

#include "concurrentQueue.h"

using namespace boost;


class TTWorker
{    
    
public:   
     
    static concurrentQueue<boost::function<bool()> > TTCallbackTasks;
    
    TTWorker(int n): n_thread(n) {};
    
    void operator()();
       
    static void start(int nThread);
   
private:     
    int n_thread; 
};  

#endif	/* TSWORKER_H */
