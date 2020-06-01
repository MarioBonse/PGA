#include <cassert>
#include <condition_variable>

class Barrier
{
public:
    Barrier(){};
    Barrier(int nb_threads)
        : m_mutex(),
        m_condition(),
        m_nb_threads(nb_threads),
        counter(nb_threads)
    {
        assert(0 != m_nb_threads);
    }

    void set_barrier(int new_counter){
        m_nb_threads = new_counter;
        counter = new_counter;
    };

    Barrier(const Barrier& barrier) = delete;

    Barrier(Barrier&& barrier) = delete;

    ~Barrier() noexcept{}

    Barrier& operator=(const Barrier& barrier) = delete;

    Barrier& operator=(Barrier&& barrier) = delete;

    void Wait()
    {
        std::unique_lock< std::mutex > lock(m_mutex);

        if (0 == --counter)
        {   
            m_condition.notify_all();
            counter = m_nb_threads;
        }
        else
        {
            m_condition.wait(lock, [this]() { return 0 == counter; });
        }
    }

private:

    std::mutex m_mutex;

    std::condition_variable m_condition;

    int counter;
    int m_nb_threads;
};