/*****************************************************
*  PROJECT  : IO Catcher                             *
*  LICENSE  : Apache 2.0                             *
*  COPYRIGHT: 2022 Sebastien Valat                   *
*****************************************************/

#ifndef IOC_SHARED_QUEUE_HPP
#define IOC_SHARED_QUEUE_HPP

/****************************************************/
#include "base/common/Debug.hpp"
#include <queue>
#include <pthread.h>

/****************************************************/
namespace IOC
{

/****************************************************/
/**
 * Implement a shared queue to transmit the tasks to the worker
 * and fetch back the ready task in the network handling thread.
 * The template type define what value type to be pushed in the queue.
**/
template <class T>
class SharedQueue
{
	public:
		SharedQueue(bool sleepOnEmpty);
		~SharedQueue(void);
		void push(T * value);
		T * pop(void);
	private:
		/** Says to sleep when there is no value otherwise return NULL. **/
		bool sleepOnEmpty;
		/** Queue containing the elements. **/
		std::queue<T*> queue;
		/** Mutex to protect the queue. **/
		pthread_mutex_t mutex;
		/** Condition to wake up a thread. **/
		pthread_cond_t cond;
};

/****************************************************/
/**
 * Constructor of the shared queue.
 * @param sleepOnEmpty If the queue is empty make the thread sleeping waiting
 * a the pthread_cond_wait to be signaled.
**/
template <class T>
SharedQueue<T>::SharedQueue(bool sleepOnEmpty)
{
	//set content
	this->sleepOnEmpty = sleepOnEmpty;

	//init locking system
	pthread_mutex_init(&this->mutex, NULL);
	pthread_cond_init(&this->cond, NULL);
}

/****************************************************/
/**
 * Destructor of the shared queue.
**/
template <class T>
SharedQueue<T>::~SharedQueue(void)
{
	//lock
	pthread_mutex_lock(&this->mutex);

	//check empty
	assume(this->queue.empty(), "Try to destroy a shared queue which is not empty !");

	//destroy
	pthread_mutex_destroy(&this->mutex);
	pthread_cond_destroy(&this->cond);
}

/****************************************************/
/**
 * Push a value in the queue.
**/
template <class T>
void SharedQueue<T>::push(T * value)
{
	//lock
	pthread_mutex_lock(&this->mutex);

	//insert
	this->queue.push(value);

	//if was empty we need to possibly wake-up a thread
	pthread_cond_signal(&this->cond);

	//unlock
	pthread_mutex_unlock(&this->mutex);
}

/****************************************************/
/**
 * Pop a value from the queue. This function can sleep until a value is pushed.
 * If sleeping mode is disabled then it return NULL if the queue is empty.
**/
template <class T>
T * SharedQueue<T>::pop(void)
{
	//vars
	T * result;

	//lock
	pthread_mutex_lock(&this->mutex);

	//check if empty
	if (this->queue.empty()) {
		//if wait is sleep we wait and try again
		if (sleepOnEmpty) {
			while (this->queue.empty()) {
				pthread_cond_wait(&this->cond, &this->mutex);
			}
			result = this->queue.front();
			this->queue.pop();
		} else {
			result = NULL;
		}
	} else {
		result = this->queue.front();
		this->queue.pop();
	}

	//unlock
	pthread_mutex_unlock(&this->mutex);

	//ok
	return result;
}

}

#endif //IOC_SHARED_QUEUE_HPP
