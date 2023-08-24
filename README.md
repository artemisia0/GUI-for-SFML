# SSGUI_SFML_GUI
_Simplest and Smallest GUI header-only library for SFML. Written for beautiful game ui creating._

## Description
It is small header-only GUI library for SFML. SSGUI was written for my game. It is neccessary to have button/knob/slider sprites to use those widgets. If You're not a UI designer like me, consider using something like https://www.g200kg.com/en/webknobman/. It is awesome tool for creating nice UI elements at the very ease. You should export sprites as vertical spritesheets (to be SSGUI-compatible). My library has only essential widgets like a button, knob, either vertical or horizontal slider, line edit or text entry and that's all. You can contribute it and add maybe menu or check box...

## Features
* Small header-only library (less than 700 LOC)
* Image-based (to create a knob prepared earlier spritesheet is required)
* Support for creating buttons, sliders (vertical and horizontal), unicode text entries and knobs
* Written in C++ 17. Tested with g++ and clang++ (MSVC and other stuff should work too...)

## Getting started
To start using SSGUI You will need to install SFML, include ssgui.hpp header to your source code file and compile ssgui.cpp

## Tutorial
Go to 'example' directory.
Here's code from it.
To understand ssgui stuff better You can check it's interface in ssgui.hpp (see comments and some prototypes).

```cpp
#include <SFML/Graphics/RenderWindow.hpp>

#define SSGUI_IMPL
#include "ssgui.hpp"

// Hi! It is near real-world ssgui usage example


int main()  // Maybe too much maybe boilerplate code, but maybe still ok...
{
    // Creating resources
    sf::Texture buttonTexture;
    sf::Texture vsliderTexture;
    sf::Texture hsliderTexture;
    sf::Texture knobTexture;

    // Loading resources
    assert(buttonTexture.loadFromFile("./buttonTexture.png"));
    assert(vsliderTexture.loadFromFile("./vsliderTexture.png"));
    assert(hsliderTexture.loadFromFile("./hsliderTexture.png"));
    assert(knobTexture.loadFromFile("./knobTexture.png"));

    // Creating sprites
    sf::Sprite 
        idleButton(buttonTexture),
        hoverButton(buttonTexture),
        hitButton(buttonTexture);
    idleButton.setTextureRect(sf::IntRect(0, 50, 128, 30));
    hoverButton.setTextureRect(sf::IntRect(0, 177, 128, 30));
    hitButton.setTextureRect(sf::IntRect(0, 305, 128, 30));

    sf::Sprite vsliderSprite(vsliderTexture);
    sf::Sprite hsliderSprite(hsliderTexture);

    sf::CircleShape knobCircleShape;
    knobCircleShape.setRadius(50);

    sf::Font font;
    assert(font.loadFromFile("./FreeSans.otf"));

    // Constructing our awesome widgets
    ss::Button button(idleButton, hoverButton, hitButton);
    ss::Slider vslider(sf::RectangleShape(sf::Vector2f(25, 200)),
                       vsliderSprite, ss::Vertical);
    ss::Slider hslider(sf::RectangleShape(sf::Vector2f(200, 25)),
                       hsliderSprite, ss::Horizontal);
    ss::LineEdit lineEdit(sf::RectangleShape(sf::Vector2f(200, 25)),
                          sf::Text("ss::LineEdit!", font, 32));
    ss::Knob knob(knobCircleShape, sf::Sprite(knobTexture));

    // Preparation: settings the right position...
    button.setPosition(200, 100);
    vslider.setPosition(400, 300);
    hslider.setPosition(200, 200);
    lineEdit.setPosition(200, 400);
    knob.setPosition(300, 300);

    sf::RenderWindow window(sf::VideoMode(640, 480), "SSGUI is alive...");

    while (window.isOpen())  // Main application loop
    {
        sf::Event event;
        while (window.pollEvent(event))  // Event handling loop
        {
            if (event.type == sf::Event::Closed)
                window.close();

            // Awesome widgets should process some events to be happy
            button.handleEvent(event);
            vslider.handleEvent(event);
            hslider.handleEvent(event);
            lineEdit.handleEvent(event);
            knob.handleEvent(event);
        }
        
        // Updating our awesome widgets
        button.update(window);
        vslider.update(window);
        hslider.update(window);
        lineEdit.update(window);
        knob.update(window);

        // Drawing our awesome widgets
        window.clear(sf::Color(26, 26, 29));  // Clear with nice gray color
        window.draw(button);
        window.draw(vslider);
        window.draw(hslider);
        window.draw(lineEdit);
        window.draw(knob);
        window.display();
    }
}
```

Here's result of the next code.
[photo](https://github.com/artemisia0/SSGUI_SFML_GUI/blob/main/ssgui_example.png)

## License
License of SSGUI is the same as the license of SFML.
