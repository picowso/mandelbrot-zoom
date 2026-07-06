#include <SFML/Graphics.hpp>
#include <iostream>
// #include "header.hpp"
#include "SimpleIni.h"

struct Config {
  long double zoom_ptx = 0;
  long double zoom_pty = 1;
  long double start_xx = -2.5;
  long double start_xy = 1.0;
  long double start_yx = -1.5;
  long double start_yy = 1.5; // i lwk dont know wtf these are, me too (vicis)
  float coefficient = 0.9;
  int iters_n = 50;
};

Config getConfig(){
  Config cfg, base_cfg;
  CSimpleIniA ini;
	SI_Error rc = ini.LoadFile("../config.ini");
	if (rc < 0) 
  {
    std::cout << "How did SimpleIni error ??\n"; 
    return base_cfg;
  };

  cfg.zoom_ptx    = std::stold(ini.GetValue("mandelbrot", "zoom_ptx", "-1"));
  cfg.zoom_pty    = std::stold(ini.GetValue("mandelbrot", "zoom_pty", "-1"));
  cfg.start_xx    = std::stold(ini.GetValue("mandelbrot", "start_xx", "-1"));
  cfg.start_xy    = std::stold(ini.GetValue("mandelbrot", "start_xy", "-1"));
  cfg.start_yx    = std::stold(ini.GetValue("mandelbrot", "start_yx", "-1"));
  cfg.start_yy    = std::stold(ini.GetValue("mandelbrot", "start_yy", "-1"));
  cfg.coefficient = std::stold(ini.GetValue("mandelbrot", "coefficient", "-1"));
  cfg.iters_n     = std::stold(ini.GetValue("mandelbrot", "iters_n", "-1"));
  bool bad = cfg.zoom_ptx == -1
    or cfg.zoom_pty == -1
    or cfg.start_xx == -1
    or cfg.start_xy == -1
    or cfg.start_yx == -1
    or cfg.start_yy == -1
    or cfg.coefficient == -1
    or cfg.iters_n == -1;
  if (bad){
    std::cout << "cfg is bad"; 
    return base_cfg;
  }
  return cfg;
}

using std::swap;

constexpr int WINDOW_WIDTH = 640;
constexpr int WINDOW_HEIGHT = 480;
long double xx, xy, yx, yy; // = -2.5, xy = 1.0, yx = -1.5, yy = 1.5; // i lwk dont know wtf these are, me too (vicis)

sf::Color getColor(long double x, long double y, Config cfg) {
    long double zx = 0., zy = 0.;
    int j = 0;
    for(; j < cfg.iters_n ; j++) {
        long double zzx = zx;
        zx = zx*zx - zy*zy + x;
        zy = 2*zzx*zy + y;
        if(zx*zx + zy*zy > 4) break;
    }
    float scale = 1.0 - static_cast<float>(j)/static_cast<float>(cfg.iters_n);
    float r = scale * scale * scale, g = scale * scale, b = scale;
    sf::Color color(255.0 * r, 255.0 * g, 255.0 * b);
    return color;
}

void zoomIn(Config cfg) {
    xx = cfg.zoom_ptx + (xx - cfg.zoom_ptx) * cfg.coefficient;
    xy = cfg.zoom_ptx + (xy - cfg.zoom_ptx) * cfg.coefficient;
    yx = cfg.zoom_pty + (yx - cfg.zoom_pty) * cfg.coefficient;
    yy = cfg.zoom_pty + (yy - cfg.zoom_pty) * cfg.coefficient;
}

void getRawImage(int arr[WINDOW_WIDTH][WINDOW_HEIGHT][3], Config cfg) {
    for(int k = 0 ; k < WINDOW_WIDTH ; k++) {
        for(int j = 0 ; j < WINDOW_HEIGHT ; j++) {
            long double x = xx + ((k+1)*(xy-xx))/WINDOW_WIDTH;
            long double y = yx + ((j+1)*(yy-yx))/WINDOW_HEIGHT;
            sf::Color v = getColor(x, y, cfg);
            arr[k][j][0] = v.r;
            arr[k][j][1] = v.g;
            arr[k][j][2] = v.b;
        }
    }
}


[[deprecated("AA does the job better now")]] void gaussianBlur(int result[WINDOW_WIDTH][WINDOW_HEIGHT][3], int original[WINDOW_WIDTH][WINDOW_HEIGHT][3]) {
    const int stab = 6;
    for(int k = 0 ; k < WINDOW_WIDTH ; k++) {
        for(int j = 0 ; j < WINDOW_HEIGHT ; j++) {
            for(int q = 0 ; q < 3 ; q++) {
                if (k==0 || j==0 || k==WINDOW_WIDTH-1 || j==WINDOW_HEIGHT-1) {
                    result[k][j][q]=original[k][j][q];
                }
                result[k][j][q] = (stab*original[k][j][q]+original[k+1][j][q]+original[k-1][j][q]+original[k+1][j+1][q]+original[k-1][j+1][q]+original[k+1][j-1][q]+original[k-1][j-1][q]+original[k][j+1][q]+original[k][j-1][q])/(stab+8);
                // to improve but this works now sadly
            }
        }
    }
}

void renderImage(int image[WINDOW_WIDTH][WINDOW_HEIGHT][3], sf::RenderWindow &window) {
    sf::VertexArray toDraw(sf::PrimitiveType::Points, WINDOW_HEIGHT*WINDOW_WIDTH);
    for(int k = 0 ; k < WINDOW_WIDTH ; k++) {
        for(int j = 0 ; j < WINDOW_HEIGHT ; j++) {
            sf::Vertex pixel;
            pixel.color.r=image[k][j][0];
            pixel.color.g=image[k][j][1];
            pixel.color.b=image[k][j][2];
            pixel.position={static_cast<float>(k),static_cast<float>(j)};
            toDraw[WINDOW_HEIGHT*k+j]=pixel;
        }
    }
    window.draw(toDraw);
}

int main()
{
    sf::ContextSettings settings;
    settings.antiAliasingLevel = 4.0;
    sf::RenderWindow window( sf::VideoMode( { WINDOW_WIDTH, WINDOW_HEIGHT } ), "Mandelbrot zoom",
        sf::Style::Close, sf::State::Windowed, settings);
    window.setFramerateLimit(60);
    int frame = 0;
    bool pause = false;
    Config cfg = getConfig();
    xx = cfg.start_xx;
    xy = cfg.start_xy;
    yx = cfg.start_yx;
    yy = cfg.start_yy;
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>()) window.close();
            bool pressed_key = event->is<sf::Event::KeyPressed>();
            if (pressed_key)
            {
              auto ev = event->getIf<sf::Event::KeyPressed>();
              if (ev->code == sf::Keyboard::Key::Space) 
              {
                pause = !pause;
              }
              if (ev->code == sf::Keyboard::Key::R) 
              {
                cfg = getConfig();
                xx = cfg.start_xx;
                xy = cfg.start_xy;
                yx = cfg.start_yx;
                yy = cfg.start_yy;
              }
            }
        }

        window.clear(sf::Color::Black);
        int Image[WINDOW_WIDTH][WINDOW_HEIGHT][3] = {};
        getRawImage(Image, cfg);
        renderImage(Image, window);
        frame++;
        if (!pause) zoomIn(cfg);
        window.display();
    }
}

