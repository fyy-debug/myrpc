#pragma once
#include <queue>
#include <thread>
#include <mutex> // pthread_mutex_t
#include <condition_variable> // pthread_condition_t

// 模板的声明和定义都要写在头文件里
// 分文件编写可能造成链接错误
// 异步写日志使用的队列
// 生产者消费者模型

// 异步写日志的日志队列
template <typename T>
class LockQueue{
public:
    // producer
    // 多个worker线程都会写日志queue
    void Push(const T &data){
        // 加锁
        std::lock_guard<std::mutex> lock(m_mutex);
        // 放数据
        m_queue.push(data);
        m_condvariable.notify_one();
    }

    // 一个写线程读日志queue，将其写入磁盘中的日志文件
    T Pop(){
        std::unique_lock<std::mutex> lock(m_mutex);
        while(m_queue.empty()){
            // 日志队列为空，线程进入wait状态
            m_condvariable.wait(lock);
        }

        // 不为空了以后
        T data=m_queue.front();
        m_queue.pop();
        return data;
    }


private:
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_condvariable;

};