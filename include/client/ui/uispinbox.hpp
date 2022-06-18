#ifndef UISPINBOX_HPP
#define UISPINBOX_HPP

#include "client/ui/uielement.hpp"
#include <functional>
#include <boost/signals2.hpp>

class UISpinBox : public UIElement
{
public:
    UISpinBox();

    void setMinimum(float a);
    float getMinimum() const;
    void setMaximum(float a);
    float getMaximum() const;

    void setValue(float a);
    float getValue() const;

    void setLabel(const std::string &lbl);
    const std::string &getLabel() const;

    void setPosition(const glm::vec2 &pos) override;
    const glm::vec2 &getPosition() const override;
    void setSize(const glm::vec2 &size) override;
    const glm::vec2 &getSize() const override;

    UIType getType() const override;

    // events
    void changeValueSubscribe(const std::function<void(float)> &callb);
    void onChangeValue(float a);
private:
    boost::signals2::signal<void(float)> m_changeValueEvents;

    std::string m_label;
    float m_value, m_min, m_max;
    glm::vec2 m_pos, m_size;
};

#endif // UISPINBOX_HPP
