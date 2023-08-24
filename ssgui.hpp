// License is the same as SFML has.

// Written by artemisia0

// ssgui.hpp - Smallest and Simplest GUI for SFML
// It is image-based, to create a button you need three sprites
// The best way to use it is to find
// some button/knob textures and use ssgui for game ui


// This library now has only a few classes:
//      ss::Button      - clickable button
//      ss::Knob        - dragable or scrollable knob
//      ss::Slider      - either vertical or horizontal dragable slider
//      ss::LineEdit    - simple unicode text entry (use of sf::Text/String)

// Feel free to modify it. It is free and open-source.
// Some widgets are absent.
// You can contribute this library and add smth like ss::Menu for example

// ssgui is header-only library. #define SSGUI_IMPL to include implementation

// #define SSGUI_IMPL

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef SSGUI_HPP
#define SSGUI_HPP

#include <functional>
#include <memory>

#include <cassert>
#include <cmath>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/Event.hpp>


namespace ss  // Interface goes here
{
    enum State  // You can attach different sprites to widget states
    {
        Idle        = 0,
        Hover       = 1,
        Hit         = 2,
    };

    enum SliderType
    {
        Horizontal  = 0,
        Vertical    = 1,
    };

    // Some compile-time config. It is OK by default
    constexpr unsigned StateCount = 3;
    constexpr float KnobDragSensitivity = 0.01f;
    constexpr float KnobScrollSensitivity = 0.1f;
    constexpr float KnobMaxMouseWheelScrollDelta = 0.1f;
    constexpr float KnobMaxMouseMoveDelta = 0.1f;

    // Base/super class for all other widgets
    class AbstractWidget : public sf::Drawable, public sf::Transformable
    {
        public:
            virtual void handleEvent(const sf::Event&) = 0;
            virtual void update(const sf::Window&) = 0;


        protected:
            virtual void draw(sf::RenderTarget&, sf::RenderStates) const = 0;
    };

    // Something that can be hovered/clicked. Button, Knob, Slider inherit it.
    template <typename T>
    class Clickable : public AbstractWidget
    {
        private:
            using Callback = std::function<void(void)>;


        public:
                                // Clickable is invisible widget.
                                // It only knows it's state (idle/hover/hit).
                                Clickable(T collisionShape=T());


        public:
            // It is possible to attach callback to a state
            void                bind(State, Callback);

            // Freezing widget in some state
            // means widget won't change it's new state.
            // It is like disabling handleEvent/update methods
            void                freeze(State);

            // Unfreezing is enabling update/handleEvent methods.
            // Unfreezed object can change it's state (idle/hover/hit)
            void                unfreeze();

            virtual void        handleEvent(const sf::Event&) override;

            // Takes a reference to window because it will need to get
            // relative mouse position
            virtual void        update(const sf::Window&) override;

            // Observer methods
            const T&            collisionShape() const;
            State               state() const;
            const Callback&     callback(State) const;
            bool                freezed() const;


        protected:
            // Declared in sf::Drawable (base class of ss::AbstractWidget)
            virtual void        draw(sf::RenderTarget&,
                                    sf::RenderStates) const override;


        private:
            // Call a callback with respect to a current state
            void                call();


        private:
            T                   mCollisionShape;
            State               mState;
            Callback            mCallbacks[StateCount];
            bool                mFreezed;
    };

    // Center origin with respect to an object's local bounds
    template <typename T>
    void centerOrigin(T&);

    // Euclidean distance between two generic vectors
    template <typename A, typename B>
    float distance(A, B);

    // Checks for collision of generic shape with a point (vector)
    template <typename T>
    bool contains(const T&, sf::Vector2i);

    // Specialization for sf::CircleShape
    template <>
    bool contains<sf::CircleShape>(const sf::CircleShape&, sf::Vector2i);

    // Clickable button that has a sprite for every state (idle/hover/hit)
    class Button : public Clickable<sf::Sprite>
    {
        public:
            // Default values are dummies
            // Button should be constructed properly for adequate use
            // Button collision shape is a shape of sprite for idle state
            Button(sf::Sprite idle=sf::Sprite(),
                   sf::Sprite hover=sf::Sprite(),
                   sf::Sprite hit=sf::Sprite());


        public:
            virtual void update(const sf::Window&) override;

            const sf::Sprite& sprite(State) const;


        protected:
            virtual void draw(
                sf::RenderTarget&, sf::RenderStates) const override;


        private:
            sf::Sprite mSprites[StateCount];
    };

    // Dragable/scrollable knob that does use
    // of sf::CircleShape as a collisionShape.
    // It's spritesheet must be (vertical) of size 1 (width) x N (height) 
    // Single sprite in that spritesheet must be a square (N x N pixels)
    class Knob : public Clickable<sf::CircleShape>
    {
        public:
            // Should be constructed property to work well
            // Default parameter values are dummies
            Knob(sf::CircleShape collisionShape=sf::CircleShape(),
                 sf::Sprite sprite=sf::Sprite());
 

        public:
            virtual void handleEvent(const sf::Event&) override;
            virtual void update(const sf::Window&) override;

            // Like an angle of knob but in range [-1.0; 1.0]
            float value() const;

            void setValue(float);


        protected:
            virtual void draw(
                sf::RenderTarget&, sf::RenderStates) const override;


        private:
            // Calculates a sprite texture rectangle with respect to mValue
            sf::IntRect textureRect();


        private:
            sf::Sprite mSprite;
            float mValue;  // Like a knob angle but in range [-1.0; 1.0]
            float mPreviousMouseY;  // Used in update method
    };

    // Dragable slider that can be either vertical or horizontal
    // May be used as a progress bar also: freeze it and setValue
    class Slider : public Clickable<sf::RectangleShape>
    {
        public:
            Slider();
            Slider(sf::RectangleShape collisionShape,
                sf::Sprite sprite,
                SliderType type);


        public:
            virtual void update(const sf::Window&) override;

            float value() const;
            void setValue(float);


        protected:
            virtual void draw(
                sf::RenderTarget&, sf::RenderStates) const override;


        private:
            bool mInitialized;  // Constructed with default constructor?
            float mValue;  // Slider progress
            sf::Sprite mSprite;
            SliderType mType;  // Vertical/Horizontal
    };

    // Can be used to write some unicode text
    // Backspace erases last character if there is one
    class LineEdit : public Clickable<sf::RectangleShape>
    {
        public:
            LineEdit();
            LineEdit(sf::RectangleShape collisionShape, sf::Text text);


        public:
            virtual void        handleEvent(const sf::Event&) override;
            virtual void        update(const sf::Window&) override;

            void                setString(const sf::String&);
            const sf::String&   string() const;


        protected:
            virtual void draw(
                sf::RenderTarget&, sf::RenderStates) const override;


        private:
            bool mInitialized;  // Constructed with default constructor?
            sf::Text mText;
    };
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifdef SSGUI_IMPL  // Implementation starts here

namespace ss
{
    template <typename T>
    Clickable<T>::Clickable(T collisionShape)
    : mCollisionShape(std::move(collisionShape))
    , mState(Idle)
    , mCallbacks{[](){}, [](){}, [](){}}
    , mFreezed(false)
    {
    }

    template <typename T>
    void Clickable<T>::bind(State state, Callback callback)
    {
        mCallbacks[static_cast<unsigned>(state)] = std::move(callback);
    }

    template <typename T>
    void Clickable<T>::freeze(State state)
    {
        mState = state;
        mFreezed = true;
    }

    template <typename T>
    void Clickable<T>::unfreeze()
    {
        mFreezed = false;
    }

    template <typename T>
    void Clickable<T>::handleEvent(const sf::Event& event)
    {
        if (mFreezed)
            return;

        if (event.type == sf::Event::MouseButtonPressed)
        {
            if (mState == Hover 
                and event.mouseButton.button == sf::Mouse::Left)
            {
                mState = Hit;
                call();
            }
        }
        else if (event.type == sf::Event::MouseButtonReleased)
        {
            if (mState == Hit
                and event.mouseButton.button == sf::Mouse::Left)
            {
                mState = Hover;
                call();
            }
        }
    }

    template <typename T>
    void Clickable<T>::update(const sf::Window& window)
    {
        mCollisionShape.setPosition(getPosition());
        mCollisionShape.setScale(getScale());
        centerOrigin(mCollisionShape);

        if (mFreezed)
            return;

        if (mState == Idle 
            and contains(mCollisionShape, sf::Mouse::getPosition(window)))
        {
            mState = Hover;
            call();
        }
        else if (mState == Hover
            and not contains(mCollisionShape, sf::Mouse::getPosition(window)))
        {
            mState = Idle;
            call();
        }
    }

    template <typename T>
    const T& Clickable<T>::collisionShape() const
    {
        return mCollisionShape;
    }

    template <typename T>
    State Clickable<T>::state() const
    {
        return mState;
    }

    template <typename T>
    bool Clickable<T>::freezed() const
    {
        return mFreezed;
    }

    template <typename T>
    const typename Clickable<T>::Callback& Clickable<T>::callback(
                                                            State state) const
    {
        return mCallbacks[static_cast<unsigned>(state)];
    }

    template <typename T>
    void Clickable<T>::draw(
        [[maybe_unused]] sf::RenderTarget& target,
        [[maybe_unused]] sf::RenderStates states) const
    {
    }

    template <typename T>
    void Clickable<T>::call()
    {
        mCallbacks[static_cast<unsigned>(mState)]();
    }

    template <typename T>
    void centerOrigin(T& object)
    {
        const auto rect = object.getLocalBounds();
        object.setOrigin(   rect.left + static_cast<unsigned>(rect.width)/2,
                            rect.top  + static_cast<unsigned>(rect.height)/2);
    }

    template <typename A, typename B>
    float distance(A a, B b)
    {
        return sqrt(pow(a.x-b.x, 2) + pow(a.y-b.y, 2));
    }

    template <typename T>
    bool contains(const T& shape, sf::Vector2i point)
    {
        return shape.getGlobalBounds()
            .contains(static_cast<sf::Vector2f>(point));
    }

    template <>
    bool contains<sf::CircleShape>(
        const sf::CircleShape& shape, sf::Vector2i point)
    {
        return distance(point, shape.getPosition()) < shape.getRadius();
    }

    Button::Button(sf::Sprite idle, sf::Sprite hover, sf::Sprite hit)
    : Clickable(idle)
    , mSprites{std::move(idle), std::move(hover), std::move(hit)}
    {
    }

    const sf::Sprite& Button::sprite(State state) const
    {
        return mSprites[static_cast<unsigned>(state)];
    }

    void Button::update(const sf::Window& window)
    {
        Clickable::update(window);
        Clickable::setPosition(getPosition());
        Clickable::setScale(getScale());
        for (auto& sprite : mSprites)
        {
            sprite.setPosition(getPosition());
            sprite.setScale(getScale());
            centerOrigin(sprite);
        }
    }

    void Button::draw(
        sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(mSprites[static_cast<unsigned>(state())], states);
    }

    Knob::Knob(sf::CircleShape collisionShape, sf::Sprite sprite)
    : Clickable(std::move(collisionShape))
    , mSprite(std::move(sprite))
    , mPreviousMouseY(0.f)
    {
    }

    void Knob::handleEvent(const sf::Event& event)
    {
        Clickable::handleEvent(event);

        if (state() == Hover
            and event.type == sf::Event::MouseWheelScrolled
            and event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
            mValue +=
                fmin(event.mouseWheelScroll.delta * KnobScrollSensitivity,
                     KnobMaxMouseWheelScrollDelta);

        if (state() == Hit and event.type != sf::Event::MouseButtonPressed)
            mValue += fmin(
                (mPreviousMouseY - event.mouseMove.y) * KnobDragSensitivity,
                KnobMaxMouseMoveDelta);

        mValue = fmax(-1.f, fmin(mValue, 1.f));
    }

    void Knob::update(const sf::Window& window)
    {
        Clickable::update(window);

        mValue = fmax(-1.f, fmin(mValue, 1.f));
        mSprite.setTextureRect(textureRect());
        mSprite.setPosition(getPosition());
        mSprite.setScale(getScale());
        centerOrigin(mSprite);
        mPreviousMouseY = sf::Mouse::getPosition(window).y;
    }

    float Knob::value() const
    {
        return mValue;
    }

    void Knob::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(mSprite, states);
    }

    sf::IntRect Knob::textureRect()
    {
        assert(mSprite.getTexture());
        const auto [w, h] = mSprite.getTexture()->getSize();
        const auto top = roundf((h/w - 1)*(mValue+1.f)/2)*w;
        return sf::IntRect(0, top, w, w);
    }

    void Knob::setValue(float value)
    {
        assert(value <= 1.0 and value >= 0.0);
        mValue = (value-0.5f)*2;
    }

    Slider::Slider()
    : Clickable(sf::RectangleShape(sf::Vector2f(1.f, 1.f)))
    , mInitialized(false)
    {
    }

    Slider::Slider(sf::RectangleShape collisionShape,
                    sf::Sprite sprite,
                    SliderType type)
    : Clickable(collisionShape)
    , mInitialized(true)
    , mValue(0.f)
    , mSprite(std::move(sprite))
    , mType(type)
    {
    }

    void Slider::update(const sf::Window& window)
    {
        assert(mInitialized);
        Clickable::setPosition(getPosition());
        Clickable::setScale(getScale());
        Clickable::update(window);
        mSprite.setPosition(getPosition());
        mSprite.setScale(getScale());
        centerOrigin(mSprite);

        if (mType == Horizontal)
        {
            if (state() == Hit)
            {
                float value
                    = (sf::Mouse::getPosition(window).x - getPosition().x)
                    / collisionShape().getSize().x;
                mValue = value*2/getScale().x;
            }
        }
        else if (mType == Vertical)
        {
            if (state() == Hit)
            {
                float value
                    = (getPosition().y - sf::Mouse::getPosition(window).y)
                    / collisionShape().getSize().y;
                mValue = value*2/getScale().y;
            }
        }

        mValue = fmax(-1.f, fmin(mValue, 1.f));

        const auto [w, h] = mSprite.getTexture()->getSize();
        const auto top = roundf((h/w - 1)*(mValue+1.f)/2)*w;
        mSprite.setTextureRect(sf::IntRect(0, top, w, w));
    }

    float Slider::value() const
    {
        assert(mInitialized);
        return mValue;
    }

    void Slider::setValue(float value)
    {
        assert(mInitialized);
        mValue = value;
    }

    void Slider::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        assert(mInitialized);
        target.draw(mSprite, states);
    }

    LineEdit::LineEdit()
    : Clickable(sf::RectangleShape(sf::Vector2f(1.f, 1.f)))
    , mInitialized(false)
    {
    }

    LineEdit::LineEdit(sf::RectangleShape collisionShape, sf::Text text)
    : Clickable(std::move(collisionShape))
    , mInitialized(true)
    , mText(std::move(text))
    {
    }

    void LineEdit::handleEvent(const sf::Event& event)
    {
        assert(mInitialized);
        Clickable::handleEvent(event);
        
        if (state() == Hover)
        {
            if (event.type == sf::Event::TextEntered
                and static_cast<char>(event.text.unicode) >= 32)
            {
                mText.setString(mText.getString() + event.text.unicode);
            }
            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Backspace
                    and mText.getString().getSize() > 0)
                {
                    sf::String string = mText.getString();
                    string.erase(string.getSize()-1);
                    mText.setString(std::move(string));
                }
            }
        }
    }

    void LineEdit::update(const sf::Window& window)
    {
        assert(mInitialized);
        Clickable::setPosition(getPosition());
        Clickable::setScale(getScale());
        Clickable::update(window);
        mText.setPosition(getPosition());
        mText.setScale(getScale());
        centerOrigin(mText);
    }

    void LineEdit::setString(const sf::String& string)
    {
        assert(mInitialized);
        mText.setString(string);
    }

    const sf::String& LineEdit::string() const
    {
        assert(mInitialized);
        return mText.getString();
    }

    void LineEdit::draw(
        sf::RenderTarget& target, sf::RenderStates states) const
    {
        assert(mInitialized);
        target.draw(mText, states);
    }
}

#endif  // SSGUI_IMPL

#endif  // SSGUI_HPP

