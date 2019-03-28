#include <iostream>
#include <exampledb.h>
#include <databaseservice.h>

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
    if(!db.create())
    {
        std::cout << "create db error" << std::endl;
        return -1;
    }

    DatabaseService service{db.db(), config.name.c_str()};

    service.run();

	return 0;
}
