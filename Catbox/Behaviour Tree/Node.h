#pragma once
#include <memory>

class Node
{
public:
    enum class Status
    {
        Invalid,
        Success,
        Failure,
        Running,
    };

    virtual ~Node() {}

    virtual Status Update() = 0;
    virtual void Initialize() {}
    virtual void Terminate(Status aStatus) {}

    Status Run();

    bool IsSuccess() const;
    bool IsFailure() const;
    bool IsRunning() const;
    bool IsTerminated() const;
    void Reset();
protected:

    Status myStatus = Status::Invalid;
};