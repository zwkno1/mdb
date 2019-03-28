#include <iostream>
#include <exampledb.h>
#include <databaseservice.h>
#include <thread>

int main (int argc, char *argv[])
{
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

    ExampleDatabase db{config};
    if(!db.open())
    {
        std::cout << "open db error" << std::endl;
        return -1;
    }

    while(true)
    {
        ExampleDatabaseReader reader{&db};
        std::cout << "-------------------------" << std::endl;
        std::cout << reader.active() << std::endl;

        for(size_t i = 0; i < config.tables.size(); ++ i)
        {
            const char * data = reader.query(i);
            std::cout << "table" << i << " data: " << data << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::seconds(3));
        std::cout << "-------------------------" << std::endl;
    }

	return 0;
}
