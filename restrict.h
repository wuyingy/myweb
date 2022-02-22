#ifndef RESTRICT_H
#define RESTRICT_H
#include "iostream"
#include "pthread.h"
#include "semaphore.h"
#include "exception"

using namespace std;

namespace RESTRICT
{
	class sem
	{
		public:
			sem()
			{
				if(sem_init(&m_sem,0,0)!=0)
				{
					throw exception();//over
				}
			}
			void wait()
			{
				sem_wait(&m_sem);
			}
			void post()
			{
				sem_post(&m_sem);
			}
			~sem()
			{
				sem_destroy(&m_sem);
			}
		private:
			sem_t m_sem;
	};

	class locker
	{
		private:
			pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
		public:
			void lock()
			{
				pthread_mutex_lock(&mutex);
			}
			void unlock()
			{
				pthread_mutex_unlock(&mutex);
			}
			~locker()
			{
				pthread_mutex_destroy(&mutex);
			}
	};

	class cond
	{
		private:
			pthread_mutex_t c_mutex;
			pthread_cond_t  m_cond;
		public:
			cond()
			{
				if(pthread_mutex_init(&c_mutex,NULL)!=0)
				{
					throw exception();
				}
				if(pthread_cond_init(&m_cond,NULL)!=0)
				{
					throw exception();
				}
			}
			void wait()
			{
				pthread_mutex_lock(&c_mutex);
				pthread_cond_wait(&m_cond,&c_mutex);
			}
			void unlock()
			{
				pthread_mutex_unlock(&c_mutex);
			}
			~cond()
			{
				pthread_mutex_destroy(&c_mutex);
				pthread_cond_destroy(&m_cond);
			}
	};
}
#endif
