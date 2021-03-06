/*********************************************************************************/
/********** THIS FILE IS GENERATED BY TOUCHGFX DESIGNER, DO NOT MODIFY ***********/
/*********************************************************************************/
#ifndef LOCKVIEWBASE_HPP
#define LOCKVIEWBASE_HPP

#include <gui/common/FrontendApplication.hpp>
#include <mvp/View.hpp>
#include <gui/lock_screen/LockPresenter.hpp>
#include <touchgfx/widgets/Box.hpp>
#include <touchgfx/widgets/canvas/Circle.hpp>
#include <touchgfx/widgets/canvas/PainterRGB565.hpp>
#include <touchgfx/widgets/canvas/Line.hpp>
#include <touchgfx/widgets/TextArea.hpp>

class LockViewBase : public touchgfx::View<LockPresenter>
{
public:
    LockViewBase();
    virtual ~LockViewBase() {}
    virtual void setupScreen();

protected:
    FrontendApplication& application() {
        return *static_cast<FrontendApplication*>(touchgfx::Application::getInstance());
    }

    /*
     * Member Declarations
     */
    touchgfx::Box __background;
    touchgfx::Circle circle1;
    touchgfx::PainterRGB565 circle1Painter;
    touchgfx::Circle circle2;
    touchgfx::PainterRGB565 circle2Painter;
    touchgfx::Circle circle2_1;
    touchgfx::PainterRGB565 circle2_1Painter;
    touchgfx::Line line1;
    touchgfx::PainterRGB565 line1Painter;
    touchgfx::Line line1_1;
    touchgfx::PainterRGB565 line1_1Painter;
    touchgfx::TextArea textArea1;
    touchgfx::TextArea textArea1_1;
    touchgfx::TextArea textArea1_2;
    touchgfx::TextArea textArea1_3;
    touchgfx::TextArea textArea2;

private:

    /*
     * Canvas Buffer Size
     */
    static const uint16_t CANVAS_BUFFER_SIZE = 3600;
    uint8_t canvasBuffer[CANVAS_BUFFER_SIZE];
};

#endif // LOCKVIEWBASE_HPP
