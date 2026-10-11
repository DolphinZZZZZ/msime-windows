#include "Key/CommitReplyWaitPolicy.h"

namespace
{
using Policy = CommitReplyTimeoutPolicy;
} // namespace

int main()
{
    // Active commit-reply path must preserve input past the observation budget.
    if (kCommitReplyTimeoutPolicy != Policy::PreserveInputKeepWaiting)
        return 1;
    if (ShouldAbortTransportOnCommitReplyTimeout(kCommitReplyTimeoutPolicy))
        return 2;
    if (!ShouldKeepWaitingPastCommitReplyObservation(kCommitReplyTimeoutPolicy))
        return 3;
    if (ShouldSoftMissOnCommitReplyTimeout(kCommitReplyTimeoutPolicy))
        return 4;

    // Legacy AbortTransport still tears down on timeout.
    if (!ShouldAbortTransportOnCommitReplyTimeout(Policy::AbortTransport))
        return 5;
    if (ShouldKeepWaitingPastCommitReplyObservation(Policy::AbortTransport))
        return 6;
    if (ShouldSoftMissOnCommitReplyTimeout(Policy::AbortTransport))
        return 7;

    // SoftMiss is for ordinary / navigation waits, not commit teardown.
    if (ShouldAbortTransportOnCommitReplyTimeout(Policy::SoftMiss))
        return 8;
    if (ShouldKeepWaitingPastCommitReplyObservation(Policy::SoftMiss))
        return 9;
    if (!ShouldSoftMissOnCommitReplyTimeout(Policy::SoftMiss))
        return 10;

    return 0;
}
