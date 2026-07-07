#ifndef CORE_H
#define CORE_H

#include "SimpleIni.h"
#include <SFML/Graphics.hpp>
#include <iostream>

struct Complexe 
{ 
    long double re, im;

    Complexe operator+(const Complexe& z) const { return Complexe { re + z.re, im + z.im }; }
    Complexe operator-(const Complexe& z) const { return Complexe { re - z.re, im - z.im }; }
    Complexe operator*(float p) const { return Complexe { re * p, im * p }; }
};

struct State
{
    Complexe top_left, bot_right, goal_pt;
    float coefficient;
    size_t max_iter;
    bool isFancy;

    void loadIni(const char* ini_file);
    void zoomIn();
};

class Fractal
{
    public:
        Fractal(unsigned int w, unsigned int h, const char* title);
        void Run();

    private:
        sf::RenderWindow window;
        State state;
        unsigned int window_width; // = 640;
        unsigned int window_height; // = 480
        bool pause;
};

sf::Color colorMap(int j, State const& state);
sf::Color getColor(long double x, long double y, State const &state);
sf::Color getColorFancy(long double x, long double y, State const &state);
sf::Color weightedColorAverage(const sf::Color& a, const sf::Color& b, float p);
sf::Color colorMapDumb(int j, State const& state);
sf::Color colorMapSmart(float j, State const& state);

#endif // CORE_H
