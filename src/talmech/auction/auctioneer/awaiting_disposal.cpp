#include "talmech/auction/auctioneer/auction_controller.h"
#include "talmech/auction/auctioneer/awaiting_disposal.h"

namespace talmech
{
namespace auction
{
namespace auctioneer
{
AwaitingDisposal::AwaitingDisposal()
  : AuctionState::AuctionState(states::AwaitingDisposal)
{}

int AwaitingDisposal::getNext() const
{
  return states::AnnouncingTask;
}

std::string AwaitingDisposal::str() const
{
  return "Awaiting Disposal";
}
}
}
}
