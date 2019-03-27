#include <iostream>
#include <exampledatabase.h>
#include <boost/interprocess/sync/named_semaphore.hpp>

class DatabaseService
{
public:
    DatabaseService(Database * db, const char * key, int timeout)
        : db_(db)
        , semaphore_(boost::interprocess::open_or_create, key, 0)
    {
    }

    void run()
    {
        boost::posix_time::seconds p(1);
        while(semaphore_.timed_wait(boost::posix_time::milliseconds(1)))
        {

        }

    }

private:
    Database * db_;

    boost::interprocess::named_semaphore semaphore_;

    boost::posix_time::ptime timeout_;
};

int main (int argc, char *argv[])
{
    DatabaseConfig config =
    {
        "exampledb",
        {
            { 102400, "table1", "/tmp/table1.src", "/tmp/table1.lock"},
            { 204800, "table2", "/tmp/table2.src", "/tmp/table2.lock"},
            { 409600, "table3", "/tmp/table3.src", "/tmp/table3.lock"},
            { 102400, "table4", "/tmp/table4.src", "/tmp/table4.lock"},
            { 102400, "table5", "/tmp/table5.src", "/tmp/table5.lock"},
        }
    };

    ExampleDatabase db{config};
    if(!db.create())
    {
        std::cout << "create db error" << std::endl;
        return -1;
    }

	return 0;
}
