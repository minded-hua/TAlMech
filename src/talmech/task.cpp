#include "talmech/exception.h"
#include "talmech/task.h"

namespace talmech
{
Task::Task(const std::string &id)
  : id_(id)
{
  if (id_.empty())
  {
    throw Exception("The task id must not be empty.");
  }
}
}