/*****************************************************
*  PROJECT  : IO Catcher                             *
*  LICENSE  : Apache 2.0                             *
*  COPYRIGHT: 2022 Sebastien Valat                   *
*****************************************************/

/****************************************************/
#include <gtest/gtest.h>
#include <thread>
#include "../SharedQueue.hpp"

/****************************************************/
using namespace IOC;
using namespace testing;

/****************************************************/
TEST(TestShareqQueue, constructor)
{
	SharedQueue<int> queue(true);
}

/****************************************************/
TEST(TestShareqQueue, pop_empty_no_wait)
{
	SharedQueue<int> queue(false);
	EXPECT_EQ(nullptr, queue.pop());
}

/****************************************************/
TEST(TestShareqQueue, push_pop)
{
	SharedQueue<int> queue(false);
	int a = 10;
	queue.push(&a);
	EXPECT_EQ(&a, queue.pop());
}

/****************************************************/
TEST(TestShareqQueue, pop_wait)
{
	//vars
	SharedQueue<int> queue(true);
	int a = 10;
	volatile bool threadReady = false;
	
	//spaw a thread to wait
	std::thread thread([&queue, &a, &threadReady](){
		threadReady = true;
		EXPECT_EQ(&a, queue.pop());
	});

	//wait thread to be ready to pop & wait
	while (!threadReady) {};

	//push a value which will wake up the thread
	queue.push(&a);

	//wait the thread
	thread.join();
}

/****************************************************/
TEST(TestShareqQueue, pop_multi_wait)
{
	//vars
	SharedQueue<int> queue(true);
	int a = 10;
	int b = 20;
	volatile bool thread1Ready = false;
	volatile bool thread2Ready = false;
	int sumThread1 = 0;
	int sumThread2 = 0;
	
	//spaw a thread to wait
	std::thread thread1([&sumThread1, &queue, &a, &b, &thread1Ready](){
		thread1Ready = true;
		for (int i = 0 ; i < 1000 ; i++) {
			int * value = queue.pop();
			bool ok = (value == &a || value == &b);
			EXPECT_TRUE(ok);
			sumThread1 += *value;
		}
	});

	//spaw a thread to wait
	std::thread thread2([&sumThread2, &queue, &a, &b, &thread2Ready](){
		thread2Ready = true;
		for (int i = 0 ; i < 1000 ; i++) {
			int * value = queue.pop();
			bool ok = (value == &a || value == &b);
			EXPECT_TRUE(ok);
			sumThread2 += *value;
		}
	});

	//wait thread to be ready to pop & wait
	while (!thread1Ready || !thread2Ready) {};

	//push a value which will wake up the thread
	for (int i = 0 ; i < 1000 ; i++) {
		queue.push(&a);
		queue.push(&b);
	}

	//wait the thread
	thread1.join();
	thread2.join();

	//check
	EXPECT_EQ(a * 1000 + b * 1000, sumThread1 + sumThread2);
}
