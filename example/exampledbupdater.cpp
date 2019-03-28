#include <iostream>
#include <boost/interprocess/sync/named_semaphore.hpp>
#include <boost/interprocess/exceptions.hpp>
#include <exampledb.h>

int main (int argc, char *argv[])
{
    if(argc != 3)
    {
        std::cout << "usage: " << argv[0] << " tableName data" << std::endl;
        return -1;
    }
    const char * tableName = argv[1];
    const char * data = argv[2];
    size_t dataSize = strlen(data)+1;

    DatabaseConfig config =
    {
        "exampledb",
        {
            { 102400, "table1", "/data/table1.src", "/data/table1.lock"},
            { 204800, "table2", "/data/table2.src", "/data/table2.lock"},
            { 409600, "table3", "/data/table3.src", "/data/table3.lock"},
            { 102400, "table4", "/data/table4.src", "/data/table4.lock"},
            { 102400, "table5", "/data/table5.src", "/data/table5.lock"},
        }
    };

    auto iter = std::find_if(config.tables.begin(), config.tables.end(), [&tableName](const TableConfig & c)
    {
        if(c.name == tableName)
            return true;
        return false;
    });

    if(iter == config.tables.end())
    {
        std::cout << "table " << tableName << "not found" << std::endl;
        return -1;
    }

    try
    {
        boost::interprocess::file_lock flock(iter->lockfile.c_str());
        boost::interprocess::scoped_lock<boost::interprocess::file_lock> l{flock};
        std::cout << "lock file: " << iter->lockfile << std::endl;

        {
            boost::filesystem::resize_file(iter->path.c_str(), dataSize);
            boost::interprocess::file_mapping file(iter->path.c_str(), boost::interprocess::read_write);
            boost::interprocess::mapped_region region(file, boost::interprocess::read_write);
            memcpy(region.get_address(), data, dataSize);
            //region.flush(0, dataSize);
            std::cout << "mapped : " << iter->path << ", size: " << region.get_size() << std::endl;
        }

        // notify
        l.unlock();
        boost::interprocess::named_semaphore semaphore{boost::interprocess::open_only, config.name.c_str()};
        semaphore.post();
        std::cout << "notify: " << iter->name << std::endl;
    }
    catch(boost::interprocess::interprocess_exception & e)
    {
        std::cout << "error: " << e.what() << std::endl;
    }

    return 0;
}
