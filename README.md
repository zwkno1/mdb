# memorydb
## introduce

A lockfree in-memory database.

## notice
The database should update with a fixed time interval: fti.
The cost time of query operation must less than fti, or data race occurs.


