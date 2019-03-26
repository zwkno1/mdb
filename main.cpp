#include <boost/interprocess/managed_shared_memory.hpp>
#include <cstdlib> //std::system
#include <sstream>
#include <iostream>
#include <exampledatabase.h>
#include <thread>

#if 0
int main (int argc, char *argv[])
{
   using namespace boost::interprocess;
    std::cout << sizeof(boost::interprocess::offset_t) << std::endl;
   if(argc == 1){  //Parent process
      //Remove shared memory on construction and destruction
      //struct shm_remove
      //{
      //   shm_remove() {  shared_memory_object::remove("MySharedMemory"); }
      //   ~shm_remove(){  shared_memory_object::remove("MySharedMemory"); }
      //} remover;

      //Create a managed shared memory segment
      managed_shared_memory segment(create_only, "MySharedMemory", 65536);

      //Allocate a portion of the segment (raw memory)
      managed_shared_memory::size_type free_memory = segment.get_free_memory();
      void * shptr = segment.allocate(1024/*bytes to allocate*/);

      //Check invariant
      if(free_memory <= segment.get_free_memory())
         return 1;

      //An handle from the base address can identify any byte of the shared
      //memory segment even if it is mapped in different base addresses
      managed_shared_memory::handle_t handle = segment.get_handle_from_address(shptr);
      std::stringstream s;
      s << argv[0] << " " << handle;
      s << std::ends;
      std::cout << s.str() << std::endl;
      //Launch child process
      if(0 != std::system(s.str().c_str()))
         return 1;
      //Check memory has been freed
      if(free_memory != segment.get_free_memory())
         return 1;
   }else{
      //Open managed segment
      managed_shared_memory segment(open_only, "MySharedMemory");

      //An handle from the base address can identify any byte of the shared
      //memory segment even if it is mapped in different base addresses
      managed_shared_memory::handle_t handle = 0;

      //Obtain handle value
      std::stringstream s; s << argv[1]; s >> handle;

      //Get buffer local address from handle
      void *msg = segment.get_address_from_handle(handle);

      //Deallocate previously allocated memory
      segment.deallocate(msg);
   }
   return 0;
}
#endif

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

    SharedMemory::remove(config.name.c_str());

    ExampleDatabase wdb{config};
    if(!wdb.create())
    {
        std::cout << "create" << std::endl;
        return -1;
    }

    ExampleDatabase rdb{config};
    if(!rdb.open())
    {
        std::cout << "open" << std::endl;
        return -1;
    }

    std::array<std::thread, 100> threads;

    std::atomic<int> count;
    for(auto & i : threads)
    {
        i = std::thread([&count, &rdb]()
        {
            ++count;
            for(int i = 0;i < 10000000; ++i)
            {
                ExampleDatabaseReader r(&rdb);
            }
            --count;
        });
    }

    while(count != 0)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "ref0:" << wdb.db()->meta()->refcount[0] << ", ref1: " << wdb.db()->meta()->refcount[1] << std::endl;
    }

    std::cout << "ref0:" << wdb.db()->meta()->refcount[0] << ", ref1: " << wdb.db()->meta()->refcount[1] << std::endl;
    for(auto & i : threads)
        i.join();
    ExampleDatabaseReader reader(&rdb);

    for(size_t i = 0; i < config.tables.size(); ++i)
    {
        std::cout << reader.query(i) << std::endl;
    }
}
