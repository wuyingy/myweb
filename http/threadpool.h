#ifndef THREADPOOL_H
#define THREADPOOL_H
#include "iostream"
#include "pthread.h"
#include "exception"
#include "list"
#include "restrict.h"
using namespace std;
using namespace RESTRICT;

namespace THREADPOOL
{
	template<typename T>//模板参数 T是任务类
	class threadpool
	{
		private:
			pthread_t* m_thread=NULL;			
			int m_thread_n;
			int m_max_request;
			list<T*> m_workqueue;
			bool m_stop;
			sem m_sem;
			locker m_queuelocker;
		public:
			threadpool(int thread_number,int max_request);
			~threadpool();
		        static void* worker(void* arg);
			void run();
			void append(T* request);
	};
	template<typename T>
	threadpool<T>::threadpool(int thread_number,int max_request):m_thread_n(thread_number),m_max_request(max_request),m_stop(false)
	{
		if((m_thread_n<=0)||(m_max_request<=0))
		{
			throw std::exception();
		}
		m_thread= new pthread_t[m_thread_n];
		if(!m_thread)
		{
			throw std::exception();
		}

		int i=0;
		for(;i<m_thread_n;i++)
		{
			if(pthread_create(m_thread+i,NULL,worker,this)!=0)
			{
				cout<<"Create thread "<<i<<"error"<<endl;
				delete [] m_thread;
			        throw std::exception();
			}
			if(pthread_detach(m_thread[i]))
			{
				delete [] m_thread;
			        throw std::exception();
			}
		}
	}

	template<class T>
	threadpool<T>::~threadpool()
	{
		delete [] m_thread;
		m_stop=true;
	}

	//向工作队列中增加工作的对象
	template<typename T>
	void threadpool<T>::append(T* request)
	{
		m_queuelocker.lock();
		if(m_workqueue.size()>(m_max_request))
		{
			cout<<"Sorry,this cantainer is full,please wait for a moment."<<endl;
			m_queuelocker.unlock();
			return;
		}
		m_workqueue.push_back(request);
		m_queuelocker.unlock();
		m_sem.post();
	}

	template<class T>
	void* threadpool<T>::worker(void* arg)
	{
		threadpool* thisObject=(threadpool*) arg;
		thisObject->run();
		return thisObject;
	}

	template<typename T>
	void threadpool<T>::run()
	{
		while(!m_stop)
		{
			m_sem.wait();
			m_queuelocker.lock();
			if(m_workqueue.empty())
			{
				m_queuelocker.unlock();
				continue;
			}
			T* request=m_workqueue.front();
			m_workqueue.pop_front();
			m_queuelocker.unlock();
			if(!request)
			{
				continue;
			}

			request->process();
		}
	}
}
#endif
