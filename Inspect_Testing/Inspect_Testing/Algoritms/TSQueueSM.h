/**
 * \copyright (C) Synapse Imaging 2023
 *
 * @File Name
 *      Inspect_Thread.h
 *
 * @Summary
 *
 * @Description
 *
 *
 * @authors Igor Dunin-Barkowski, Iurii Kim
 * @version 1.0
 * @date 2023.06.20
 */
 /* ************************************************************************** */
 /* Section: Included Files                                                    */
/* ************************************************************************** */
#pragma once
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>

// Thread-safe queue
template <typename T>
class TSQueueSM 
{

private:
	// Underlying queue
	std::queue<T> m_queue;

	// mutex for thread synchronization
	std::mutex m_mutex;

	// Condition variable for signaling
	std::condition_variable m_cond;

public:
	// Pushes an element to the queue
	void Push(T item)
	{
		// Acquire lock
		std::unique_lock<std::mutex> lock(m_mutex);

		// Add item
		// m_queue.push(item);

		// Notify one thread that
		// is waiting
		//m_cond.notify_one();

		m_queue.push(std::move(item));
        m_cond.notify_one();
	}

	// Pops an element off the queue
	T Pop()
	{
		// acquire lock
		std::unique_lock<std::mutex> lock(m_mutex);

		//// wait until queue is not empty
		//m_cond.wait(lock,
		//	[this]() { return !m_queue.empty(); });

		// T item = m_queue.front();
		// m_queue.pop();
		// return item;

        m_cond.wait(lock, [this]() { return !m_queue.empty(); });

        T item = std::move(m_queue.front());
        m_queue.pop();
        return item;
	}
	// Checks if the underlying container has no elements
	bool Empty()
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		return m_queue.empty();
	}
	// Size
	int Size()
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		return m_queue.size();

	}
};

/* ************************************************************************** */
/* End of file                                                                */
/* ************************************************************************** */
