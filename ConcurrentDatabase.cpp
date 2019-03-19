#include "ConcurrentDatabase.h"

DatabaseReader::DatabaseReader(Database * db)
    : db_(db)
    , readIndex_(db_->meta().read_index)
{
    db_->meta().increaseRef(readIndex_);
}

DatabaseReader::~DatabaseReader()
{
    db_->meta().decreaseRef(readIndex_);
}
