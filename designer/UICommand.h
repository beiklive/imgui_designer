#pragma once
#include <functional>
#include <memory>
#include <vector>

namespace designer {
class UICommand {
public:
    virtual ~UICommand() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
};
template<class T>
class PropertyChangeCommand final : public UICommand {
public:
    PropertyChangeCommand(T& target, T next) : target_(target), before_(target), after_(std::move(next)) {}
    void execute() override { target_ = after_; }
    void undo() override { target_ = before_; }
private:
    T& target_; T before_; T after_;
};
class CommandHistory {
public:
    void execute(std::unique_ptr<UICommand> command) {
        command->execute(); undo_.push_back(std::move(command)); redo_.clear();
    }
    void undo() { if (undo_.empty()) return; auto c = std::move(undo_.back()); undo_.pop_back(); c->undo(); redo_.push_back(std::move(c)); }
    void redo() { if (redo_.empty()) return; auto c = std::move(redo_.back()); redo_.pop_back(); c->execute(); undo_.push_back(std::move(c)); }
private:
    std::vector<std::unique_ptr<UICommand>> undo_, redo_;
};
}
