#pragma once
namespace ui {
enum class UIInteractionState { Normal, Hovered, Focused, Pressed, Disabled, Selected };
struct UIState { UIInteractionState value = UIInteractionState::Normal; };
}
