#ifndef ENACT_VM_H
#define ENACT_VM_H

#include "common.h"
#include "Value.h"
#include "Chunk.h"
#include "Object.h"

#include <optional>

class EnactContext;
enum class InterpretResult;

constexpr size_t FRAMES_MAX = 64;

struct CallFrame {
    ClosureObject* closure;
    const uint8_t* ip;
    size_t slotsBegin;
};

class VM {
    friend class GC;

    EnactContext& m_context;

    std::vector<Value> m_stack{};

    std::array<CallFrame, FRAMES_MAX> m_frames{CallFrame{nullptr, nullptr, 0}};
    size_t m_frameCount = 0;

    UpvalueObject* m_openUpvalues = nullptr;
public:
    explicit VM(EnactContext& context);

    InterpretResult run(FunctionObject* function);

    void push(Value value);
    Value pop();
    Value peek(size_t depth);

    void call(ClosureObject* closure);

    UpvalueObject* captureUpvalue(uint32_t location);
    void closeUpvalues(uint32_t last);

    void runtimeError(const std::string& msg);
};

#endif //ENACT_VM_H
