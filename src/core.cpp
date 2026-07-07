#include "core.h"

void State::loadIni(const char* ini_file) 
{
    using std::stold, std::to_string;
    CSimpleIniA ini;
    SI_Error rc = ini.LoadFile(ini_file);
    if (rc < 0) { return; }
    
    top_left.re  = stold(ini.GetValue("config", "top_left_re",  to_string(top_left.re).c_str()));
    top_left.im  = stold(ini.GetValue("config", "top_left_im",  to_string(top_left.im).c_str()));

    bot_right.re = stold(ini.GetValue("config", "bot_right_re", to_string(bot_right.re).c_str()));
    bot_right.im = stold(ini.GetValue("config", "bot_right_im", to_string(bot_right.im).c_str()));

    goal_pt.re   = stold(ini.GetValue("config", "goal_pt_re",   to_string(goal_pt.re).c_str()));
    goal_pt.im   = stold(ini.GetValue("config", "goal_pt_im",   to_string(goal_pt.im).c_str()));

    coefficient  = stold(ini.GetValue("config", "coefficient",  to_string(coefficient).c_str()));
    max_iter     = stold(ini.GetValue("config", "max_iter",     to_string(max_iter).c_str()));
    isFancy      = stold(ini.GetValue("config", "isFanchy",    to_string(isFancy).c_str()));
}

void State::zoomIn()
{
    top_left = goal_pt + (top_left - goal_pt) * coefficient;;
    bot_right = goal_pt + (bot_right - goal_pt) * coefficient;;
}

Fractal::Fractal(unsigned int w, unsigned int h, const char* title)
{
    sf::ContextSettings settings;
    settings.antiAliasingLevel = 4.0;
    window_width = w, window_height = h;
    window = sf::RenderWindow(sf::VideoMode({window_width, window_height}), title, sf::Style::Close, sf::State::Windowed, settings);
    window.setFramerateLimit(60);
    state.loadIni("../configs/mandelbrot.ini");
    pause = false;
}

void Fractal::Run() 
{
    while (window.isOpen()) 
    {
        while (const std::optional event = window.pollEvent()) 
        {
            if (event->is<sf::Event::Closed>()) window.close();
            bool pressed_key = event->is<sf::Event::KeyPressed>();
            if (pressed_key) 
            {
                auto ev = event->getIf<sf::Event::KeyPressed>();
                if (ev->code == sf::Keyboard::Key::Space) { pause = !pause; }
            }
        }

        window.clear(sf::Color::Black);
        sf::VertexArray toDraw(sf::PrimitiveType::Points, window_height * window_width);
        for (int k = 0; k < window_width; k++)
        {
            for (int j = 0; j < window_height; j++) 
            {
                long double px = static_cast<long double>(k + 1)/static_cast<long double>(window_width);
                long double py = static_cast<long double>(j + 1)/static_cast<long double>(window_height);
                long double x = state.top_left.re + px * (state.bot_right.re - state.top_left.re);
                long double y = state.top_left.im + py * (state.bot_right.im - state.top_left.im);
                sf::Vertex pixel;
                pixel.color = state.isFancy ? getColorFancy(x, y, state) : getColor(x, y, state);
                pixel.position = { static_cast<float>(k), static_cast<float>(j) };
                toDraw[window_height * k + j] = pixel;
            }
        }
        window.draw(toDraw);
        if (!pause) state.zoomIn();
        window.display();
    }
}

sf::Color colorMap(int j, State const& state) 
{
    float x = static_cast<float>(j)/state.max_iter;
    float scale = 1.0 - x;
    scale = pow(scale,2.2);
    float r = scale, g = scale, b = scale;
    sf::Color color(255.0 * r, 255.0 * g, 255.0 * b);
    return color;
}

sf::Color getColor(long double x, long double y, State const &state)
{
    long double zx = 0., zy = 0.;
    long double zx2 = 0, zy2 = 0;
    int j = 0;
    for (; j < state.max_iter; j++) 
    {
        zx2 = zx * zx;
        zy2 = zy * zy;
        zy = 2 * zx * zy + y;
        zx = zx2 - zy2 + x;
        if (zx2 + zy2 > 4) break;
    }
    return colorMap(j, state);
}

sf::Color getColorFancy(long double x, long double y, State const &state) {
    long double zx = 0., zy = 0.;
    long double zx2 = 0, zy2 = 0;
    long double j = 0;
    for (; j < state.max_iter; j++) {
        zx2 = zx * zx;
        zy2 = zy * zy;
        zy = 2 * zx * zy + y;
        zx = zx2 - zy2 + x;
        if (zx2 + zy2 > 1e5) break;
    }
    if (j < state.max_iter) {
        long double log_zn = log2l(zx2 + zy2) / 2;
        j += 1 - log2l(log_zn);
    }
    return colorMapSmart(j, state);
}

sf::Color weightedColorAverage(const sf::Color& a, const sf::Color& b, float p)
{
    return sf::Color(
        (1.0f - p) * a.r + p * b.r,
        (1.0f - p) * a.g + p * b.g,
        (1.0f - p) * a.b + p * b.b
    );
}

sf::Color colorMapDumb(int j, State const& state) {
    float x = static_cast<float>(j)/state.max_iter;
    float scale = 1.0 - x;
    float r = scale * scale * scale, g = scale * scale, b = scale;
    sf::Color color(255.0 * r, 255.0 * g, 255.0 * b);
    return color;
}

sf::Color colorMapSmart(float j, State const& state){
    long double t = 15.0 * j/state.max_iter;
    if (j < state.max_iter)
    {
        sf::Color mapping[16];
        mapping[0] = sf::Color(66, 30, 15);
        mapping[1] = sf::Color(25, 7, 26);
        mapping[2] = sf::Color(9, 1, 47);
        mapping[3] = sf::Color(4, 4, 73);
        mapping[4] = sf::Color(0, 7, 100);
        mapping[5] = sf::Color(12, 44, 138);
        mapping[6] = sf::Color(24, 82, 177);
        mapping[7] = sf::Color(57, 125, 209);
        mapping[8] = sf::Color(134, 181, 229);
        mapping[9] = sf::Color(211, 236, 248);
        mapping[10] = sf::Color(241, 233, 191);
        mapping[11] = sf::Color(248, 201, 95);
        mapping[12] = sf::Color(255, 170, 0);
        mapping[13] = sf::Color(204, 128, 0);
        mapping[14] = sf::Color(153, 87, 0);
        mapping[15] = sf::Color(106, 52, 3);
        int k = floor(t);
        if (k == 15) return mapping[15];
        long double p = t - k;
        return weightedColorAverage(mapping[k], mapping[k+1], p);
    }
    return sf::Color::Black;
}


