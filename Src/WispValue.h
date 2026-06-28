//
// Created by Abhinav Jha on 07/06/26.
//

#ifndef WISP_WISPVALUE_H
#define WISP_WISPVALUE_H

#include <cstdint>
#include <memory>
#include <string>
#include <utility>

namespace Wisp {

enum class WispValueKind {
    Int,
    Float,
    Bool,
    String,
    Error,
};

class WispValue {
public:
    virtual ~WispValue() = default;
    virtual WispValueKind kind() const = 0;
    virtual std::string to_string() const = 0;
    virtual std::unique_ptr<WispValue> clone() const = 0;
};


class IntValue : public WispValue {
public:
    explicit IntValue(int32_t value) : m_value(value) {}
    WispValueKind kind() const override { return WispValueKind::Int; }
    std::int32_t value() const { return m_value; }
    std::string to_string() const override { return std::to_string(m_value); }
    std::unique_ptr<WispValue> clone() const override { return std::make_unique<IntValue>(m_value); }

private:
    std::int32_t m_value;
};


class FloatValue : public WispValue {
public:
    explicit FloatValue(float value) : m_value(value) {}
    WispValueKind kind() const override { return WispValueKind::Float; }
    float value() const { return m_value; }
    std::string to_string() const override { return std::to_string(m_value); }
    std::unique_ptr<WispValue> clone() const override { return std::make_unique<FloatValue>(m_value); }

private:
    float m_value;
};


class BoolValue : public WispValue {
public:
    explicit BoolValue(bool value) : m_value(value) {}
    WispValueKind kind() const override { return WispValueKind::Bool; }
    bool value() const { return m_value; }
    std::string to_string() const override { return m_value ? "true" : "false"; }
    std::unique_ptr<WispValue> clone() const override { return std::make_unique<BoolValue>(m_value); }

private:
    bool m_value;
};


class StringValue : public WispValue {
public:
    explicit StringValue(std::string value) : m_value(std::move(value)) {}
    WispValueKind kind() const override { return WispValueKind::String; }
    const std::string& value() const { return m_value; }
    std::string to_string() const override { return m_value; }
    std::unique_ptr<WispValue> clone() const override { return std::make_unique<StringValue>(m_value); }

private:
    std::string m_value;
};

class ErrorValue : public WispValue {
public:
    ErrorValue(std::string value, std::size_t line) : m_value(std::move(value)), m_line(line) {}
    WispValueKind kind() const override { return WispValueKind::Error; }
    const std::string& message() const { return m_value; }
    std::string to_string() const override { return m_value; }
    std::size_t line() const { return m_line; }
    std::unique_ptr<WispValue> clone() const override { return std::make_unique<ErrorValue>(m_value, m_line); }

private:
    std::string m_value;
    std::size_t m_line;
};

}   // namespace Wisp


#endif //WISP_WISPVALUE_H
