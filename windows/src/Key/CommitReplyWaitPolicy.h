#pragma once

// How a commit-reply wait treats the observation budget
// (FANY_IME_COMMIT_REPLY_TIMEOUT_MS). Kept free of TSF/COM and Windows headers
// so the mapping is unit-tested (tests/commit_reply_wait_policy.cpp).
//
// Background: a Space / digit / punctuation selection request has already been
// written to the Server. Waiting for the same request_id is the only way to
// learn which candidate it chose. The old policy aborted the reply pipe and
// cleared the deferred queue when the observation budget elapsed, which
// dropped in-flight input on a slow machine. PreserveInputKeepWaiting keeps
// the pipe and the queue; only a real transport failure tears them down.

enum class CommitReplyTimeoutPolicy
{
    // Leave the pipe up and return an empty Normal frame for the caller to
    // fall back (used by ordinary / navigation-only waits, not commits).
    SoftMiss,
    // Close the reply pipe, return TransportUnavailable, and let deferred-key
    // failure policy clear the queue (legacy commit-reply behaviour).
    AbortTransport,
    // Log a slow-request observation once the budget elapses, then keep
    // waiting for the same request_id. Do not close the pipe or clear state.
    PreserveInputKeepWaiting,
};

// Active policy for TryReadCommitReplyFromServerPipe.
// The commit-reply hot path hard-wires keepWaitingPastTimeout=true and
// static_asserts this value; do not reintroduce a runtime SoftMiss/Abort branch there.
inline constexpr CommitReplyTimeoutPolicy kCommitReplyTimeoutPolicy =
    CommitReplyTimeoutPolicy::PreserveInputKeepWaiting;

inline constexpr bool ShouldAbortTransportOnCommitReplyTimeout(CommitReplyTimeoutPolicy policy)
{
    return policy == CommitReplyTimeoutPolicy::AbortTransport;
}

inline constexpr bool ShouldKeepWaitingPastCommitReplyObservation(CommitReplyTimeoutPolicy policy)
{
    return policy == CommitReplyTimeoutPolicy::PreserveInputKeepWaiting;
}

inline constexpr bool ShouldSoftMissOnCommitReplyTimeout(CommitReplyTimeoutPolicy policy)
{
    return policy == CommitReplyTimeoutPolicy::SoftMiss;
}
