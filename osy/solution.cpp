#ifndef __PROGTEST__

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <climits>
#include <cfloat>
#include <cassert>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <string>
#include <vector>
#include <array>
#include <iterator>
#include <set>
#include <list>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <compare>
#include <queue>
#include <stack>
#include <deque>
#include <memory>
#include <functional>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <stdexcept>
#include <condition_variable>
#include <pthread.h>
#include <semaphore.h>
#include "progtest_solver.h"
#include "sample_tester.h"

using namespace std;
#endif /* __PROGTEST__ */

//-------------------------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
class BlockingQueue {
public:
    template<typename... Args>
    void emplace(Args &&... args) {
        unique_lock<mutex> lock(m);
        q.emplace(std::forward<Args>(args)...);
        lock.unlock();
        cv.notify_one();
    }

    void push(const T &value) {
        unique_lock<mutex> lock(m);
        q.push(value);
        lock.unlock();
        cv.notify_one();
    }

    // pop front
    T pop() {
        unique_lock<mutex> lock(m);
        cv.wait(lock, [this] { return !q.empty(); });
        auto res = q.front();
        q.pop();
        lock.unlock();
        return res;
    }

private:
    std::queue<T> q{};
    std::mutex m;
    condition_variable cv;
};

class COptimizer {
//    Metodu solve lze zavolat pro danou instanci CProgtestSolver pouze jednou, další pokusy skončí chybou.
public:
    // don't create threads in constructor!!!!!!


    static bool usingProgtestSolver() {
        return true; // false if I want my own implementation
        // Pokud metoda vrací false, testovací prostředí upraví chování poskytovaného řešiče CProgtestSolver tak, že úmyslně vyplňuje nesprávné výsledky.
    }

    static void checkAlgorithmMin(APolygon p) {
        // dummy implementation if usingProgtestSolver() returns true
    }

    static void checkAlgorithmCnt(APolygon p) {
        // dummy implementation if usingProgtestSolver() returns true
    }

    // create 2 threads for each company and <threadCount> worker threads
    void start(int threadCount);

    // wait for workers to finito and deliver all results
    // dont call exit
    void stop() {
        // stop managers
        for (auto &project: projects) {
            project->manager.stop();
        }
        // poison pill
        tasks.push({nullptr, nullptr, SLEEP_SIGNAL});
        // wait for workers to finish
        {
            unique_lock<mutex> lock(mutex_active_workers);
            cv_active_workers.wait(lock, [this] { return n_active_workers == 0; });
        }
        // pop sleep signal
        tasks.pop();
        solverPool.stop();
        tasks.push({nullptr, nullptr, SOLUTION_COUNT});
        tasks.push({nullptr, nullptr, MIN_LENGTH});
        // after the final 2 solvers are done, threads will be killed
        tasks.push({nullptr, nullptr, STOP_SIGNAL});
        {
            unique_lock<mutex> lock(sleep_mtx);
            sleep = false;
        }
        sleep_cv.notify_all();
        // solve last problems
        // wait for worker to stop
        {
            unique_lock<mutex> lock(mutex_active_workers);
            cv_active_workers.wait(lock, [this] { return n_active_workers == 0; });
        }
        // stop senders
        for (auto &project: projects) {
            project->sender.stop();
        }
        for (auto & worker:workers) {
            worker.join();
        }
        for (auto & project:projects) {
            project->manager.join();
            project->sender.join();
        }
    }

    void addCompany(ACompany company);

private:
    enum ProblemType {
        MIN_LENGTH,
        SOLUTION_COUNT,
        STOP_SIGNAL,
        SLEEP_SIGNAL
    };

    class Batch {
    public:
        Batch() = delete;

        explicit Batch(AProblemPack &pack) : pack(pack),
                                             n_unsolved_problems(
                                                     pack->m_ProblemsCnt.size() + pack->m_ProblemsMin.size()) {}

        void waitForCompletion() {
            unique_lock<mutex> lock(mut);
            // might tak a while :(
            cond.wait(lock, [this] { return n_unsolved_problems == 0; });
            // happy sending :)
        }

        void solveTask() {
            unique_lock<mutex> lock(mut);
            n_unsolved_problems--;
            if (n_unsolved_problems == 0) {
                lock.unlock();
                cond.notify_one();
            }
        }

        AProblemPack pack;
    private:
        std::mutex mut;
        condition_variable cond;
        atomic_size_t n_unsolved_problems;
    };

    class Task {
    public:
        Task() = delete;

        Task(shared_ptr<Batch> origin_batch, APolygon polygon, ProblemType problemType) : polygon(std::move(polygon)),
                                                                                          origin_batch(std::move(
                                                                                                  origin_batch)),
                                                                                          problemType(problemType) {}


        APolygon polygon;
        shared_ptr<Batch> origin_batch;
        ProblemType problemType;
    };

    struct SolverPool {
        SolverPool() = default;

        void init() {
            unique_lock<mutex> lock(mtx);
            cnt = createProgtestCntSolver();
            min = createProgtestMinSolver();
        }

        // return AProgtest solver if full, nullptr otherwise
        pair<AProgtestSolver, vector<Task>> addTask(const Task &task) {
            unique_lock<mutex> lock(mtx);
            AProgtestSolver solver;
            // push task into vector
            if (task.problemType == MIN_LENGTH) {
                if (task.polygon)
                    tasks_min.emplace_back(task);
                solver = min;
            } else if (task.problemType == SOLUTION_COUNT) {
                if (task.polygon)
                    tasks_cnt.emplace_back(task);
                solver = cnt;
            }

            if (task.polygon) {
                solver->addPolygon(task.polygon);
            }
            if (solver->hasFreeCapacity() && !finito) {
                return {nullptr, {}};
            }

            if (task.problemType == MIN_LENGTH) {
                min = createProgtestMinSolver();
                auto old_vec = this->tasks_min;
                tasks_min.clear();
                return {solver, old_vec};
            } else {
                cnt = createProgtestCntSolver();
                auto old_vec = this->tasks_cnt;
                tasks_cnt.clear();
                return {solver, old_vec};
            }
        }

        // force SolverPool to return the solver and tasks
        pair<AProgtestSolver, vector<Task>> getCnt() {
            unique_lock<mutex> lock(mtx);
            auto res = make_pair(cnt, tasks_cnt);
            cnt = nullptr;
            tasks_cnt.clear();
            return res;
        }

        pair<AProgtestSolver, vector<Task>> getMin() {
            unique_lock<mutex> lock(mtx);
            auto res = make_pair(min, tasks_min);
            min = nullptr;
            tasks_min.clear();
            return res;
        }

        void stop() {
            finito = true;
        }

    private:
        // count
        AProgtestSolver cnt = nullptr;
        vector<Task> tasks_cnt;
        // min
        AProgtestSolver min = nullptr;
        vector<Task> tasks_min;
        // misc
        mutex mtx;
        bool finito = false;
    };

    class Worker {
    public:
        Worker(SolverPool &solvers, BlockingQueue<Task> &tasks, size_t &n_active_workers, mutex &mtx,
               condition_variable &cv, bool &sleep, mutex &sleep_mtx, condition_variable &sleep_cv)
                : solvers(solvers), tasks(tasks), n_active_workers(n_active_workers), mtx_active_workers(mtx),
                  cv_active_workers(cv), sleep(sleep), sleep_mtx(sleep_mtx),
                  sleep_cv(sleep_cv) { my_id = worker_count++; }

        void start() {
            {
                unique_lock<mutex> lock(mtx_active_workers);
                n_active_workers++;
            }
            thread = std::thread(&Worker::run, this);
        }

        void join() {
            thread.join();
        }

    private:
        void run();

        SolverPool &solvers;
        std::thread thread;
        BlockingQueue<Task> &tasks;
        static size_t worker_count;
        size_t &n_active_workers;
        mutex &mtx_active_workers;
        condition_variable &cv_active_workers;
        bool &sleep;
        mutex &sleep_mtx;
        condition_variable &sleep_cv;
        size_t my_id;
    };

    struct Project {
        explicit Project(ACompany companyPtr, BlockingQueue<Task> &tasks) : company_ptr(std::move(companyPtr)),
                                                                            worker_q_ptr(tasks) {}

        void run() {
            manager.run();
            sender.run();
        }


        class Sender {
        public:
            Sender(ACompany companyPtr, BlockingQueue<shared_ptr<Batch>> &packsToSolve)
                    : company_ptr(std::move(companyPtr)), packs_to_solve(packsToSolve) {}


            void stop() {
                unique_lock<mutex> lock(mtx);
                cv.wait(lock, [this] { return finished; });
            }

            void run() {
                thread = std::thread(&Sender::send, this);
            }

            void join() {
                thread.join();
            }

        private:
            void send();

            std::thread thread;
            ACompany company_ptr;
            BlockingQueue<shared_ptr<Batch>> &packs_to_solve;
            condition_variable cv;
            mutex mtx;
            bool finished = false;
        };

        class Manager {
        public:
            Manager(ACompany companyPtr, BlockingQueue<shared_ptr<Batch>>

            &packsToSolve, BlockingQueue<Task> &workerQPtr)
                    : company_ptr(std::move(companyPtr)), packs_to_solve(packsToSolve), worker_q_ptr(workerQPtr) {}

            void run() {
                thread = std::thread(&Manager::manage, this);
            }

            void stop() {
                unique_lock<mutex> lock(mtx);
                cv.wait(lock, [this] { return finished; });
            }

            void join() {
                thread.join();
            };
        private:
            void manage();

            std::thread thread;
            ACompany company_ptr;
            BlockingQueue<shared_ptr<Batch>> &packs_to_solve;
            BlockingQueue<Task> &worker_q_ptr;
            mutex mtx;
            condition_variable cv;
            bool finished = false;
        };

        ACompany company_ptr;
        BlockingQueue<shared_ptr<Batch>> packs_to_solve; // Project owns this
        BlockingQueue<Task> &worker_q_ptr; // borrowed from optimizer
        Manager manager{company_ptr, packs_to_solve, worker_q_ptr}; // wait for packs of problems, enqueue
        Sender sender{company_ptr, packs_to_solve}; // wait for solution of problems, send them as solved
    };

    size_t n_active_workers = 0;
    SolverPool solverPool;
    std::vector<unique_ptr<Project>> projects;
    BlockingQueue<Task> tasks;
    std::vector<Worker> workers;
    mutex mutex_active_workers;
    condition_variable cv_active_workers;

    bool sleep = true;
    condition_variable sleep_cv;
    mutex sleep_mtx;

};

void COptimizer::addCompany(ACompany company) {
    projects.emplace_back(make_unique<Project>(company, tasks));
}

void COptimizer::start(int threadCount) {
    solverPool.init();

    // get the workers ready
    for (int i = 0; i < threadCount; i++) {
        workers.emplace_back(solverPool, tasks, n_active_workers, mutex_active_workers, cv_active_workers, sleep,
                             sleep_mtx, sleep_cv);
    }
    for (auto &worker: workers) {
        worker.start();
    }
    // managers start getting new problems, senders start sending solutions
    for (auto &project: projects) {
        project->run();
    }
}


void COptimizer::Project::Sender::send() {
    while (auto batch = packs_to_solve.pop()) {
        batch->waitForCompletion();
        company_ptr->solvedPack(batch->pack);
    }

    unique_lock<mutex> lock(mtx);
    finished = true;
    lock.unlock();
    cv.notify_one();
}

void COptimizer::Project::Manager::manage() {
    while (auto pack = company_ptr->waitForPack()) {
        auto batch = make_shared<Batch>(pack);
        packs_to_solve.push(batch);
        for (const auto &polygon: pack->m_ProblemsMin) {
            worker_q_ptr.emplace(batch, polygon, MIN_LENGTH);
        }
        for (const auto &polygon: pack->m_ProblemsCnt) {
            worker_q_ptr.emplace(batch, polygon, SOLUTION_COUNT);
        }
    }
    // signal to sender that there are no more packs to solve
    packs_to_solve.push(nullptr);
    // manager is now done
    unique_lock<mutex> lock(mtx);
    finished = true;
    lock.unlock();
    cv.notify_one();
}

void COptimizer::Worker::run() {
    while (true) {
        auto curr_task = this->tasks.pop();

        if (curr_task.problemType != MIN_LENGTH && curr_task.problemType != SOLUTION_COUNT) {
            // thread is about to sleep
            tasks.push(curr_task);
            {
                unique_lock<mutex> lock(mtx_active_workers);
                n_active_workers--;
            }
            cv_active_workers.notify_one();
            if (curr_task.problemType == SLEEP_SIGNAL) {
                unique_lock<mutex> lock(sleep_mtx);
                sleep_cv.wait(lock, [this] { return !sleep; });
                {
                    unique_lock<mutex> counter_lock(mtx_active_workers);
                    n_active_workers++;
                }
                continue;
            } else {
                return;
            }
        }
        auto [solver, todo] = solvers.addTask(curr_task);
        if (todo.empty()) {
            continue;
        } // not full
        solver->solve();
        // signal that the task had been solved
        // allowing sender to send a completed batch of tasks
        for (const auto &task: todo) {
            task.origin_batch->solveTask();
        }
    }
}

size_t COptimizer::Worker::worker_count = 0;


// TODO: COptimizer implementation goes here
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifndef __PROGTEST__

int main() {
    COptimizer optimizer;
    ACompanyTest company = std::make_shared<CCompanyTest>();
    optimizer.addCompany(company);
    optimizer.start(300);
    optimizer.stop();
    if (!company->allProcessed())
        throw std::logic_error("(some) problems were not correctly processsed");
    return 0;
}

#endif /* __PROGTEST__ */
