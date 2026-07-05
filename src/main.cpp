#include <SFML/Graphics.hpp>
#include "header.hpp"

using std::swap;

constexpr int WINDOW_WIDTH = 640;
constexpr int WINDOW_HEIGHT = 480;
long double xx = -2.5, xy = 1.0, yx = -1.5, yy = 1.5; // i lwk dont know wtf these are
sf::Color getColor(long double x, long double y) {
    long double zx = 0., zy = 0.;
    for(int j = 0 ; j < iters_n ; j++) {
        long double zzx = zx;
        zx = zx*zx - zy*zy + x;
        zy = 2*zzx*zy + y;
        if(zx*zx + zy*zy > 4) return sf::Color::White;
    }
    return sf::Color::Black;
}
void zoomIn(long double coefficient) {
    xx = zom_ptx + (xx - zom_ptx) * coefficient;
    xy = zom_ptx + (xy - zom_ptx) * coefficient;
    yx = zom_pty + (yx - zom_pty) * coefficient;
    yy = zom_pty + (yy - zom_pty) * coefficient;
}
void getRawImage(int arr[WINDOW_WIDTH][WINDOW_HEIGHT][3]) {
    for(int k = 0 ; k < WINDOW_WIDTH ; k++) {
        for(int j = 0 ; j < WINDOW_HEIGHT ; j++) {
            long double x = xx + ((k+1)*(xy-xx))/WINDOW_WIDTH;
            long double y = yx + ((j+1)*(yy-yx))/WINDOW_HEIGHT;
            // cout << yay << endl;
            sf::Color v = getColor(x, y);
            arr[k][j][0] = v.r;
            arr[k][j][1] = v.g;
            arr[k][j][2] = v.b;
        }
    }
}
void gaussianBlur(int result[WINDOW_WIDTH][WINDOW_HEIGHT][3], int original[WINDOW_WIDTH][WINDOW_HEIGHT][3]) {
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
    settings.antiAliasingLevel = 8.0;
    sf::RenderWindow window( sf::VideoMode( { WINDOW_WIDTH, WINDOW_HEIGHT } ), "Mandelbrot zoom",
        sf::Style::Close, sf::State::Windowed, settings);
    window.setFramerateLimit(60);
    int frame = 0;
    while ( window.isOpen() )
    {
        while ( const std::optional event = window.pollEvent() )
        {
            if ( event->is<sf::Event::Closed>() )
                window.close();
        }

        window.clear(sf::Color::Black);
        int beforeGaussian[WINDOW_WIDTH][WINDOW_HEIGHT][3] = {};
        getRawImage(beforeGaussian);
        int afterGaussian[WINDOW_WIDTH][WINDOW_HEIGHT][3] = {};
        gaussianBlur(afterGaussian,beforeGaussian);
        renderImage(afterGaussian,window);

        sf::Texture texture(window.getSize());
        texture.update(window);
        if (texture.copyToImage().saveToFile(std::format("Frame {}.png", frame)))
        {
        }
        else {
            throw std::runtime_error("Failed to save frame");
        }
        frame++;
        zoomIn(0.9);
        window.display();
    }
}

