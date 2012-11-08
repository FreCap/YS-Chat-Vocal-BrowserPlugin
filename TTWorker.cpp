#include "TTWorker.h"
    
void TTWorker::operator()()  
{  

    boost::function<bool()> func;

    while(TTWorker::TTCallbackTasks.wait_and_pop(func)){

        func();
		
    }

}  
      
void TTWorker::start(int nThread)
{

    for (int i = 0; i < nThread; i++) {
        TTWorker worker(i);
        boost::thread workerThread(worker);  
    }

}

concurrentQueue<boost::function<bool()> > TTWorker::TTCallbackTasks(1);

